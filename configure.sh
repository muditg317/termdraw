set -e

_cmake=/usr/bin/cmake
_cc=/bin/clang
cxx=/bin/clang++-10
toolchain=/home/muditg317/application-installs/vcpkg/scripts/buildsystems/vcpkg.cmake

proj_dir=$(pwd)
build_dir=$proj_dir/build

rm -rf build
$_cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo -DCMAKE_C_COMPILER:FILEPATH=$_cc -DCMAKE_CXX_COMPILER:FILEPATH=$cxx -DCMAKE_TOOLCHAIN_FILE:FILEPATH=$toolchain -H$proj_dir -B$build_dir -G "Unix Makefiles"
