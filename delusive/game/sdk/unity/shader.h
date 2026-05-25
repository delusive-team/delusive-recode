#pragma once
#include "../../il2cpp/il2cpp.h"
#include "object.h"

namespace unity {
    class Shader : public Object {
    private:
        static inline uintptr_t find_ptr = 0;

    public:
        static void initialize() {
            find_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Shader")), _("Find"), 1));
        }

        static Shader* find(const char* name) {
            return safe.call<Shader*>(find_ptr, il2cpp::new_string(name));
        }
    };
}
