IF NOT EXIST zlib-1.2.11 GOTO BUILD
GOTO END

:BUILD
appveyor DownloadFile http://zlib.net/zlib-1.2.11.tar.gz -FileName zlib-1.2.11.tar.gz
7z x zlib-1.2.11.tar.gz > NUL
7z x zlib-1.2.11.tar > NUL
cd zlib-1.2.11
md build
cd build
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config %configuration%
copy zconf.h ..\zconf.h

:END