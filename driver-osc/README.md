# Non OSC driver for Traktor Kontrol F1

This software aims to provide a [Non](https://non.tuxfamily.org) compatible userspace driver for the Traktor Kontrol F1.
Interacting with the Kontrol F1 is done via Linux's hidraw API.
Interacting with Non is done via Non Session Management and the (as I call it) Non hello protocol as described in [nsm_client_hello](doc/nsm_client_hello.md).

## Requirements

  * Build
    * Linux
    * [liblo](http://liblo.sourceforge.net)
	* [JACK](https://jackaudio.org)
    * [CMake](https://cmake.org)
    * [Ninja](https://ninja-build.org) (make does the job too, but I like Ninja)
    * A C++ compiler (tested with GCC)
    * GTest (`WITH_TESTS=true` only)
  * Runtime
    * The driver needs to run as root (but drops privileges after startup).
    * The driver needs to run inside a Non session (i.e. the `NSM_URL`) environment variable needs to be set.

## Building

```
$ git clone https://github.com/nexadn/ni-traktor-kontrol-f1.git
$ mkdir -p traktor-kontrol-f1/build
$ cd traktor-kontrol-f1/build
$ cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DWITH_TESTS=false
$ ninja
```

## Unit testing

```
$ cd traktor-kontrol-f1/build
$ cmake .. -DWITH_TESTS=true
$ ninja check
```

## Installing

Currently, there is no automatic installation method provided, but one can install the driver e.g. by issuing:

```
$ sudo install -o root -g root -Dm4755 src/exe/osc_driver /usr/local/bin/traktor-kontrol-f1-driver
```

**Setting setuid happens at your own risk. This enables any program on your computer to run the driver as root.**
