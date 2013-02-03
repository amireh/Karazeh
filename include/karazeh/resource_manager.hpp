/*
 *  Copyright (c) 2011-2012 Ahmad Amireh <kandie@mxvt.net>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#ifndef H_KARAZEH_RESOURCE_MANAGER_H
#define H_KARAZEH_RESOURCE_MANAGER_H

#include "karazeh/karazeh.hpp"
#include "karazeh/logger.hpp"
#include "karazeh/hasher.hpp"
#include "karazeh/settings.hpp"
#include <curl/curl.h>
#include <boost/filesystem.hpp>
#include "binreloc/binreloc.h"

namespace kzh {

  namespace fs = boost::filesystem;
  typedef boost::filesystem::path path_t;

  struct download_t;
  class resource_manager : protected logger {
  public:
    
    /** @param host a fully-qualified URI of the patch server */
    resource_manager(string_t const& host = "http://127.0.0.1");
    virtual ~resource_manager();

    string_t const& host_address() const;
    void rebase(const string_t& new_host);
    
    /** The number of times to retry a download */
    int retry_count() const;
    void set_retry_count(int);
    
    /** 
     * The root path can be overridden at runtime via the -r option,
     * otherwise it is assumed to be KZH_DISTANCE_FROM_ROOT steps
     * above the directory that contains the running process.
     *
     * For example, if the running process is at bin/something
     * and KZH_DISTANCE_FROM_ROOT is set to 1, then the root will
     * be correctly set to `bin/..`
     *
     * All changes are deployted relative to the root path.
     */
    static path_t const& root_path();

    /**
     * The "staging" directory used internally by Karazeh which
     * resides in `$ROOT/.kzh`.
     */
    static path_t const& cache_path();

    /** 
     * The directory of the running process.
     *
     * On Linux, it is located using binreloc (see deps/binreloc/binreloc.h)
     * On OS X, it is located using NSBundlePath() (see karazeh/utility.hpp)
     * On Windows, it is located using GetModuleFileName() (see resolve_paths())
     *
     * All the other paths are derived from the bin_path unless overridden.
     */
    static path_t const& bin_path();

    void resolve_paths(path_t root = "");

    /** Loads the content of a file stream into memory */
    bool load_file(std::ifstream &fs, string_t& out_buf);

    /** Loads the content of a file found at @path into memory */
    bool load_file(string_t const& path, string_t& out_buf);
    bool load_file(path_t const& path, string_t& out_buf);

    /** Checks if the resource at the given path exists, is a file, and is readable. */
    bool is_readable(path_t const &path) const;
    bool is_readable(string_t const &path) const;

    /** Checks if the resource at the given path exists, is a file, and is writable. */
    bool is_writable(path_t const &path) const;
    bool is_writable(string_t const &path) const;

    /**
     * Creates a directory indicated by the given path,
     * while creating all necessary ancestor directories (similar to mkdir -p)
     *
     * Returns false if the directories couldn't be created, and the cause
     * will be logged.
     */
    bool create_directory(path_t const& path);

    /**
     * Downloads the file found at URI and stores it in out_buf. If
     * @URI does not start with http:// then it will be prefixed by
     * the assigned server URI.
     *
     * @return true if the file was correctly DLed, false otherwise
     */
    bool get_remote(string_t const& URI, string_t& out_buf);

    /** same as above but outputs to file instead of buffer */
    bool get_remote(string_t const& URI, std::ostream& out_file);

    /**
     * Downloads the file found at the given URI and verifies
     * its integrity against the given checksum. The download
     * will be retried up to retry_count() times.
     *
     * Returns true if the file was downloaded and its integrity verified.
     */
    bool 
    get_remote(string_t const& URI, 
               path_t const& path_to_file, 
               string_t const& checksum,
               uint64_t expected_size_bytes = 0,
               int* const nr_retries = NULL);

    /** 
     * If @URI_or_path begins with http[s]:// then the resource
     * is expected to be remotely downloaded, otherwise it is assumed to be
     * a local one.
     */
    bool get_resource(string_t const& URI_or_path, string_t& out);

    /** 
     * Enables the executable permission flag for systems that support it.
     *
     * The file will have a permission mask equal to 0711
     *
     * Returns false if the permissions couldn't be modified.
     */
    bool make_executable(path_t const&);

    uint64_t stat_filesize(path_t const&);
    uint64_t stat_filesize(std::ifstream&);

  private:
    static path_t root_path_, bin_path_, cache_path_;
    string_t host_;

    bool get_remote(string_t const& URI, download_t*, bool assume_ownership = true);

    int nr_retries_;
  };

  /** Used internally by the resource_manager to manage downloads */
  struct download_t {
    inline 
    download_t(std::ostream& s)
    : to_file(false), 
      status(false), 
      size(0),
      retry_no(0),
      stream(s) {
    }

    string_t      *buf;
    string_t      uri;
    bool          status;
    bool          to_file;
    std::ostream  &stream;
    uint64_t      size;
    int           retry_no;
  };

} // end of namespace kzh

#endif
