IF NOT EXIST build_cache GOTO BUILD_DIR
GOTO END

:BUILD_DIR

md build_cache
cd build_cache

appveyor DownloadFile http://zlib.net/zlib-1.2.11.tar.gz -FileName zlib-1.2.11.tar.gz
7z x zlib-1.2.11.tar.gz > NUL
7z x zlib-1.2.11.tar > NUL
cd zlib-1.2.11
md build
cd build
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config %configuration%

cd ..\..

appveyor DownloadFile https://botan.randombit.net/releases/Botan-2.8.0.tgz -FileName Botan-2.8.0.tgz
7z x Botan-2.8.0.tgz > NUL
7z x Botan-2.8.0.tar > NUL
cd Botan-2.8.0
configure.py --with-cmake --cpu=%platform%
cd build
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config %configuration%

:END
copy zlib-1.2.11\build\Botan-2.8.0 build_cache\Botan-2.8.0

set ZLIB_LIBRARY="%APPVEYOR_BUILD_FOLDER%\build_cache\zlib-1.2.11\build\%configuration%\zlib.lib"
set ZLIB_INCLUDE_DIR="%APPVEYOR_BUILD_FOLDER%\zlib-1.2.11"

set Botan_INCLUDE_DIR="%APPVEYOR_BUILD_FOLDER%\build_cache\Botan-2.8.0\build\include"
set Botan_LIBRARIES="%APPVEYOR_BUILD_FOLDER%\build_cache\Botan-2.8.0\build\%configuration%\botan.lib"