### Tests

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