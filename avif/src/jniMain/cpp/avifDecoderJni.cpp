#include <jni.h>
#include <iostream>

#include "avif/avif.h"
#include "helperJni.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_seiko_avif_AvifDecoder_versionString(JNIEnv *env, jclass type) {
    char codec_versions[256];
    avifCodecVersions(codec_versions);

    char libyuv_version[64];
    if (avifLibYUVVersion() > 0) {
        sprintf(libyuv_version, "%u",avifLibYUVVersion());
    } else {
        libyuv_version[0] = '\0';
    }

    char version_string[512];
    snprintf(version_string, sizeof(version_string),
             "libavif: %s\nCodecs: %s\nlibyuv: %s",
             avifVersion(),
             codec_versions,
             libyuv_version);
    return env->NewStringUTF(version_string);
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_seiko_avif_AvifDecoder_isAvifImage(JNIEnv *env, jclass type,
                                            jbyteArray byteArray,
                                            jint length) {
    jbyte *buf = env->GetByteArrayElements(byteArray, nullptr);
    const avifROData avif = {(const uint8_t *) buf, static_cast<size_t>(length)};
    return avifPeekCompatibleFileType(&avif);
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_seiko_avif_AvifDecoder_createContext(JNIEnv *env, jclass type,
                                              jbyteArray byteArray,
                                              jint length,
                                              jint threads) {
    avifDecoder *decoder = avifDecoderCreate();
    decoder->maxThreads = threads;
    decoder->ignoreExif = AVIF_TRUE;
    decoder->ignoreXMP = AVIF_TRUE;

    // Turn off libavif's 'clap' (clean aperture) property validation. This allows
    // us to detect and ignore streams that have an invalid 'clap' property
    // instead failing.
    decoder->strictFlags &= ~AVIF_STRICT_CLAP_VALID;
    // Allow 'pixi' (pixel information) property to be missing. Older versions of
    // libheif did not add the 'pixi' item property to AV1 image items (See
    // crbug.com/1198455).
    decoder->strictFlags &= ~AVIF_STRICT_PIXI_REQUIRED;

    jbyte *buf = env->GetByteArrayElements(byteArray, nullptr);
    avifResult result = avifDecoderSetIOMemory(decoder, (const uint8_t *) buf, length);
    if (result != AVIF_RESULT_OK) {
        throwIllegalStateException(env, "Failed to set AVIF IO to a memory reader: %s.",
                                   avifResultToString(result));
        goto cleanup;
    }
    result = avifDecoderParse(decoder);
    if (result != AVIF_RESULT_OK) {
        throwIllegalStateException(env, "Failed to parse AVIF image: %s.",
                                   avifResultToString(result));
        goto cleanup;
    }
    return reinterpret_cast<jlong>(decoder);

    cleanup:
    avifDecoderDestroy(decoder);
    return 0;
}

extern "C" JNIEXPORT void JNICALL
Java_com_seiko_avif_AvifDecoder_destroyContext(JNIEnv *env, jobject type, jlong context) {
    avifDecoder *decoder = reinterpret_cast<avifDecoder *>(context);
    avifDecoderDestroy(decoder);
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_seiko_avif_AvifDecoder_reset(JNIEnv *env, jobject type, jlong context) {
    avifDecoder *decoder = reinterpret_cast<avifDecoder *>(context);
    avifResult result = avifDecoderReset(decoder);
    return result == AVIF_RESULT_OK;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_seiko_avif_AvifDecoder_nthFrame(JNIEnv *env, jobject type, jlong context, jint index) {
    avifDecoder *decoder = reinterpret_cast<avifDecoder *>(context);
    avifResult result = avifDecoderNthImage(decoder, index);
    return result == AVIF_RESULT_OK;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_seiko_avif_AvifDecoder_nextFrame(JNIEnv *env, jobject type, jlong context) {
    avifDecoder *decoder = reinterpret_cast<avifDecoder *>(context);
    avifResult result = avifDecoderNextImage(decoder);
    return result == AVIF_RESULT_OK;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_seiko_avif_AvifDecoder_getFrame(JNIEnv *env, jobject type, jlong context) {
    avifDecoder *decoder = reinterpret_cast<avifDecoder *>(context);
    avifImage *image = decoder->image;
    return reinterpret_cast<jlong>(image);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_seiko_avif_AvifDecoder_getFrameIndex(JNIEnv *env, jobject type, jlong context) {
    avifDecoder *decoder = reinterpret_cast<avifDecoder *>(context);
    return decoder->imageIndex;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_seiko_avif_AvifDecoder_getFrameCount(JNIEnv *env, jobject type, jlong context) {
    avifDecoder *decoder = reinterpret_cast<avifDecoder *>(context);
    return decoder->imageCount;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_seiko_avif_AvifDecoder_getFrameDurationMs(JNIEnv *env, jobject type, jlong context) {
    avifDecoder *decoder = reinterpret_cast<avifDecoder *>(context);
    return decoder->imageTiming.duration * 1000; //ms
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_seiko_avif_AvifDecoder_getAlphaPresent(JNIEnv *env, jobject type, jlong context) {
    avifDecoder *decoder = reinterpret_cast<avifDecoder *>(context);
    return decoder->alphaPresent;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_seiko_avif_AvifDecoder_getRepetitionCount(JNIEnv *env, jobject type, jlong context) {
    avifDecoder *decoder = reinterpret_cast<avifDecoder *>(context);
    return decoder->repetitionCount;
}
