@echo off

set CONFIG=Debug
for /f "delims=" %%A in ('"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -find **\VsDevCmd.bat') do set "VSCMD=%%A"


if not defined VSCMD (
    echo Visual Studio not found!
    PAUSE
    exit
)

if %CONFIG% == Debug (
	REM Path to spdlog
	cd Thingy/vendor/spdlog
	if not exist build (
		mkdir build
	)
	cd build

	REM Build spdlog shared library
	cmake ..
	cmake --build .

	REM Path to SDL
	cd "%~dp0"Thingy/vendor/SDL_mixer/external/SDL
	
	if not exist sdl_build (
		mkdir sdl_build
	)
	cd sdl_build

	REM Build SDL shared library
	cmake .. -G "Visual Studio 16 2022" -DBUILD_SHARED_LIBS=ON
	cmake --build . --config Debug

	REM Path to SDL_mixer
	cd "%~dp0"Thingy/vendor/SDL_mixer
	if not exist build (
		mkdir build
	)
	cd build

	REM Build SDL_mixer shared library
	cmake .. -DBUILD_SHARED_LIBS=ON
	cmake --build . --config Debug


	REM Path to curl
	cd "%~dp0"Thingy/vendor/curl

	REM run Curl prebuild config 
	call buildconf.bat

	cd winbuild
	REM cd winbuild
	call "%VSCMD%"
	REM Build curl shared library in Debug mode
	nmake /f Makefile.vc mode=dll Debug=yes clean
) else (
	REM Build spdlog shared library
	cd Thingy/vendor/spdlog
	if not exist build (
		mkdir build
	)
	cd build
	cmake ..
	cmake --build .

	REM Path to SDL
	cd "%~dp0"Thingy/vendor/SDL_mixer/external/SDL
	
	if not exist sdl_build (
		mkdir sdl_build
	)
	cd sdl_build

	REM Build SDL shared library
	cmake .. -G "Visual Studio 16 2022" -DBUILD_SHARED_LIBS=ON
	cmake --build . --config Release

	REM Path to SDL_mixer
	cd "%~dp0"Thingy/vendor/SDL_mixer
	if not exist build (
		mkdir build
	)
	cd build

	REM Build SDL_mixer shared library
	cmake .. -DBUILD_SHARED_LIBS=ON
	cmake --build . --config Release


	REM Path to curl
	cd "%~dp0"Thingy/vendor/curl

	REM run Curl prebuild config 
	call buildconf.bat

	cd winbuild
	REM cd winbuild
	call "%VSCMD%"
	REM Build curl shared library in Debug mode
	nmake /f Makefile.vc mode=dll Debug=no clean
)

cd "%~dp0"

call GenerateProjects.bat

PAUSE