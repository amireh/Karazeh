# Architecture

<iframe
  src='http://www.xmind.net/embed/jXME'
  width='100%'
  height='540px'
  frameborder='0'
  scrolling='no'
></iframe>

## Core

### Transporters

1. Need to provide a post-download hook for a compressor to decompress if necessary.

### Path Resolver

1. Must allow for user-defined special paths

## Images

1. are uniquely identified by the sum of checksums of identity files specified in an identity list

## Manifests

### The Flavor Manifest (FMF)

Every application has one and only one FMF. Each FMF entry describes a completely separate "flavor" of the application which will be represented by its own set of manifests.

This is crucial for applications of all types, and games in particular. If a game supports DLCs or Expansion sets, then each flavor would most probably need its own branch of patching.

An application is not restricted to qualify as only one flavor. Following the game example, a flavor of the game with the DLC X may very well still use the patches for the original game. And if both DLC X and Y are installed, then all of the original, DLC X, and DLC Y flavor branches should be followed.

An FMF entry:

  1. should be tagged
  1. should define a condition that identifies the flavor

FMF template:

```xml
<karazeh>
  <flavors>
    <flavor name="Vanilla">
      <conditions>
        <condition>
          <name>file-exists</name>
          <args><![CDATA[path/to/file|folder]]></args>
        </condition>
        <condition>
          <name>custom</name>
          <args><![CDATA[identify_my_flavor]]></args>
        </condition>        
      </conditions>
    </flavor>
  </flavors>
</karazeh>
```

Finally, the use of FMF is optional. The default should contain a single entry tagged as 'Vanilla' or 'Original' with a condition that always evaluates to true.

### Identity Manifests (IMF)

There needs to be a method to uniquely identify the application at any version. Identity lists are intended to do just that; specify a list of files, the accumulative checksum of which will be the application's *identity*.

Using that identity value, the application at each version will represented by a unique *Image*.

The IMF contains all the identity lists (ILs) the application requires. It is fetched and parsed right after the FMF. An identity registry will be built using the identity lists described in the IMF, and the application is expected to be identified by exactly one IL for each flavor the application was identified to represent.

The actual identification is done in the VMF parsing stage.

### The Version Manifest (VMF)

As the name implies, the purpose of the VMF is to identify the version of the application (aka, locating its Image), and describe the history of the application.

There should be only one VMF per application flavor. The VMF contains a listing of entries that map to the final manifest entity, Release Manifests (RMFs).

VMF entries should:

  1. specify the image id the RMF is for
  2. specify the RMF URI

If the application could not be identified at this stage, no further processing will be allowed and the image should be reported to be corrupt.
