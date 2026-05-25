#pragma once
#include <cstdint>
#include "../../../vcruntime/memory/memory.h"

namespace unity {
    template<class T>
    class c_list {
    public:
        T get(uint32_t idx) {
            if (!this)
                return { };

            return mem::read<T>(reinterpret_cast<uintptr_t>(this) + 0x20 + idx * sizeof(T));
        }

        T value(uint32_t idx) {
            if (!this)
                return { };

            const auto& list = mem::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0x10);
            if (!memory::is_valid(list))
                return { };

            return mem::read<T>(list + 0x20 + idx * sizeof(T));
        }

        uint32_t count() {
            return mem::read<uint32_t>(reinterpret_cast<uintptr_t>(this) + 0x18);
        }
    };
}
