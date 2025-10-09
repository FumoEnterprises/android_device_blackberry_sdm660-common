#include <android-base/logging.h>

extern "C" void* _ZN7android4base10LogMessageC1EPKcjNS0_5LogIdENS0_11LogSeverityES3_i(const char* file, unsigned int line, android::base::LogId id, android::base::LogSeverity severity, const char* tag, int error);

extern "C" void* _ZN7android4base10LogMessageC1EPKcjNS0_5LogIdENS0_11LogSeverityEi(const char* file, unsigned int line,android::base::LogId id, android::base::LogSeverity severity,int error) {
    return _ZN7android4base10LogMessageC1EPKcjNS0_5LogIdENS0_11LogSeverityES3_i(file, line, id, severity, "<unknown>", error);
}

/*
extern "C" void _ZN7android4base10LogMessageC1EPKcjNS0_5LogIdENS0_11LogSeverityEi(
    android::base::LogMessage* this_ptr,
    const char* file,
    unsigned int line,
    android::base::LogId id,
    android::base::LogSeverity severity,
    int error)
{
    // Call the newer constructor with a fake tag
    new (this_ptr) android::base::LogMessage(file, line, id, severity, "<unknown>", error);
} */
