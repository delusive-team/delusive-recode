#pragma once
#include "../../il2cpp/il2cpp.h"
#include "string.h"
#include "object.h"

namespace unity {
	class AssetBundle : public Object {
	private:
		static inline uintptr_t unload_ptr = 0;
		static inline uintptr_t load_from_file_ptr = 0;
		static inline uintptr_t load_asset_internal_ptr = 0;
		static inline uintptr_t load_from_memory_internal_ptr = 0;

	public:
		static void initialize() {
			unload_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("AssetBundle")), _("Unload"), 1));
			load_from_file_ptr = mem::read<uintptr_t>(il2cpp::get_method<uintptr_t>(il2cpp::get_class(_("UnityEngine"), _("AssetBundle")), _("LoadFromFile"), 1));
			
			load_asset_internal_ptr = il2cpp::resolve_icall(_("UnityEngine.AssetBundle::LoadAsset_Internal"));
			load_from_memory_internal_ptr = il2cpp::resolve_icall(_("UnityEngine.AssetBundle::LoadFromMemory_Internal"));
		}

		inline void unload(bool unload_all_loaded_objects) {
			safe.call<AssetBundle*>(unload_ptr, this, unload_all_loaded_objects);
		}

		template <class T>
		inline T load_asset(const char* name, uintptr_t type_object) {
			return safe.call<T>(load_asset_internal_ptr, this, il2cpp::new_string(name), type_object);
		}

		static AssetBundle* load_from_file(const char* path) {
			return safe.call<AssetBundle*>(load_from_file_ptr, il2cpp::new_string(path));
		}

		template <class T>
		static AssetBundle* load_from_memory(T data) {
			return safe.call<AssetBundle*>(load_from_memory_internal_ptr, data, 0, 0);
		}
	};
}
