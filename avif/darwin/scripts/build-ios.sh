cd libavif || exit 255

# START dav1d
if ! [ -f ext/dav1d ]; then
  git clone -b 1.2.1 --depth 1 https://code.videolan.org/videolan/dav1d.git ext/dav1d
fi
cd ext/dav1d || exit 255

rm -rf "build"
mkdir "build"
cd "build" || exit 255

echo "ios_cross_file: ${IOS_CROSS_FILE}"
echo "ios_toolchain_cmake: ${IOS_TOOLCHAIN_FILE}"

meson setup \
  --cross-file="${IOS_CROSS_FILE}" \
  --default-library=static \
  --buildtype=release \
  -Db_lto=false \
  -Db_ndebug=false \
  -Denable_asm=false \
  -Denable_tools=false \
  -Denable_examples=false \
  -Denable_tests=false \
  ..
ninja

cd ..
cd ../..
# END dav1d

# START avif
build_dir="_build-ios_${ARCH}"
rm -rf "${build_dir}"
mkdir -p "${build_dir}"

cmake -B "${build_dir}" -G Xcode ${IOS_CMAKE_PARAMS} \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -DAVIF_CODEC_DAV1D=ON \
  -DAVIF_LOCAL_DAV1D=ON \
  -DAVIF_LOCAL_LIBYUV=OFF \
  -DAVIF_LOCAL_LIBSHARPYUV=OFF
cmake --build "${build_dir}" --config Release
# END avif

# START copy *.a & rm cache dir
mkdir -p "${IOS_OUTPUT_DIR}"

cp -v ext/dav1d/build/src/*.a "${IOS_OUTPUT_DIR}" || exit 255
cp -v ${build_dir}/Release-*/*.a "${IOS_OUTPUT_DIR}" || exit 255

rm -rf "ext/dav1d/build"
rm -rf "${build_dir}"
# END copy *.a & rm cache dir
