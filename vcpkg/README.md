contains overlay-ports for vcpkg, in order to enable clang's address sanitizer on Windows(otherwise wired linker errors will occur).
also the utils lib.
```powershell
cd ./vcpkg
vcpkg install scnlib:x64-windows --overlay-triplets=./triplets --no-dry-run
vcpkg install auxilia:x64-windows --overlay-triplets=./triplets --no-dry-run
```