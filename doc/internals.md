## Current dependencies

* [boost 1.46+](http://boost.org) for the `filesystem` library
* [cURL](http://curl.haxx.se)
* [CMake 2.8+](http://cmake.org) for building

**Embedded Dependencies**

* [librsync](http://librsync.sourceforge.net)
* [json11](https://github.com/dropbox/json11)
* [binreloc](http://pawnscript.googlecode.com/svn-history/r6/trunk/linux/binreloc.c) *Linux only*
* md5

## Identifying the application version

Karazeh tells which version of the application the user is currently running by way of calculating checksums of a number of files referred to as the **identity files**. Those checksums are joined together and the resulting "blob" is finally digested into another checksum known as the **identity checksum**. The identity checksum is, to Karazeh, the version of the running application.

Identity files could be a singular binary file (the application's executable, for example), or a list of files; maybe the executable joined with the core data archive, or a crtiical script component. The usage of identity files guarantees that should any of them be tampered with, the identity checksum will no longer match the expected one resulting in an unidentified version, or a *corrupt* one.

Finally, each release can define its own list of identity files. For example, a game might use its binary as the identity file until its expansion comes out where another binary is introduced, so the expansion releases can define both binaries to be the identity files.

## Versioning schemes

Thanks to using identity files for identifying the version of the application, Karazeh does not force you to any particular versioning scheme. In fact, a "version", or *tag* as it is referred to internally, is really nothing but a string to it that has no meaning. The tag can be used by you, the application developer, to label your versions in a way **you** see fit. The tag is also what's commonly displayed to the user, since it's much more friendly than a hex digest (the identity checksum).

Examples of some popular versioning schemes: `1.0.3-rc1`, `10.6`, `b01cd.0`, `10.7 Lion`, etc.

Notice how you can also use "codenames" as well as numbered schemes; define the tag you way you want, and parse it the way you want, Karazeh will not interfere.

## Generating binary diffs

Binary files and data archives are usually very large, and it's inefficient to force the user to re-download them everytime they're updated. Karazeh can help you transmit only the parts that change - to an extent - in those binary files via a [delta encoding](http://en.wikipedia.org/wiki/Delta_encoding) solution, [`librsync`](librsync.sourceforge.net).

### The hunt for a delta encoding solution

The difficulty lied in the number of requirements the library had to meet for it to be usable in Karazeh:

1. memory efficiency in both encoding and decoding routines
2. licensing compatibility (not proprietary, nor GPL)
3. support for processing large enough basis files that can acommodate today's binary file requirements (some games have data archives as large as 20GBytes)
4. delta filesize efficiency
5. cross-platform operabiliity

So in my hunt for a solution, I came accross the following:

1. [`bsdiff`](http://www.daemonology.net/bsdiff/) - unforgivably fast and efficient, but just as much memory-hungry; it didn't satisfy requirement#1
2. [`xdelta3`](http://xdelta.org) - the most memory efficient solution I've tried, and the fastest, with a bit higher patch file sizes but that's a trivial price to its speed - but it's GPL licensed
3. [`open-vcdiff`](http://code.google.com/p/open-vcdiff/) - maybe I didn't know how to use it, but it ate all my memory while patching a 1GByte archive just like `bsdiff` did

Finally, I met `rdiff`/`librsync` and it won on all grounds; license compatibility, memory requirements, speed, and patch filesize.

## Operations

The following breakdown satisfies the operation requirements: Creation, Renaming, Updating, and Deletion of files (or directories, when applicable.)

### `create`

Arguments:

1. the source file: this is the file that will be fetched from the server and "copied" to a final destination
2. the destination: the fully qualified path the file should be placed at when the patch is committed

**Staging**

1. verify that no file exists at the destination
2. verify that the running user has write permissions
3. verify that there's enough space to hold the file
4. fetch the file and store it in the staging reposistory
5. validate the file's integrity, and redownload if necessary

**Deployment**

1. move the file from the staged source to the destination

**Synopsis**

```json
{
  "type": "create",
  "source": {
    "url": String,
    "checksum": String,
    "size": Number
  },
  "destination": String,
  "flags": {
    "executable": Boolean
  }
}
```

### `update`

Arguments:

1. the source file; the file to be patched
2. the source file's checksum
3. the patch file
4. the patch file's checksum
5. the patch file's size

**Staging**

1. verify that the source exists
2. validate the integrity of the source
3. verify that there's enough space to hold the patch file
   **and** the backup of the source file
4. fetch the patch file
5. validate the integrity of the patch file
6. create a backup of the source file

**Deployment**

1. apply the patch on the clone (aka backup)
2. validate the integrity of the patched file:
  1. if the integrity test fails, announce a rollback
3. remove the source file
4. move the patched file to the source's destination

**Synopsis**

```json
{
  "type": "update",
  "basis": {
    "pre_checksum": String,
    "post_checksum": String,
    "filepath": String
  },

  "delta": {
    "checksum": String,
    "size": Number,
    "url": String
  }
}
```

### `delete`

Arguments:

1. the fully qualified source path

**Staging**

1. verify that the source exists

**Deployment**

1. if the source is a directory, recursively empty its contents
2. remove the source

**Synopsis**

```json
{
  "type": "delete",
  "checksum": String,
  "target": String
}
```

### Notes

* `create` with the `executable` flag will cause the patcher to mark the created file as executable (chmod 0711)
* `delete` entries will internally mark those paths as "to be deleted", so any subsequent  `create` entry with one of those paths will know that they will be deleted, and will not cause a staging error; effectively, we achieve the effect of `replace` without having to implement any!
* running with `-v` will cause the `resource_manager` to print out the content of all downloaded files
* `delete` recursively removes directories as well as files

## The Version Manifest

```json
{
  "identities": [
    {
      "name": String,
      "files": Array.<String>
    }
  ],

  "releases": [
    {
      // The identity list used for computing the id of this release. See
      // the "id" attribute below for more information.
      "identity": String,

      // The identifier of this release. This value is computed based on the 
      // accumulation of checksums of the identity files (as specified per the
      // identity list @identity) which is then digested using MD5.
      // 
      // For example, say the identity "Base" has the files:
      // 
      //     - bin/app.exe # => c23cca28502254df15f39dc8300ef752
      //     - data/assets.mpq # => f1eb970aeb2e380593480ed76070acbe
      // 
      // The identifier for such a version of the application would be:
      // 
      //     MD5(
      //       "c23cca28502254df15f39dc8300ef752" +
      //       "f1eb970aeb2e380593480ed76070acbe"
      //     ) # => 3a2f552b7db42835a84ec19ee3edf4c9
      "id": String,

      "operations": Array,

      // Identifier of the "head" release; the preceding release to this one.
      // 
      // When no "head" is specified, the release is assumed to be the initial
      // one (i.e. the starting state of the application).
      // 
      // Having more than one release per identity list with no "head" results
      // in undefined behavior.
      "head": String?,

      // A tag for use by the integrating application to refer to the release.
      // This is not employed by Karazeh in any way and is only meant for user
      // display purposes.
      "tag": String?,

      // A URI to download the release manifest from. Use of this attribute
      // is optional IFF the "operations" attribute is specified inline in
      // the version manifest.
      // 
      // This allows the integration layer to choose whether to split the
      // manifests up on a per-release basis, or to specify them all in the
      // version manifest.
      "uri": String?
    }
  ]
}
```

## The Release Manifest

```xml
[!include!](https://raw.github.com/amireh/karazeh_v2/master/doc/release_manifest.template.xml)
```