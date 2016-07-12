# Karazeh

> **NOTE**
> 
> You are currently reading the documentation for the _master_ branch which
> is NOT stable and is not yet ready for production use. Refer to the 
> [stable](https://github.com/amireh/Karazeh/tree/stable) branch if that's
> what you're looking for.

Karazeh is a cross-platform tool for patching and launching desktop applications, suited for PC games. The purpose of this tool is to provide you with the ability to update your users' clients with minimal hassle without requiring you to re-invent the wheel and write your own patcher.

Karazeh is meant to be used with [Kiwi](https://github.com/amireh/Kiwi), the tool that generates the patch scripts which Karazeh consumes. They both should operate on Windows, Linux, and Mac OS X.

Both tools are freely available and licensed under the MIT license.

## Building

See the [[Building]] guide.

## Usage

_TBD_

## Tests

To run the unit tests you must launch an http server on port 9393 to serve the 
files under `/path/to/karazeh/test/fixture` for the tests. 

If you have Python2 available, it's very easy:

    (cd test/fixture; python2 -m SimpleHTTPServer 9393)

If you're on Arch Linux, you can use `darkhttpd` instead:

```bash
pacman -S darkhttpd
darkhttpd /path/to/karazeh/test/fixture --port 9393
```

Afterwards, run the binary:

```bash
./karazeh_tests
```

The test runner can be customized using some environment variables:

- `VERBOSE=1` if you want more output
- `ROOT=/path/to/root` should point to the root directory of the Karazeh repository; this is needed in case the built-in binary locators aren't doing their job correctly.
- `HOST=http://customhost:1234` should point to the fixture server. Defaults to `http://localhost:9393`

## License

Copyright 2011-2016 Ahmad Amireh <ahmad@amireh.net>.

The code is licensed under the GNU L-GPL v3.0 (see COPYING) and is free to use,
link-against and embed within free and commercial applications.

Libraries used by Karazeh to link against and their respective licenses:

- [TinyXML-2](https://github.com/leethomason/tinyxml2) - z-lib
- [librsync](http://librsync.sourcefrog.net/) - GNU LGPL v2.1
- [binreloc](https://github.com/datenwolf/binreloc) - DWTFYWT
- RSA Data Security, Inc. MD5 Message-Digest
Algorithm - ???
