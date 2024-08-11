# mcpelauncher-dcblock

This is a mod for mcpelauncher-manifest that blocks double clicking so you don't get banned from servers like zeqa.


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

## Conf
The conf file is at `<data dir>/dcblock.conf`. Options:  
`BlockRightDc` - Controls if right double clicks are blocked.  
`LogClicks` - Controls if clicks should be logged to console. Useful for debugging or if you want to prove you are using dc block.  
`UseLegacyMousefeed` - Set this to true if you are using a version below 1.19.60. Otherwise, leave this at false.
