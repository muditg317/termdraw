set -e

build_type=RelWithDebInfo
build_type=Debug
build_type=Release

_cmake=$(command -v cmake)
if [ -z "$_cmake" ]; then
    echo "cmake not found"
    exit 1
fi

_cc=$(command -v clang)
cxx=$(command -v clang++)
toolchain=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake

proj_dir=$(pwd)
build_dir=$proj_dir/build

rm -rf build
$_cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=$build_type -DCMAKE_C_COMPILER:FILEPATH=$_cc -DCMAKE_CXX_COMPILER:FILEPATH=$cxx -DCMAKE_TOOLCHAIN_FILE:FILEPATH=$toolchain -H$proj_dir -B$build_dir -G "Unix Makefiles"
