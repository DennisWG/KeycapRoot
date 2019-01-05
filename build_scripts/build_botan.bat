IF NOT EXIST Botan-2.8.0 GOTO BUILD
GOTO END

:BUILD
appveyor DownloadFile https://botan.randombit.net/releases/Botan-2.8.0.tgz -FileName Botan-2.8.0.tgz
7z x Botan-2.8.0.tgz > NUL
7z x Botan-2.8.0.tar > NUL
cd Botan-2.8.0
configure.py --with-cmake --cpu=%platform%
cd build
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config %configuration%

:END