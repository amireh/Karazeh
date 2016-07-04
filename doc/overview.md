# Overview

The main reason I decided to rebuild Karazeh from the grounds up is to redesign the main patching process in order to obtain a reasonable amount of **robustness**. I don't want it to be responsible for *corrupting* an application repository, and so I've decided to break down the process of applying a patch into four phases: two of them are sequential, and two are conditional.

Each operation has its own routine for each phase.

<iframe
  id="xmindshare_embedviewer"
  src="http://www.xmind.net/embed/FFcq?size=large"
  width="100%"
  height="540px"
  frameborder="0"
  scrolling="no"
></iframe>

The map lists every action and test done by each operation during each phase of applying a patch. The stages are explained in the following sections.

## Phase 1 - Staging

The first stage of applying a patch is aptly named *staging* and it is a stage of aggressive checks and verifications that tries to ensure that nothing will go wrong when Karazeh will start modifying things.

Operations that require downloading remote resources, such as `create` and `update`, do so during the staging process. The downloaded files are kept in the cache. They will also validate the integrity of the downloaded files.

The diagram above explains in detail what each operation does in each stage; the checks and actions it takes.

**During staging and deployment, any operation that fails for any reason will cause the patcher to invoke a rollback, effectively restoring the application to its earlier state.**

## Phase 2 - Deployment

Alright, so now we have our patch files ready, and we've done the required testing. Well, something might have changed since the operations were staged, so **some** of the tests are re-run before deployment.

Deployment is where application files are actually modified. `create` operation now actually creates the new resource at its specified destination, the `update` operation performs the patching on the target file, `delete` removes the file (almost true), etc.

As noted in the Staging section, any failure here will also cause a rollback. But the question is, how can we rollback if we've modified files or removed them? Even in the deployment stage (and the staging one), no operation modifies an application file in any way without either:

1. making a backup
2. making sure there's a way to revert the file to its original state

So in the case of `delete`, all it truly does in the deployment stage is *move* the file from its original location into the *cache*. So if a rollback is invoked, it's simply a matter of moving the file back from the cache to its original location.

If everything went okay with staging and deployment, the committing stage is entered.

## Rolling Back

The only thing worth noting here is that any failure reached while rolling back the changes will most definitely cause a **version corruption**. So far, I haven't thought out a solution to *repair* corrupt versions so I have little to say here.

Rollback routines are meant to undo any changes made while staging and deploying.

## Committing

If everything went OK and no rollback was invoked, all operations are called to "commit" their changes; a commit means that the patch was applied successfully, and any *transient* data maintained for rolling-back can be safely discarded.

In the case of `delete`, this means it will remove the file from the cache. `update` will delete the patch file and the backup, etc. The cache should be effectively purged after this stage, and there will be nothing more to do!
