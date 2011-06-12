Karazeh is a cross-platform tool for patching and launching desktop applications, suited for PC games. The purpose of this tool is to provide you with the ability to update your users' clients with minimal hassle without requiring you to re-invent the wheel and write your own patcher.

Karazeh is meant to be used with [Kiwi](https://github.com/amireh/Kiwi), the tool that generates the patch scripts which Karazeh consumes. They both should operate on Windows, Linux, and Mac OS X.

Both tools are, and will always be, freely available and licensed under the MIT license.

## How it works
[BZip2](http://www.bzip.org/) is used for (de)compression of patch data, [CURL](http://curl.haxx.se/) is used for fetching patch data, and [bsdiff/bspatch](http://www.daemonology.net/bsdiff/) are used for generating file patches and applying them.

## Current Features
* Uses boost::filesystem for handling file paths and file management across different platforms
* Pluggable renderers: Karazeh does not tie you to any front-end/rendering library: I've shipped 3 "Renderers", one is Ogre3D-based, another is Qt-based, and a vanilla CLI one. Creating a Cocoa/GTK/wxWidgets/any other toolkit based renderer is very straightforward and possible
* Pluggable thread providors: use boost::thread, Qt threads, Poco threads, Intel TBB, or any other thread providor you're using for threading support while patching so the launcher can stay responsive and inject feedback for the user
* Threadless mode: can't use threads? Don't!
* Karazeh can patch itself! This was tested on Linux and Windows XP, there might be UAC problems on 7/Vista
* Is runtime path agnostic; you can run it from anywhere and it will work granted that the user doesn't manually move the binary to a different directory
* Uses CURL for fetching patch data, and supports for retries in downloading; if a file could not be fetched, or didn't pass the integrity check, you can specify how many times it can be redownloaded
* Integrity: all downloaded files are MD5-summed and verified for integrity
* Efficiency: in-memory binary patching using bsdiff so users won't have to waste bandwidth. Send the partial changes instead of huge files
* Diff files are (de)compressed using BZip2
* Supports 4 operation types (CRUD): create new files, modify, rename/move, and delete existing files
* Patching is staged: transactions are not committed until all operations were verified. Karazeh should never corrupt your tree
* Support for hard-coded patch server URLs and an external, runtime-parsed patch mirrors list
* Graceful roll-back if there was any problem carrying out any operation, if there's no internet connectivity, or if all of the registered hosts could not be resolved

## Documentation
Please refer to the [wiki](https://github.com/amireh/Karazeh/wiki) for building, configuring, and running Karazeh.

## Dependencies
* Core:
  * boost_filesystem 1.46
  * libcurl
  * log4cpp
  * libbzip2
* Optional dependencies:
  * Qt 4.7
  * Ogre 1.7 & OIS 1.2
  * boost::thread 1.46 & boost::date_time 1.46

## Testing
Karazeh and Kiwi have so far been tested on the following platforms/distributions:

* Linux:
  * Arch Linux 32bit
  * Ubuntu 10.04 LTS 32bit
* Windows:
  * XP SP2

If you try any of the tools out and can be bothered with notifying me about any distribution-specific issues, please do.

## Upcoming Features
* Integration *within* your current application: if you don't want your users to run the Launcher to use your game/application, simply embed Karazeh within it to provide patching support
* Utilize platform-specific user local data directories for staging patches, such as User\AppData on Windows, ~/ on Linux and ~/Library on Mac OS X.
* Self-contained, portable patches: if a user doesn't have internet connectivity, they can grab the patch data from a friend and patch their client using it
* Patch data compression so users will use less bandwidth for fetching a patch
* Persistency: if the user stopped the Launcher while patching or fetching data, they will not have to re-fetch the data

## Similar Projects

If Karazeh doesn't suit your needs, check out [Technogothica's launcher](http://technogothica.com/wiki/index.php?title=Launcher/Updater_Project), it's a promising project and also MIT-licensed.

## License
Copyright (c) 2011 Ahmad Amireh

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
