#pragma once
#include "../il2cpp/il2cpp.h"
#include "unity/asset_bundle.h"
#include "unity/shader.h"
#include "unity/material.h"
#include "unity/game_object.h"
#include "unity/resources.h"
#include "unity/string.h"
#include <string>

namespace sdk {
	namespace bundles {
		inline bool initialized = false;

		inline unity::AssetBundle* chams_bundle = nullptr;

		inline unity::Shader* flat_shader = nullptr;

		inline unity::Shader* fire_a_shader = nullptr;
		inline unity::Material* fire_a_material = nullptr;

		inline unity::Shader* fire_b_shader = nullptr;
		inline unity::Material* fire_b_material = nullptr;

		inline unity::Shader* glitter_shader = nullptr;
		inline unity::Material* glitter_material = nullptr;

		inline unity::Shader* damascus_shader = nullptr;
		inline unity::Material* damascus_material = nullptr;

		inline unity::Shader* lightning_shader = nullptr;
		inline unity::Material* lightning_material = nullptr;

		inline unity::Shader* geometric_shader = nullptr;
		inline unity::Material* geometric_material = nullptr;

		inline unity::Shader* wire_frame_shader = nullptr;
		inline unity::Material* wire_frame_material = nullptr;

		inline unity::GameObject* ghost_hit_effect_prefab = nullptr;

		inline unity::AssetBundle* find_loaded_bundle(const char* target_name) {
			uintptr_t asset_bundle_type = il2cpp::type_object(_("UnityEngine"), _("AssetBundle"));
			if (!asset_bundle_type) return nullptr;

			auto array = unity::Resources::find_objects_of_type_all<unity::AssetBundle*>(asset_bundle_type);
			if (!memory::is_valid(array)) return nullptr;

			uint32_t count = array->count();
			for (uint32_t i = 0; i < count; i++) {
				auto bundle = array->get(i);
				if (!memory::is_valid(bundle)) continue;

				auto name_str = reinterpret_cast<unity::Object*>(bundle)->name();
				if (memory::is_valid(name_str)) {
					std::string name = name_str->string();
					if (name.find(target_name) != std::string::npos) {
						return bundle;
					}
				}
			}
			return nullptr;
		}

		inline bool initialize() {
			if (initialized)
				return false;

			if (!chams_bundle) {
				chams_bundle = find_loaded_bundle(_("delusive"));
				if (!chams_bundle) {
					chams_bundle = unity::AssetBundle::load_from_file(_("C:\\Delusive\\Rust\\Bundles\\delusive.bundle"));
				}
			}

			if (memory::is_valid(chams_bundle)) {
				auto shader_type = il2cpp::type_object(_("UnityEngine"), _("Shader"));
				auto mat_type = il2cpp::type_object(_("UnityEngine"), _("Material"));
				auto go_type = il2cpp::type_object(_("UnityEngine"), _("GameObject"));

				flat_shader = chams_bundle->load_asset<unity::Shader*>(_("chams.shader"), shader_type);

				fire_a_shader = chams_bundle->load_asset<unity::Shader*>(_("new amplifyshader.shader"), shader_type);
				fire_a_material = chams_bundle->load_asset<unity::Material*>(_("fire.mat"), mat_type);

				fire_b_shader = chams_bundle->load_asset<unity::Shader*>(_("new amplifyshader.shader"), shader_type);
				fire_b_material = chams_bundle->load_asset<unity::Material*>(_("fire2.mat"), mat_type);

				glitter_shader = chams_bundle->load_asset<unity::Shader*>(_("el_glitter.shader"), shader_type);
				glitter_material = chams_bundle->load_asset<unity::Material*>(_("el_glitter.mat"), mat_type);

				damascus_shader = chams_bundle->load_asset<unity::Shader*>(_("el_designshader.shader"), shader_type);
				damascus_material = chams_bundle->load_asset<unity::Material*>(_("el_designeffect.mat"), mat_type);

				lightning_shader = chams_bundle->load_asset<unity::Shader*>(_("poiyomi pro.shader"), shader_type);
				lightning_material = chams_bundle->load_asset<unity::Material*>(_("lightning.mat"), mat_type);

				geometric_shader = chams_bundle->load_asset<unity::Shader*>(_("poiyomi pro geometric dissolve.shader"), shader_type);
				geometric_material = chams_bundle->load_asset<unity::Material*>(_("galaxy.mat"), mat_type);

				wire_frame_shader = chams_bundle->load_asset<unity::Shader*>(_("poiyomi pro wireframe.shader"), shader_type);
				wire_frame_material = chams_bundle->load_asset<unity::Material*>(_("wireframe.mat"), mat_type);

				ghost_hit_effect_prefab = chams_bundle->load_asset<unity::GameObject*>(_("cfxr2 souls escape.prefab"), go_type);
			}

			initialized = true;
			return true;
		}

		inline bool destroy_all() {
			if (!initialized)
				return false;

			if (memory::is_valid(chams_bundle)) {
				chams_bundle->unload(true);
				chams_bundle = nullptr;
			}

			flat_shader = nullptr;
			fire_a_shader = nullptr;
			fire_a_material = nullptr;
			fire_b_shader = nullptr;
			fire_b_material = nullptr;
			glitter_shader = nullptr;
			glitter_material = nullptr;
			damascus_shader = nullptr;
			damascus_material = nullptr;
			lightning_shader = nullptr;
			lightning_material = nullptr;
			geometric_shader = nullptr;
			geometric_material = nullptr;
			wire_frame_shader = nullptr;
			wire_frame_material = nullptr;
			ghost_hit_effect_prefab = nullptr;

			initialized = false;
			return true;
		}
	}
}
