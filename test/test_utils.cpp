#include "test_utils.hpp"
#include <boost/filesystem.hpp>
#include <iostream>

namespace kzh {
  namespace test_utils {
    namespace fs = boost::filesystem;

    bool copy_directory(path_t const& source, path_t const& destination) {
      namespace fs = boost::filesystem;

      try
      {
        // Check whether the function call is valid
        if( !fs::exists(source) ||
          !fs::is_directory(source) )
        {
          std::cerr << "Source directory "
                    << source.string()
                    << " does not exist or is not a directory."
                    << '\n';
          return false;
        }

        if( fs::exists( destination ) )
        {
          std::cerr << "Destination directory "
                    << destination.string()
                    << " already exists." << '\n';
          return false;
        }

        // Create the destination directory
        if( !fs::create_directory( destination ) )
        {
          std::cerr << "Unable to create destination directory"
                    << destination.string() << '\n';
          return false;
        }
      }

      catch( fs::filesystem_error const & e)
      {
        std::cerr << e.what() << '\n';
        return false;
      }

      // Iterate through the source directory
      for( fs::directory_iterator file( source );
           file != fs::directory_iterator();
           ++file )
      {
        try
        {
          path_t current( file->path() );

          if( fs::is_directory( current ) ) {
            // Found directory: Recursion
            if ( !::kzh::test_utils::copy_directory( current, destination / current.filename() ) ) {
              return false;
            }
          }
          else
          {
            // Found file: Copy
            fs::copy( current, destination / current.filename() );
          }
        }

        catch( fs::filesystem_error const & e )
        {
          std:: cerr << e.what() << '\n';
        }
      }
      return true;
    }

    void create_file(path_t const& path, string_t const& contents) {
      if (!fs::is_directory(path.parent_path())) {
        fs::create_directories(path.parent_path());
      }

      std::ofstream fh(path_t(path).make_preferred().string(), std::ios::trunc | std::ios::in);
      fh << contents;
      fh.close();
    }

    void remove_file(path_t const& path) {
      if (fs::exists(path)) {
        fs::remove(path);
      }
    }
  } // namespace test_utils
} // namespace kzh
