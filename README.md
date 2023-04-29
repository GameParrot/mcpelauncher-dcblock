# mcpelauncher-dcblock

This is a mod for mcpelauncher-manifest that blocks double clicking so you don't get banned from servers like zeqa. It supports 1.19.60+.


## Building
`PATH_TO_NDK="/path/to/ndk"`
- x86

  `cmake -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86 ..`
- x86_64

  `cmake -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86_64 ..`
- armeabi-v7a

  `cmake -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a ..`
- arm64-v8a

  `cmake -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a ..`

## Versions older than 1.19.60
To use with versions older than 1.19.60, you will have to replace `_ZN5Mouse4feedEcassss` with `_ZN5Mouse4feedEccssss`. You will also have to change the signed chars to chars.
