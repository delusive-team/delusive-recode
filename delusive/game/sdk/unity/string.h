#pragma once
#include <string>
#include "object.h"
#include "../../il2cpp/il2cpp.h"
#include "../system/string.h"

namespace unity {
    struct c_string : Il2CppObject {
        const wchar_t* c_str() const {
            return m_firstChar;
        }

        auto string() {
            std::wstring ws = m_firstChar;
            return std::string(ws.begin(), ws.end());
        }

        wchar_t* begin() {
            return m_firstChar;
        }

        wchar_t* end() {
            return (begin() + m_stringLength);
        }

        const wchar_t* begin() const {
            return m_firstChar;
        }

        const wchar_t* end() const {
            return (begin() + m_stringLength);
        }

        std::int32_t m_stringLength = 0;
        wchar_t m_firstChar[1] = {};

        bool contains(const char* str) {
            if (!this)
                return false;

            return safe.call<bool>(sys::String::contains_ptr, this, il2cpp::new_string(str));
        }
    };
}
