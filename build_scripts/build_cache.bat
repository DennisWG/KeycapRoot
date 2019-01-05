IF NOT EXIST "C:\build_cache" GOTO BUILD_DIR
GOTO END

:BUILD_DIR

md C:\build_cache
cd C:\build_cache

echo "calling build_zlib"
call build_scripts\build_zlib.bat

cd C:\build_cache

echo "calling build_botan"
call build_scripts\build_botan.bat

:END
dir C:\build_cache\zlib-1.2.11\build\%configuration%\
echo %configuration%

set ZLIB_LIBRARY="C:\build_cache\zlib-1.2.11\build\%configuration%\zlibstatic.lib"
if %configuration% == "Debug" set ZLIB_LIBRARY="C:\build_cache\zlib-1.2.11\build\%configuration%\zlibstaticd.lib"
set ZLIB_INCLUDE_DIR="C:\build_cache\zlib-1.2.11"

set Botan_INCLUDE_DIR="C:\build_cache\Botan-2.8.0\build\include"
set Botan_LIBRARIES="C:\build_cache\Botan-2.8.0\build\%configuration%\botan.lib"