IF NOT EXIST Botan-2.8.0 GOTO BUILD
GOTO END

:BUILD
appveyor DownloadFile https://github.com/randombit/botan/archive/2.13.0.zip -FileName Botan-2.8.0.tgz
7z x Botan-2.8.0.tgz > NUL
7z x Botan-2.8.0.tar > NUL
cd Botan-2.8.0
configure.py --with-cmake --cpu=%platform%
cd build
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config %configuration%

:END