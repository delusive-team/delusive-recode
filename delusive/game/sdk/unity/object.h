#pragma once

#include "../../il2cpp/il2cpp.h"
#include "array.h"
#include "../math/vector.h"

namespace unity {
    struct c_string;
    struct MonitorData;
    struct Il2CppClass;

    using Il2CppVTable = Il2CppClass;

    struct Il2CppObject {
        union {
            Il2CppClass* klass = nullptr;
            Il2CppVTable* vtable;
        };
        MonitorData* monitor = nullptr;
    };

    class Object : public Il2CppObject {
    private:
        static inline uintptr_t get_name_ptr = 0;
        static inline uintptr_t destroy_immediate_ptr = 0;
        static inline uintptr_t find_object_of_type_ptr = 0;
        static inline uintptr_t dont_destroy_on_load_ptr = 0;
        static inline uintptr_t find_objects_of_type_ptr = 0;
        static inline uintptr_t internal_instantiate_single_ptr = 0;
        static inline uintptr_t find_object_from_instance_id_ptr = 0;

    public:
        static void initialize() {
            get_name_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Object")), _("get_name"), -1));
            destroy_immediate_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Object")), _("DestroyImmediate"), 1));
            find_object_of_type_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Object")), _("FindObjectOfType"), 1));
            dont_destroy_on_load_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Object")), _("DontDestroyOnLoad"), -1));
            find_objects_of_type_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Object")), _("FindObjectsOfType"), 1));
            internal_instantiate_single_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Object")), _("Internal_InstantiateSingle"), 3));
            find_object_from_instance_id_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("Object")), _("FindObjectFromInstanceID"), 1));
        }

        inline void dont_destroy_on_load() {
            safe.call<void>(dont_destroy_on_load_ptr, this);
        }

        inline void destroy() {
            safe.call<void>(destroy_immediate_ptr, this);
        }

        inline c_string* name() {
            return safe.call<c_string*>(get_name_ptr, this);
        }

        inline const char* class_name() {
            const auto& oc = mem::read<uintptr_t>(this);
            if (!memory::is_valid(oc))
                return { };

            return mem::read<const char*>(oc + 0x10);
        }

        inline bool is_class(const char* name) {
            return HASH_EQUAL(RHASH(class_name()), name);
        }

        template <class T>
        static T find_from_instance_id(int instance_id) {
            return safe.call<T>(find_object_from_instance_id_ptr, instance_id);
        }

        template <class T>
        static T find_object_of_type(uintptr_t type) {
            return safe.call<T>(find_object_of_type_ptr, type);
        }

        template <class T>
        static c_array<T>* find_objects_of_type(uintptr_t type) {
            return safe.call<c_array<T>*>(find_objects_of_type_ptr, type);
        }

        template <class Q>
        static void instantiate(Object* original, vec3_t pos, Q rot) {
            safe.call<void>(internal_instantiate_single_ptr, original, pos, rot);
        }
    };
}
