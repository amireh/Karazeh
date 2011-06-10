Karazeh is a cross-platform tool for patching and launching desktop applications, suited for PC games. The purpose of this tool is to provide you with the ability to update your users' clients with minimal hassle without requiring you to re-invent the wheel and write your own patcher.

Karazeh is meant to be used with [Kiwi](https://github.com/amireh/Kiwi), the tool that generates the patch scripts which Karazeh consumes. They both should operate on Windows, Linux, and Mac OS X.

Both tools are, and will always be, freely available and licensed under the MIT license.

## How it works
[BZip2](http://www.bzip.org/) is used for (de)compression of patch data, [CURL](http://curl.haxx.se/) is used for fetching patch data, and [bsdiff/bspatch](http://www.daemonology.net/bsdiff/) are used for generating file patches and applying them.

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

## License
Copyright (c) 2011 Ahmad Amireh

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.