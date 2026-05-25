#pragma once
#include "../crypto/xor.h"
#include <Windows.h>
#include <stdio.h>

namespace logger {

    bool initialize();

    void write_to_file(const char* text);

    inline const char* format(const char* fmt, ...) {
        static char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        return buf;
    }

    inline const char* build(const char* file, int line,
        const char* func, const char* level,
        const char* msg)
    {
        static char buf[2048];

        SYSTEMTIME st;
        GetLocalTime(&st);

        const char* fname = file;
        for (const char* p = file; *p; p++)
            if (*p == '\\' || *p == '/') fname = p + 1;

        snprintf(buf, sizeof(buf),
            "[%02d:%02d:%02d.%03d] [%s:%d] [%s] %s%s\n",
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
            fname, line,
            func,
            level, msg);

        return buf;
    }

} // namespace logger

#ifdef _DEBUG

#define LOG(fmt, ...) \
    do { \
        const char* _msg  = logger::format(fmt, ##__VA_ARGS__); \
        const char* _line = logger::build(__FILE__, __LINE__, __FUNCTION__, "", _msg); \
        printf("%s", _line); \
        logger::write_to_file(_line); \
    } while(0)

#define LOG_WARN(fmt, ...) \
    do { \
        const char* _msg  = logger::format(fmt, ##__VA_ARGS__); \
        const char* _line = logger::build(__FILE__, __LINE__, __FUNCTION__, "[WARN] ", _msg); \
        printf("%s", _line); \
        logger::write_to_file(_line); \
    } while(0)

#define LOG_ERROR(fmt, ...) \
    do { \
        const char* _msg  = logger::format(fmt, ##__VA_ARGS__); \
        const char* _line = logger::build(__FILE__, __LINE__, __FUNCTION__, "[ERROR] ", _msg); \
        printf("%s", _line); \
        logger::write_to_file(_line); \
    } while(0)

#define LOG_OK(fmt, ...) \
    do { \
        const char* _msg  = logger::format(fmt, ##__VA_ARGS__); \
        const char* _line = logger::build(__FILE__, __LINE__, __FUNCTION__, "[OK] ", _msg); \
        printf("%s", _line); \
        logger::write_to_file(_line); \
    } while(0)

#else

#define LOG(fmt, ...)       do {} while(0)
#define LOG_WARN(fmt, ...)  do {} while(0)
#define LOG_ERROR(fmt, ...) do {} while(0)
#define LOG_OK(fmt, ...)    do {} while(0)

#endif

#define PAUSE() getchar()