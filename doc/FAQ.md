## What is the cache, where is it, and why is it used?

The cache is a collection of files and directories used to store transient data required for deploying or rolling back a patch; newly created files, `.patch` patch files, and files to be *deleted* are all stored inside the cache. The cache is also used when attempting to repair a corrupt version.

The cache can be found inside the `.kzh/cache` folder in the application's root directory.

Note that the cache is emptied (or *purged*) once Karazeh is done deploying a patch (regardless of the state), so users won't have to worry about wasted space becaue there won't be any.

## Can users mess with the cache? Is it harmful?

Nothing stops them from deleting the cache or tampering with files inside of it as it is a visible and writable folder. Usually it is harmless unless Karazeh gets abnormally terminated and it needs to perform a rollback from a corrupt version.

## What happens if the patcher was interrupted, killed, or somehow terminated while doing its work?

When Karazeh starts its patching routines, it probes the cache, and if the cache turns out not to be empty, that indicates that Karazeh was interrupted abnormally the last time it was run.

Once it is determined that a patch was being applied, Karazeh attempts to identify its *last known version* which is **stored in the cache**^[1] right before any patch is applied (and removed once the patch is successfully deployed, or rolled-back).

Now that the original version is identified, Karazeh will fetch the version manifest, and download the release version for the one that comes right after the original one (which is the patch that had been interrupted). Using that manifest, it resumes the procedure as it normally does: it will first roll-back all the changes, then stage, deploy, and purge.

  > [1] The reason the patcher does not in this case identify the version using the version manifest as it normally does is because the *identity files* might have been changed during the last patch, and then checksum-ming those files would result in an identity that corresponds to no valid release!

## What happens if a patch file could not be downloaded?

If the file's integrity was mismatched (the expected checksum or filesize of the original file didn't match the downloaded one) then the download is retried up to a configurable number of times. If it still fails, or none of the patch servers could be contacted, the patcher performs a roll-back.

## What happens if something goes wrong while patching?

For every operation that's "comitted" in a single Karazeh patch there's a rollback procedure that undoes it. This design allows for a robust and determinate behavior.

For example, if a `delete` operation was committed at some point while patching (and its target file or directory was effectively deleted) while a later operation fails and the patch needs to be rolled back, the deleted target will be properly restored.
