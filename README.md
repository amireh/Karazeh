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

_TBD_

## Usage

_TBD_

## Tests

To run the unit tests you must launch an http server to serve the fixtures. You can
easily do this using a light http daemon such as `darkhttpd` and running it
on port 8080 to serve `/path/to/karazeh/test/fixture` as the root directory.

Example on Arch Linux:

```bash
pacman -S darkhttpd
cd /path/to/karazeh/test/fixture
darkhttpd .
```

Afterwards, run the binary:

```bash
./karazeh_unit_tests
```

## License

Copyright 2011-2016 Ahmad Amireh <ahmad@amireh.net>.

The code is licensed under the GNU L-GPL v3.0 (see COPYING) and is free to use,
link-against and embed within free and commercial applications.