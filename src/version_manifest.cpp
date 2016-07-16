/**
 * karazeh -- the library for patching software
 *
 * Copyright (C) 2011-2016 by Ahmad Amireh <ahmad@amireh.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "karazeh/version_manifest.hpp"

namespace kzh {
  typedef file_manager file_manager_t;
  typedef hasher       hasher_t;

  static const JSON::shape SCHEMA_IDENTITY_LIST = {
    { "name", JSON::STRING },
    { "files", JSON::ARRAY },
  };

  static const JSON::shape SCHEMA_RELEASE = {
    { "id", JSON::STRING },
    { "identity", JSON::STRING },
  };

  static const JSON::shape SCHEMA_CREATE_OPERATION = {
    { "source", JSON::OBJECT },
    { "destination", JSON::STRING },
  };

  static const JSON::shape SCHEMA_CREATE_OPERATION_SOURCE = {
    { "url", JSON::STRING },
    { "checksum", JSON::STRING },
    { "size", JSON::NUMBER },
  };

  static const JSON::shape SCHEMA_UPDATE_OPERATION = {
    { "basis", JSON::OBJECT },
    { "delta", JSON::OBJECT },
  };

  static const JSON::shape SCHEMA_UPDATE_OPERATION_BASIS = {
    { "pre_checksum", JSON::STRING },
    { "post_checksum", JSON::STRING },
    { "filepath", JSON::STRING },
  };

  static const JSON::shape SCHEMA_UPDATE_OPERATION_DELTA = {
    { "checksum", JSON::STRING },
    { "size", JSON::NUMBER },
    { "url", JSON::STRING },
  };

  static const JSON::shape SCHEMA_DELETE_OPERATION = {
    { "target", JSON::STRING },
  };

  static const auto validate_schema = [](JSON const& node, JSON::shape const& schema) -> void {
    string_t schema_error;

    if (!node.has_shape(schema, schema_error)) {
      throw invalid_manifest("Malformed entity: " + schema_error);
    }
  };

  version_manifest::version_manifest(config_t const& config)
  : config_(config) {
  }

  version_manifest::~version_manifest() {
    identity_lists_.clear();

    while (!releases_.empty()) {
      delete releases_.back();
      releases_.pop_back();
    }
  }

  void
  version_manifest::load_from_uri(string_t const& uri) {
    string_t buffer;

    if (!config_.downloader->fetch(uri, buffer)) {
      throw invalid_resource(uri);
    }

    return load_from_string(buffer);
  }

  void
  version_manifest::load_from_string(string_t const& json_string) {
    string_t err;

    JSON manifest = JSON::parse(json_string, err);

    if (!err.empty()) {
      throw invalid_manifest("JSON Parse error: " + err);
    }

    return parse(manifest);
  }

  void
  version_manifest::load_release_from_string(string_t const& json_string) {
    string_t err;

    JSON manifest = JSON::parse(json_string, err);

    if (!err.empty()) {
      throw invalid_manifest("JSON Parse error: " + err);
    }

    for (auto release_node : manifest["releases"].array_items()) {
      parse_release(release_node);
    }
  }

  void
  version_manifest::load_release_from_uri(string_t const& uri) {
    string_t buffer;

    if (!config_.downloader->fetch(uri, buffer)) {
      throw invalid_resource(uri);
    }

    load_release_from_string(buffer);
  }

  void
  version_manifest::parse(JSON const& manifest) {
    const JSON::array &identity_list_nodes = manifest["identities"].array_items();
    const JSON::array &release_nodes = manifest["releases"].array_items();

    if (identity_list_nodes.size() == 0) {
      throw invalid_manifest("Version manifest must contain at least one identity list.");
    }

    if (release_nodes.size() == 0) {
      throw invalid_manifest("Version manifest must contain at least one release entry.");
    }

    for (auto identity_list_node : identity_list_nodes) {
      validate_schema(identity_list_node, SCHEMA_IDENTITY_LIST);

      const JSON::array &file_nodes = identity_list_node["files"].array_items();

      if (file_nodes.size() == 0) {
        throw invalid_manifest(
          "Identity list (" + identity_list_node["name"].string_value() +
          ") must contain at least one file entry."
        );
      }

      identity_list_t identity_list(identity_list_node["name"].string_value());

      for (auto identity_file_node : file_nodes) {
        path_t identity_file = (config_.root_path / identity_file_node.string_value());

        if (!config_.file_manager->is_readable(identity_file)) {
          throw std::domain_error("Identity file " + identity_file.string() + " is not readable.");
        }

        identity_list.files.push_back(identity_file);
      }

      // TODO: optimize
      identity_lists_.insert({ identity_list.name, identity_list });
    }

    // Now that the identity lists are built, we run through all the release entries
    // and attempt to find one whose checksum matches that of the identity list it
    // points to
    for (auto release_node : release_nodes) {
      find_or_create_release(release_node);
    }
  }

  void
  version_manifest::parse_release(const JSON& manifest) {
    find_or_create_release(manifest);
  }

  int
  version_manifest::get_release_count() const {
    return releases_.size();
  }

  int
  version_manifest::get_release_count(string_t const& identity) const {
    int count = 0;

    for (auto release : releases_) {
      count += release->identity == identity ? 1 : 0;
    }

    return count;
  }

  string_t
  version_manifest::get_current_version() const {
    const hasher_t *hasher = config_.hasher;
    const file_manager_t *file_manager = config_.file_manager;

    map<string_t, string_t> versions;

    for (auto identity_list_entry : identity_lists_) {
      string_t list_checksum;

      auto identity_list = identity_list_entry.second;

      for (auto identity_file : identity_list.files) {
        if (!file_manager->is_readable(identity_file)) {
          throw std::domain_error("Identity file " + identity_file.string() + " is not readable.");
        }

        hasher_t::digest_rc rc = hasher->hex_digest(identity_file);

        if (!rc.valid) {
          throw std::domain_error("Identity file " + identity_file.string() + " could not be digested.");
        }

        list_checksum += rc.digest;
      }

      versions.insert({
        identity_list.name,
        hasher->hex_digest(list_checksum).digest
      });
    }

    // Now that the identity list checksums are calculated, we run through all
    // the release entries and attempt to find one whose checksum matches that
    // of the identity list it points to
    for (auto release : releases_) {
      const string_t &release_checksum = release->id;
      const string_t &identity_list_name = release->identity;
      const string_t &identity_list_checksum = versions.find(identity_list_name)->second;

      if (release_checksum == identity_list_checksum) {
        return release_checksum;
      }
    }

    return string_t();
  }

  vector<string_t>
  version_manifest::get_available_updates(string_t const &current_version) const {
    // we'll use the map for fast look-ups and off-load into a list at output
    // time
    map<string_t, const release_manifest*> update_map;
    vector<string_t> update_list;

    for (auto release : releases_) {
      if (release->head == current_version || update_map.find(release->head) != update_map.end()) {
        update_list.push_back(release->id);
        update_map.insert({ release->id, release });
      }
    }

    return update_list;
  }

  release_manifest*
  version_manifest::find_or_create_release(JSON const& release_node) {
    // TODO: we need to relax this restriction if the release is to be loaded
    // from multiple sources!
    validate_schema(release_node, SCHEMA_RELEASE);

    const string_t &identity_list_name = release_node["identity"].string_value();
    const JSON::array &operation_nodes = release_node["operations"].array_items();

    if (identity_lists_.find(identity_list_name) == identity_lists_.end()) {
      throw invalid_manifest(
        "Release (" + release_node["id"].string_value() + ") points " +
        "to an undefined identity list (" + identity_list_name + ")."
      );
    }

    release_manifest *release(nullptr);
    bool owned = false;

    for (auto existing_release : releases_) {
      if (existing_release->id == release_node["id"].string_value()) {
        release = existing_release;
      }
    }

    if (release == nullptr) {
      release = new release_manifest();
      owned = true;
    }

    try {
      if (release->identity.empty()) {
        release->identity = release_node["identity"].string_value();
      }

      if (release->id.empty()) {
        release->id = release_node["id"].string_value();
      }

      if (release->head.empty()) {
        release->head = release_node["head"].string_value();
      }

      if (release->tag.empty()) {
        release->tag = release_node["tag"].string_value();
      }

      if (release->uri.empty()) {
        release->uri = release_node["uri"].string_value();
      }

      for (auto operation_node : operation_nodes) {
        operation *op = parse_operation(*release, release_node, operation_node);

        if (op != nullptr) {
          release->operations.push_back(op);
        }
      }
    }
    catch(...) {
      if (owned) {
        delete release;
      }

      throw;
    }

    if (owned) {
      releases_.push_back(release);
    }

    return release;
  }

  operation*
  version_manifest::parse_operation(release_manifest const& release, JSON const& releaes_node, JSON const& operation_node) const {
    const string_t &operation_type = operation_node["type"].string_value();

    if (operation_type == "create") {
      validate_schema(operation_node, SCHEMA_CREATE_OPERATION);

      const JSON &source_node = operation_node["source"];

      validate_schema(source_node, SCHEMA_CREATE_OPERATION_SOURCE);

      create_operation *op = new create_operation(config_, release);

      op->src_uri = source_node["url"].string_value();
      op->src_checksum = source_node["checksum"].string_value();

      if (source_node["size"].type() == JSON::NUMBER) {
        op->src_size = source_node["size"].int_value();
      }

      op->dst_path = operation_node["destination"].string_value();
      op->is_executable = operation_node["flags"]["executable"].bool_value();

      for (auto sibling_operation_node : releaes_node["operations"].array_items()) {
        if (sibling_operation_node == operation_node) {
          break;
        }
        else if (
          sibling_operation_node["type"].string_value() == "delete" &&
          sibling_operation_node["target"].string_value() == op->dst_path
        )
        {
          op->marked_for_deletion();
        }
      }

      return op;
    }
    else if (operation_type == "update") {
      validate_schema(operation_node, SCHEMA_UPDATE_OPERATION);
      validate_schema(operation_node["basis"], SCHEMA_UPDATE_OPERATION_BASIS);
      validate_schema(operation_node["delta"], SCHEMA_UPDATE_OPERATION_DELTA);

      auto op = new update_operation(config_, release);

      op->basis = operation_node["basis"]["filepath"].string_value();
      op->basis_checksum = operation_node["basis"]["pre_checksum"].string_value();
      op->patched_checksum = operation_node["basis"]["post_checksum"].string_value();
      op->delta = operation_node["delta"]["url"].string_value();
      op->delta_checksum = operation_node["delta"]["checksum"].string_value();

      if (operation_node["delta"]["size"].type() == JSON::NUMBER) {
        op->delta_length = operation_node["delta"]["size"].int_value();
      }

      return op;
    }
    else if (operation_type == "delete") {
      validate_schema(operation_node, SCHEMA_DELETE_OPERATION);

      auto op = new delete_operation(config_, release);

      op->dst_path = operation_node["target"].string_value();

      return op;
    }

    return nullptr;
  }

  const release_manifest*
  version_manifest::get_release(string_t const& id) const {
    for (auto existing_release : releases_) {
      if (existing_release->id == id) {
        return existing_release;
      }
    }

    return nullptr;
  }
}