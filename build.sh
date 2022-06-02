set -e

_cmake=$(command -v cmake)
if [ -z "$_cmake" ]; then
    echo "cmake not found"
    exit 1
fi

proj_dir=$(pwd)
build_dir=$proj_dir/build

if [ ! -d "$build_dir" ]; then
   echo "'$DIR' not found! Configuring CMake..."
   ./configure.sh
fi

$_cmake --build $build_dir --config RelWithDebInfo --target all -j 14 --
