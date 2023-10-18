cd libavif || exit 255

# START libjpeg
if ! [ -f ext/libjpeg ]; then
  git clone --depth 1 https://github.com/joedrago/libjpeg.git ext/libjpeg
fi
# END libjpeg

# START libyuv
if ! [ -f ext/libyuv ]; then
  git clone --single-branch https://chromium.googlesource.com/libyuv/libyuv ext/libyuv
fi
cd ext/libyuv || exit 255

libyuv_build_dir="build"
rm -rf "${libyuv_build_dir}"
mkdir -p "${libyuv_build_dir}"

cmake -B "${libyuv_build_dir}" -G Ninja ${DARWIN_CMAKE_PARAMS} \
 -DCMAKE_BUILD_TYPE=Release \
 -DCMAKE_POSITION_INDEPENDENT_CODE=ON
ninja yuv -C "${libyuv_build_dir}"

cd ../..
# END libyuv

# START libwebp
if ! [ -f ext/libwebp ]; then
  git clone --single-branch https://chromium.googlesource.com/webm/libwebp ext/libwebp
fi
cd ext/libwebp || exit 255

git checkout e2c85878f6a33f29948b43d3492d9cdaf801aa54

libwebp_build_dir="build"
rm -rf "${libwebp_build_dir}"
mkdir -p "${libwebp_build_dir}"

cmake -B "${libwebp_build_dir}" -G Ninja ${DARWIN_CMAKE_PARAMS} \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF
ninja sharpyuv -C "${libwebp_build_dir}"

cd ../..
# END libwebp

# START dav1d
if ! [ -f ext/dav1d ]; then
  git clone -b 1.2.1 --depth 1 https://code.videolan.org/videolan/dav1d.git ext/dav1d
fi
cd ext/dav1d || exit 255

dav1d_build_dir="build"
rm -rf "${dav1d_build_dir}"
mkdir -p "${dav1d_build_dir}"

meson setup "${dav1d_build_dir}" ${DARWIN_MESON_PARAMS} \
  --default-library=static \
  --buildtype release \
  -Denable_tools=false \
  -Denable_tests=false
ninja -C "${dav1d_build_dir}"

cd ../..
# END dav1d

# START avif
build_dir="_build-native"
rm -rf "${build_dir}"
mkdir -p "${build_dir}"

cmake -B "${build_dir}" ${DARWIN_CMAKE_PARAMS} \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -DAVIF_CODEC_DAV1D=ON \
  -DAVIF_LOCAL_DAV1D=ON \
  -DAVIF_LOCAL_JPEG=ON \
  -DAVIF_LOCAL_LIBYUV=ON \
  -DAVIF_LOCAL_LIBSHARPYUV=ON
cmake --build "${build_dir}" --config Release
# END avif

# START copy *.a & rm cache dir
mkdir -p "${DARWIN_OUTPUT_DIR}"

cp -v "ext/dav1d/build/src/libdav1d.a" "${DARWIN_OUTPUT_DIR}" || exit 255
cp -v "${build_dir}/libavif.a" "${DARWIN_OUTPUT_DIR}" || exit 255

rm -rf "ext/libyuv/build"
rm -rf "ext/libwebp/build"
rm -rf "ext/dav1d/build"
rm -rf "${build_dir}"
# END copy *.a & rm cache dir
