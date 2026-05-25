#pragma once
#include <cstdint>
#include "../../../vcruntime/memory/memory.h"

namespace unity {
    class c_list_dictionary {
    public:
        template <class T>
        T value() {
            return mem::read<T>(reinterpret_cast<uintptr_t>(this) + 0x28);
        }

        int count() {
            const auto& val = value<uintptr_t>();
            if (!memory::is_valid(val))
                return 0;

            return mem::read<int>(val + 0x10);
        }

        uintptr_t buffer() {
            const auto& val = value<uintptr_t>();
            if (!memory::is_valid(val))
                return { };

            return mem::read<uintptr_t>(val + 0x18);
        }

        template <class T>
        T get(int32_t index) {
            return mem::read<T>(buffer() + 0x20 + (index * 0x8));
        }
    };
}
