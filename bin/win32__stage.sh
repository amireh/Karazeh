# A script for staging pre-built .dll binaries to the
# cmake binary directory so that you don't have to manually
# copy them over to run the project binaries (like the test
# runner and rdiff.exe etc.)
#
# Usage: run this using MINGW shell (Git Bash) after having
# compiled everything.
#   
#   cd /path/to/karazeh
#   ./scripts/win32__stage.sh

$(grep "# Karazeh" README.md &> /dev/null) || {
  echo "Must be run from Karazeh root!"
  exit 1
}

DEPS_PATH="/c/local"
STAGE_PATH="build/Debug/"

# libcurl
cp "${DEPS_PATH}"/curl-7.49.1/builds/libcurl-vc12-x86-release-dll-ipv6-sspi-winssl/bin/libcurl.dll "${STAGE_PATH}"

# googletest, if you're building tests
cp "${DEPS_PATH}"/googletest/build/googlemock/gtest/Debug/gtest.dll "${STAGE_PATH}"

# boost
cp "${DEPS_PATH}"/boost_1_61_0/lib32-msvc-12.0/boost_system-vc120-mt-gd-1_61.dll "${STAGE_PATH}"
cp "${DEPS_PATH}"/boost_1_61_0/lib32-msvc-12.0/boost_filesystem-vc120-mt-gd-1_61.dll "${STAGE_PATH}"

# popt (for rdiff.exe)
cp "${DEPS_PATH}"/GnuWin32/bin/* "${STAGE_PATH}"

# karazeh
cp build/Debug/kzh.dll "${STAGE_PATH}"
