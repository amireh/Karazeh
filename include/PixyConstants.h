#ifndef H_PixyConstants_H
#define H_PixyConstants_H

#include <map>
#include <string>

namespace Pixy {

  enum PATCH_STAGE {
    P_UNABLE_TO_CONNECT = 100,
    P_VALIDATE_STARTED,
    P_OUT_OF_DATE,
    P_UP_TO_DATE,
    P_PATCH_STARTED,
    P_PATCH_FAILED_NO_PERMISSION,
    P_PATCH_FAILED_BAD_APP_DATA,
    P_PATCH_FAILED_BAD_PATCH_DATA,
    P_PATCH_FAILED_BAD_VERSION,
    P_PATCH_COMPLETE
  };

  std::map<int, std::string> P_MESSAGES;

  P_MESSAGES[P_UNABLE_TO_CONNECT] =
    "Unable to connect to patch server, please verify your internet connectivity.";
  P_MESSAGES[P_VALIDATE_STARTED] =
    "Validating application version, please wait...";
  P_MESSAGES[P_OUT_OF_DATE] =
    "Updates are available. Would you like to install them now?";
  P_MESSAGES[P_UP_TO_DATE] =
    "Application is up to date ${VERSION}";
  P_MESSAGES[P_PATCH_STARTED] =
    "Updating to ${VERSION}";
  P_MESSAGES[P_PATCH_FAILED_NO_PERMISSION] =
    std::string("There are not enough permissions to modify your application data, please re-run this program") +
    std::string(" using an account with enough privilege.");
  P_MESSAGES[P_PATCH_FILE_BAD_APP_DATA] =
    "Unable to apply patch, application data seems to be modified. Please re-install.";
  P_MESSAGES[P_PATCH_FILE_BAD_PATCH_DATA] =
    "Unable to apply patch due to a failure in retrieving it, please re-run this program.";
  P_MESSAGES[P_PATCH_FILE_BAD_VERSION] =
    "Unrecognized application version. If you're unable to restore the file states, please re-install.";
  P_MESSAGES[PATCH_COMPLETE] =
    "Application was successfully updated to ${VERSION}";
}

#endif
