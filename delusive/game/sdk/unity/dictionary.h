#pragma once
#include <cstdint>
#include "../../../vcruntime/memory/memory.h"

namespace unity {
    template <typename key_t, typename value_t = uintptr_t>
    class c_dictionary {
    public:
        inline int count() {
            if (!this)
                return 0;

            return mem::read<int>(reinterpret_cast<uintptr_t>(this) + 0x40);
        }

        inline uintptr_t entries() {
            if (!this)
                return { };

            return mem::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0x18);
        }

        inline key_t key(int index) {
            if (!this)
                return { };

            return mem::read<key_t>(this->entries() + 0x28 + static_cast<unsigned long long>(index) * 0x18);
        }

        inline value_t value(int index) {
            if (!this)
                return { };

            return mem::read<value_t>(this->entries() + 0x30 + static_cast<unsigned long long>(index) * 0x18);
        }
    };
}
