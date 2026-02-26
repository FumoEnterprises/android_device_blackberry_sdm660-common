/*
 * Stub implementations for Skia symbols needed by OREO camera.sdm660.so
 * These are used for watermark text drawing which is not needed on LOS22.
 * tryAllocPixels returns false to prevent any actual drawing.
 */

#include <stdint.h>
#include <stdarg.h>

#define EXPORT __attribute__((visibility("default")))

EXPORT void sk_abort_no_print() {}
EXPORT void SkDebugf(const char*, ...) {}

struct SkImageInfo {};

class EXPORT SkPaint {
    char _pad[128];
public:
    SkPaint();
    ~SkPaint();
    void setTextSize(float);
    void setARGB(unsigned int, unsigned int, unsigned int, unsigned int);
};

SkPaint::SkPaint() { __builtin_memset(_pad, 0, sizeof(_pad)); }
SkPaint::~SkPaint() {}
void SkPaint::setTextSize(float) {}
void SkPaint::setARGB(unsigned int, unsigned int, unsigned int, unsigned int) {}

class EXPORT SkBitmap {
    char _pad[128];
public:
    SkBitmap();
    ~SkBitmap();
    bool tryAllocPixels(const SkImageInfo&, unsigned int);
};

SkBitmap::SkBitmap() { __builtin_memset(_pad, 0, sizeof(_pad)); }
SkBitmap::~SkBitmap() {}
bool SkBitmap::tryAllocPixels(const SkImageInfo&, unsigned int) { return false; }

enum SkBlendMode : int {};

class EXPORT SkCanvas {
    char _pad[128];
public:
    SkCanvas(const SkBitmap&);
    ~SkCanvas();
    void drawText(const void*, unsigned int, float, float, const SkPaint&);
    void drawColor(unsigned int, SkBlendMode);
};

SkCanvas::SkCanvas(const SkBitmap&) { __builtin_memset(_pad, 0, sizeof(_pad)); }
SkCanvas::~SkCanvas() {}
void SkCanvas::drawText(const void*, unsigned int, float, float, const SkPaint&) {}
void SkCanvas::drawColor(unsigned int, SkBlendMode) {}
