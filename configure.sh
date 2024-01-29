set -e

build_type=RelWithDebInfo
build_type=Debug
build_type=Release

if [ -z "$VCPKG_ROOT" ]; then
    echo "VCPKG_ROOT not set"
    exit 1
fi

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
mkdir -p $build_dir

# rm -rf build
find $build_dir -mindepth 1 -maxdepth 1 ! -regex "^$build_dir/vcpkg\(.*\)?" -exec rm -rf '{}' \;
echo "cmake: $_cmake"
$_cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=$build_type -DCMAKE_C_COMPILER:FILEPATH=$_cc -DCMAKE_CXX_COMPILER:FILEPATH=$cxx -DCMAKE_TOOLCHAIN_FILE:FILEPATH=$toolchain -H$proj_dir -B$build_dir -G "Unix Makefiles"
