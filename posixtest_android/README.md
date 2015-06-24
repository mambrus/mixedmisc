---
format: Markdown
...

# Welcome to ``posixtest``

Test pthread\_cancel() and friends with CMake based build for Android.

# Build

Build-system is based on CMake, with all it's blessings and flaws. 

## Native

* For a local build (recommended method):
```bash
mkdir BUILD_local
cd BUILD_local
cmake ../
ccmake .         #Adjust settings. Optional
make -j$(cat /proc/cpuinfo | grep processor | wc -l)
```
*The name of the build-directory is not important, just make sure it differs
from other build-directories if you have more than one builds, which for
this project you'd typically have.*


## Cross-build
For cross-compiling you need so called *tool-chain* files. In this project
there is one such available ``Android.cmake``, but normally these are
external components.

``Android.cmake`` is generic and can be used for any Android-based system
using Googles x-tools in the NDK. Just set your $PATH to the compilers you
intend to use.

Actually this build-method can be used for any Linux-system if you also set
the following options explicitly:

* SYSROOT 
* HAVE_ANDROID_OS
* XTOOL_PREFIX 

What you need to take special note about is the ``CMAKE_SYSROOT`` option.
This expands to ``SYSROOT`` and need to correspond to where your systems
headers and library-files are stored on your build-host. For the simple case
the X-tools have a default. But for Android, this is the part of the NDK and
must be defined explicitly. It usually looks something like the following:

```
$SOME_NDK_PATH/platforms/android-NN/arch-arm
```

**NOTE:** For most cases for Android, build-system should make a fair
guess and you can use the snippet below as is.

* For an Android build (recommended method):
```bash
mkdir BUILD_android
cd BUILD_android
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/Android.cmake  ../
ccmake .         #Adjust settings. Optional but do verify SYSROOT
make -j$(cat /proc/cpuinfo | grep processor | wc -l)
```

* Run-test
```bash
adb push posixtest /data/local/tmp/
adb shell /data/local/tmp/posixtest
```

Try it, run it. If it builds and runs, it works ;-)
