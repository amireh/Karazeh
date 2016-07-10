#include "karazeh/operations/update.hpp"

namespace kzh {
  namespace fs = boost::filesystem;

  update_operation::update_operation(
    config_t const& config,
    file_manager const& file_manager,
    downloader& downloader,
    release_manifest& rm
  )
  : operation(config, file_manager, downloader, rm),
    logger("op_update"),
    patched_(false)
  {
  }

  update_operation::~update_operation() {
  }

  string_t update_operation::tostring() {
    std::ostringstream s;
    s << "update basis[" << basis << ']'
      << " using[" << delta << ']';
    return s.str();
  }

  STAGE_RC update_operation::stage() {
    basis_path_     = (config_.root_path / basis).make_preferred();
    cache_dir_      = path_t(config_.cache_path / rm_.checksum / basis).make_preferred().parent_path();
    signature_path_ = (cache_dir_ / basis).make_preferred().filename().string() + ".signature";
    delta_path_     = (cache_dir_ / basis).make_preferred().filename().string() + ".delta";
    patched_path_   = (cache_dir_ / basis).make_preferred().filename().string() + ".patched";

    // basis must exist
    if (!file_manager_.is_readable(basis_path_)) {
      error()
        << "basis file does not exist at: " << basis_path_;

      return STAGE_FILE_MISSING;
    }

    // basis checksum check
    hasher::digest_rc digest = hasher::instance()->hex_digest(basis_path_);
    if (digest != basis_checksum) {
      error()
        << "Basis file checksum mismatch: "
        << digest.digest << " vs " << basis_checksum
        << " in file " << basis_path_;

      return STAGE_FILE_INTEGRITY_MISMATCH;
    }

    if (file_manager_.stat_filesize(basis_path_) != basis_length) {
      error()
        << "Length mismatch: "
        << file_manager_.stat_filesize(basis_path_) << " to " << basis_length
        << " in file " << basis_path_;

      return STAGE_FILE_INTEGRITY_MISMATCH;
    }


    // prepare our cache directory, if necessary
    if (!fs::is_directory(cache_dir_)) {
      if (!file_manager_.create_directory(cache_dir_)) {
        error() << "Unable to create cache directory: " << cache_dir_;
        return STAGE_UNAUTHORIZED;
      }
    }

    // TODO: free space checks, need at least 2x basis file size + delta size

    // get the delta patch
    if (!downloader_.fetch(delta, delta_path_, delta_checksum, delta_length)) {
      throw invalid_resource(delta);
    }

    // create the signature
    debug() << "generating signature for " << basis_path_ << " out to " << signature_path_;

    rs_result rc = encoder_.signature(basis_path_.c_str(), signature_path_.c_str());
    if (rc != RS_DONE) {
      error() << "Generating of signature for file " << basis_path_ << " has failed. librsync rc: " << rc;
      return STAGE_INTERNAL_ERROR;
    }

    return STAGE_OK;
  }

  STAGE_RC update_operation::deploy() {
    debug() << "patching file " << basis_path_ << " using delta " << delta_path_ << " out to " << patched_path_;
    rs_result rc = encoder_.patch(basis_path_.c_str(), delta_path_.c_str(), patched_path_.c_str());

    if (rc != RS_DONE) {
      error()
        << "Patching file " << basis_path_ << " using patch " << delta_path_
        <<" has failed. librsync rc: " << rc;

      return STAGE_INTERNAL_ERROR;
    }

    hasher::digest_rc digest = hasher::instance()->hex_digest(patched_path_);
    if (digest != patched_checksum) {
      error()
        << "Checksum mismatch: "
        << digest.digest << " vs " << patched_checksum
        << " in file " << patched_path_;

      return STAGE_FILE_INTEGRITY_MISMATCH;
    }

    if (file_manager_.stat_filesize(patched_path_) != patched_length) {
      error()
        << "Length mismatch: "
        << file_manager_.stat_filesize(patched_path_) << " to " << patched_length
        << " in file " << patched_path_;

      return STAGE_FILE_INTEGRITY_MISMATCH;
    }

    // swap the files
    debug() << "Swapping files: " << basis_path_ << " & " << patched_path_;

    fs::rename(patched_path_, path_t(patched_path_.string() + ".tmp"));
    fs::rename(basis_path_, patched_path_);
    fs::rename(path_t(patched_path_.string() + ".tmp"), basis_path_);

    patched_ = true;

    return STAGE_OK;
  }

  void update_operation::rollback() {
    // make sure the basis still exists in cache
    if (patched_) {

      if (!fs::exists(patched_path_)) {
        throw invalid_state("Basis no longer exists in cache, can not rollback!");
      }

      // the file should be there.. but just in case
      if (fs::exists(basis_path_)) {
        fs::remove(basis_path_);
      }

      fs::rename(patched_path_, basis_path_);
    }

    cleanup();
  }

  void update_operation::commit() {
    cleanup();
  }

  void update_operation::cleanup() {

    // delete the delta patch
    if (fs::exists(delta_path_)) {
      fs::remove(delta_path_);
    }

    if (fs::exists(signature_path_)) {
      fs::remove(signature_path_);
    }

  }

}