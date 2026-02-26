/*
 * Stub for libjnigraphics symbols needed by OREO camera vendor libs.
 * libjnigraphics.so is not available in vendor namespace on Android 12+.
 * These functions are called by libopencv_java3.so and libVDSuperPhotoAPI.so
 * for bitmap processing that is not essential on LOS22.
 */

#include <stdint.h>

#define EXPORT __attribute__((visibility("default")))

/* From android/bitmap.h */
#define ANDROID_BITMAP_RESULT_SUCCESS 0
#define ANDROID_BITMAP_RESULT_BAD_PARAMETER -1

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    int32_t format;
    uint32_t flags;
} AndroidBitmapInfo;

extern "C" {

EXPORT int AndroidBitmap_getInfo(void* /*env*/, void* /*jbitmap*/, AndroidBitmapInfo* info) {
    if (!info) return ANDROID_BITMAP_RESULT_BAD_PARAMETER;
    info->width = 0;
    info->height = 0;
    info->stride = 0;
    info->format = 0;
    info->flags = 0;
    return ANDROID_BITMAP_RESULT_SUCCESS;
}

EXPORT int AndroidBitmap_lockPixels(void* /*env*/, void* /*jbitmap*/, void** addrPtr) {
    if (addrPtr) *addrPtr = nullptr;
    return ANDROID_BITMAP_RESULT_BAD_PARAMETER;
}

EXPORT int AndroidBitmap_unlockPixels(void* /*env*/, void* /*jbitmap*/) {
    return ANDROID_BITMAP_RESULT_SUCCESS;
}

}
