# Kernel

-   `Kernel` - Backend, the main core of the program.

-   This project is a cross-platform `C++` project that support many operating systems such: `Windows x64+`, `Linux x64+`, `Macintosh x64+`, `iPhone iOS 10.0+`, `Android 7+`

## Development

-   [CMake 3.8+](https://cmake.org/)

-   [Powershell for Windows](https://learn.microsoft.com/en-us/powershell/)

-   [Ubuntu for Linux](https://ubuntu.com/tutorials/command-line-for-beginners)

-   [Terminal for Macintosh](https://developer.apple.com/library/archive/documentation/OpenSource/Conceptual/ShellScripting/Introduction/Introduction.html)

-   [MSVC 19.38 with MSVC for Windows](https://visualstudio.microsoft.com/downloads/)

-   [Clang 17.0 for Linux](https://llvm.org/)

-   [Clang 17.0 for Macintosh](https://llvm.org/)

-   [Clang 14.0 for Android (NDK r26.1)](https://developer.android.com/ndk/downloads)

-   [Clang 17.0 for iPhone](https://llvm.org/)

## Dependencies

-   [quickjs](https://github.com/bellard/quickjs/): JavaScript Engine used by `Kernel`, for `Clang`

-   [quickjs.c_smile](https://github.com/c-smile/quickjspp/): JavaScript Engine used by `Kernel`, for `MSVC`

-   [libpng](http://www.libpng.org/pub/png/libpng.html): PNG Decode/Encode

-   [zlib](https://www.zlib.net/): Zlib compress/uncompress

-   [fmt](https://github.com/fmtlib/fmt): Format library

-   [json](https://github.com/nlohmann/json): JSON Deserialize/Serialize

-   [tinyxml2](https://github.com/leethomason/tinyxml2): XML Deserialize/Serialize

-   [simdjson](https://simdjson.org/): JSON Deserialize

-   [md5](https://github.com/JieweiWei/md5): MD5 Hash

-   [open_vcdiff](https://github.com/google/open-vcdiff): Diff two file and generate the different after compare

-   [PvRTCCompressor](https://github.com/brenwill/PVRTCCompressor): PVRTC Encode/Decode

-   [SHA](https://github.com/pr0f3ss/SHA): SHA-384, SHA-512 Hash

-   [SHA256](https://github.com/System-Glitch/SHA256): Sha-256 Hash

-   [SHA224CryptoServiceProvider](https://github.com/redduxi/SHA-224): SHA-224 Hash

-   [Rijndael](#): Rijndael encrypt/decrypt

-   [etcpak](https://github.com/wolfpld/etcpak): ETC1 Encode algorithm

-   [zip](https://github.com/kuba--/zip): zip compress/uncompress

-   [lzma](https://www.7-zip.org/sdk.html): lzma compress/uncompress

-   [ETCPACK](https://github.com/Ericsson/ETCPACK): ETC1 Decode algorithm

-   [cpp-base64](https://github.com/ReneNyffenegger/cpp-base64): Base64 Encode/Decode

-   [compile-time-regular-expressions](https://github.com/hanickadot/compile-time-regular-expressions): Regex for modern C++, support `constexpr` regex

-   [bzip2](https://sourceware.org/bzip2): Bzip2 Compress/Uncompress

-   [avir](https://github.com/avaneev/avir): Image scaling algorithm

-   [dirent](https://github.com/tronkko/dirent): dirent for `MSVC`

## Modification

-   To make sure all the libraries work perfectly fine with `Kernel`, there are some modification compare to original project. All modification will be added `Haruma :: Add`, `Haruma :: Change`, `Haruma :: Delete`

-   Special thanks to [TwinStar](https://github.com/twinkles-twinstar/) for his modification on some libraries which makes all the process easier.

-   All the modification has been made:

|                         Library                          |                    Modification                    |
| :------------------------------------------------------: | :------------------------------------------------: |
|      [quickjs](https://github.com/bellard/quickjs/)      |         Adapt some `ES2022/ES2023` methods         |
| [quickjs.c_smile](https://github.com/c-smile/quickjspp/) |         Adapt some `ES2022/ES2023` methods         |
|       [etcpak](https://github.com/wolfpld/etcpak)        | Added `PubDecodeETCRGB` methods in `BlockData.cpp` |
