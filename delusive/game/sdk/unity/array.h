#pragma once
#include <cstdint>
#include "../../../vcruntime/memory/memory.h"

namespace unity {
    template <class T>
    class c_array {
    public:
        inline int count() {
            if (!this)
                return 0;

            return mem::read<int>(reinterpret_cast<uintptr_t>(this) + 0x18);
        }

        inline T get(int index) {
            if (!this)
                return { };

            return mem::read<T>(reinterpret_cast<uintptr_t>(this) + 0x20 + static_cast<unsigned long long>(index) * 0x8);
        }
    };
}
