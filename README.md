# Pando
Pando makes it easy to run experiments with and collect data from several different time tagger and SPAD devices. It supports Brain-Machine Interface research and a recent Nature-published research on **Measuring human cerebral blood flow and brain function with fiber-based speckle contrast optical spectroscopy system** can be found [here](https://www.nature.com/articles/s42003-023-05211-4).

Additional documentation can be found [here](https://github.com/facebookresearch/pando/blob/main/software/doc/sphinx_src/).

# Windows Build Tools Installation
1. Get a windows 10 VM [https://developer.microsoft.com/en-us/windows/downloads/virtual-machines]

2. Install Chocolatey [https://chocolatey.org/install]
3. In an administrator powershell, run:

   ```
   choco install -y python3 --version 3.6.8
   New-Item -ItemType SymbolicLink -Path "C:\Python36\python3.exe" -Target "C:\Python36\python.exe"
   choco install -y git buck python2 visualstudio2017-workload-vctools
   ```
4. Restart

# Runtime Dependencies
Libraries and drivers from various vendors must be installed before certain devices can be used. If the correct runtime dependencies are not installed, the result is usually a hard crash when an attempt to configure one of these devices is made.

All Basler cameras require the Pylon runtime v6.3:
https://www.baslerweb.com/en/downloads/software-downloads/runtime-pylon-6-3-0-windows/

Basler CXP-12 acquisition cards (used with the Basler Boost camera) requre the imaWorx CXP-12 Runtime:
https://www.baslerweb.com/en/downloads/software-downloads/complete-installation-5-9-0-windows-64bit/

The microEnable 5 marathon ACL Frame grabber (used with the Basler acA2000-340kM camera) requires this runtime: 
https://www.baslerweb.com/en/downloads/software-downloads/setup-runtime-5-for-windows-64bit-ver-5-7-0/

The Hamamatsu Orca Fusion requires the "Active Silicon Firebird/Phoenix" component of the DCAM-API:
https://ftp.hostedftp.com/~hamamatsu/DCAM-API/2022/22.9/DCAM-API%20for%20Windows%20(22.9.6509).zip

# Building Pando GUI
Before building Pando, you must generate `software/pando/include/version.h`
```
buck run @mode/win/x86_64/release //software/python/generate_version_header:generate_version_header -- --header --define-name=JULIET_VERSION_STRING >  software/pando/include/version.h
buck build @mode/win/x86_64/release software/python/gui:pando
```

## Mode Files
Buck a has feature called "mode files" (also known as flag files) that allows multiple command line arguments to be supplied at once by passing a path (preceded by an `@`) to a text file containing them. We use this feature to specify various platforms and build types. Our mode files (and the buckconfig files they reference) are stored in `./mode/`.

If you require debug symbols or want to enable asserts, you could instead specify `@mode/win/x86_64/rel_with_deb_info`. If you want an unoptimized debug build, you could specify `@mode/win/x86_64/debug`.

## Using the Correct Toolchain on Windows
Launch "x64 Native Tools Command Prompt for VS 2017" from the start menu. Buck will (apparently) use the environment variables set by this special command prompt to use the right build tools and compile/link against the right windows SDK. Alternatively, run C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvars64.bat from a normal command prompt.

# Contributors
Mark Spatz, Ryan Catoen, Dan Furie, Jen Dority, Charles Freeman, Jami Friedman, Rudy Sevile and Jimmy Feng.

**License**:
LabGraph is MIT licensed, as found in the LICENSE file.
