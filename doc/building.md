## Windows

One BIG point to keep in mind while getting the dependencies set up on Windows 
is to watch out for the architecture and compiler version otherwise you're 
getting into a hell of linker issues.

This guide was written for Visual Studio Express 2013 (referred to as the MSVC 
12 compiler) under Windows 7 x86.

### Setting up CMake

Download CMake from https://cmake.org and follow the installation steps. Once
you have that set up, we'll need to get the dependencies built before 
attempting to build Karazeh itself.

### Boost

Any version past 1.46 works. At this time, I used 1.61.

Get the binaries from https://sourceforge.net/projects/boost/files/boost-binaries/1.61.0/ and run the installer.

**MAKE SURE YOU CHOOSE THE RIGHT COMPILER AND ARCHITECTURE!!!** In my case it
was `boost_1_61_0-msvc-12.0-32.exe`.

In the later steps, I'll assume you've used the default installation path - if 
you haven't, make sure you amend where needed.

### libcurl (7.49.1+)

Get the cURL sources, unzip them to something like `C:\local\curl` and then
launch the Visual Studio Command Prompt (we need this for `nmake`).

Follow the guide over at https://github.com/curl/curl/blob/master/winbuild/BUILD.WINDOWS.txt but it basically comes down to compiling with a command like
the following:

    nmake /f Makefile.vc mode=dll MACHINE=x86 VC=12 ENABLE_WINSSL=yes

We do not build against OpenSLL or SSH2, just use the Windows stock SSL 
library. You can optionally install zlib and configure cURL to use it if you
want.

Anyway, should the compilation succeed, the build output will be found under
`C:\local\curl\builds\libcurl-{COMPILER}-{ARCHITECTURE}-{CONFIGURATION}-{OPTIONS...}` mine was something like `libcurl-vc12-x86-release-dll-ipv6-sspi-winssl`.
The files here are what we'll be using to satisfy Karazeh's dependency. We'll 
touch upon this later.

### librsync (2.0.0)

Might be the worst of the lot, mainly because of its stupid dependencies on 
Perl (only as a pre-compile step though.)

1. Get Perl installed from https://www.perl.org/get.html#win32 - I chose the 
ActiveState perl version because it was the smallest download. Nothing 
special here, follow with the installer then CMake will find the interpreter
that we need.
2. Cross your fingers.

Nothing more to do here, this is built in the actual Karazeh solution we'll 
get to later.

### Karazeh

In order to make CMake find the libraries automatically (I'm talking about the
CMake GUI here) we need to tune some environment variables[1].

> **Note**
> 
> Anytime you modify an ENV var, make sure you _restart_ CMake gui otherwise
> it doesn't seem to pick up the new variables.

**CMake with `boost`**

- set `BOOST_ROOT` to wherever the boost installation was, mine was 
  `C:\local\boost_1_61_0`
- set `BOOST_LIBRARYDIR` to the folder in which the libraries for your architecture and compiler version, e.g.: `C:\local\boost_1_61_0\lib32-msvc-12.0`

**CMake with `libcurl`**

To tune for `libcurl`, we adjust `CMAKE_PREFIX_PATH` to include the folder 
where the headers (`curl/curl.h`) may be found as well as the built library (`libcurl.lib`). If we unpacked the sources of libcurl to `C:\local\curl`, we need a value like the following:

    C:\local\curl\builds\libcurl-vc12-x86-release-dll-ipv6-sspi-winssl;
    C:\local\curl\builds\libcurl-vc12-x86-release-dll-ipv6-sspi-winssl\lib

[1] Go to System Properties -> Advanced -> Environment Variables or type 
`rundll32 sysdm.cpl,EditEnvironmentVariables` in the run prompt.

## OS X

I'll assume you have a C++ compiler installed (clang probably). If you don't,
get the XCode developer tools installed.

We now need to install CMake and some version of boost, we can use 
[Homebrew](http://brew.sh/) to do this easily:

```shell
brew install cmake # CMake and "ccmake"
brew install boost # libboost
```

Now you should have the system dependencies installed (libcurl is automatically
available on OS X and Linux so we don't need to do anything special) so we're 
to run CMake to generate our build:

```shell
mkdir build
cd build
ccmake ..
```

Configure and generate the build files. When that is done, run make:

```shell
make
```

## Linux

On Linux it's basically the same as [[os-x]] except you'd be using your 
distro's package manager to get cmake and boost installed. You can also use
`ninja` instead of `make` if you want faster builds.