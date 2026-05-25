#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cstdarg>

namespace util {
    inline std::string format(const char* format, ...) {
        va_list args;
        va_start(args, format);

        va_list args_copy;
        va_copy(args_copy, args);

        const int message_length = vsnprintf(nullptr, 0, format, args_copy);
        va_end(args_copy);

        if (message_length <= 0) {
            va_end(args);
            return std::string();
        }

        std::string message(message_length, '\0');
        vsnprintf(&message[0], message_length + 1, format, args);

        va_end(args);
        return message;
    }

    inline char d2c(unsigned digit) {
        return (digit <= 9) ? '0' + digit : 'a' + (digit - 10);
    }
}