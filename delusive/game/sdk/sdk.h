#pragma once

#include <cmath>
#include <algorithm>
#include <vector>

#include "../../thirdparty/imgui/imgui.h"

// enums
#include "enums/enums.h"

#include "math/vector.h"
#include "math/matrix.h"
#include "math/rect.h"
#include "math/bounds.h"

//convar
#include "convar/admin.h"
#include "convar/graphics.h"
#include "convar/input.h"
#include "convar/culling.h"
#include "convar/client.h"
#include "convar/weather.h"

#include "model_state.h"
#include "animator.h"
#include "base_movement.h"
#include "player_walk_movement.h"
#include "steam_platform.h"
#include "entity_realm.h"
#include "base_networkable.h"
#include "base_entity.h"
#include "base_combat_entity.h"
#include "base_mountable.h"
#include "base_player.h"
#include "local_player.h"
#include "world_item.h"
#include "workbench.h"
#include "base_view_model.h"
#include "view_model.h"
#include "viewmodel_bob.h"
#include "viewmodel_lower.h"
#include "viewmodel_sway.h"
#include "stash_container.h"
#include "player_tick.h"
#include "trace_info.h"
#include "asset_name_cache.h"
#include "projectile.h"
#include "skinned_multi_mesh.h"
#include "player_model.h"
#include "player_inventory.h"
#include "player_input.h"
#include "player_team.h"
#include "player_eyes.h"
#include "model.h"
#include "map_interface.h"
#include "ui_inventory.h"
#include "main_camera.h"
#include "magazine.h"
#include "lootable_corpse.h"
#include "building_block.h"
#include "building_privlidge.h"
#include "dropped_item_container.h"
#include "game_manager.h"
#include "game_physics.h"
#include "game_trace.h"
#include "console_system.h"
#include "base_melee.h"
#include "recoil_properties.h"
#include "held_entity.h"
#include "hit_info.h"
#include "hit_test.h"
#include "input_message.h"
#include "input_state.h"
#include "item.h"
#include "item_container.h"
#include "item_definition.h"
#include "item_icon.h"
#include "game_manifest.h"
#include "translate_phrase.h"
#include "bundles.h"
#include "game_object_ref.h"
#include "item_mod_projectile.h"

//tod_sky
#include "tod_ambient_parameters.h"
#include "tod_cycle_parameters.h"
#include "tod_day_parameters.h"
#include "tod_night_parameters.h"
#include "tod_sun_parameters.h"
#include "tod_world_parameters.h"
#include "tod_atmosphere_parameters.h"
#include "tod_cloud_parameters.h"
#include "tod_sky.h"

//protobuf
#include "protobuf/attack.h"
#include "protobuf/entity.h"
#include "protobuf/environment.h"
#include "protobuf/player_attack.h"
#include "protobuf/player_name_id.h"
#include "protobuf/player_projectile_attack.h"
#include "protobuf/player_team.h"
#include "protobuf/projectile.h"
#include "protobuf/projectileshoot.h"
#include "protobuf/player_projectile_update.h"

//system
#include "system/string.h"

//unity
#include "unity/object.h"
#include "unity/game_object.h"
#include "unity/component.h"
#include "unity/camera.h"
#include "unity/transform.h"
#include "unity/behaviour.h"
#include "unity/rigid_body.h"
#include "unity/asset_bundle.h"
#include "unity/string.h"
#include "unity/screen.h"
#include "unity/color.h"
#include "unity/texture.h"
#include "unity/sprite.h"
#include "unity/shader.h"
#include "unity/material.h"
#include "unity/renderer.h"
#include "unity/quaternion.h"
#include "unity/time.h"
#include "unity/input.h"
#include "unity/ray.h"
#include "unity/raycast_hit.h"
#include "unity/physics.h"
#include "unity/collider.h"
#include "unity/capsule_collider.h"
#include "unity/box_collider.h"
#include "unity/sphere_collider.h"
#include "unity/list.h"
#include "unity/array.h"
#include "unity/list_dictionary.h"
#include "unity/dictionary.h"
#include "unity/resources.h"

namespace sdk {
    inline void initialize() {
        convar::Admin::initialize();
        convar::Graphics::initialize();
        convar::Input::initialize();
        convar::Culling::initialize();
        convar::Client::initialize();
        convar::Weather::initialize();
        
        ModelState::initialize();
        BaseMovement::initialize();
        PlayerWalkMovement::initialize();
        RandomUsernames::initialize();
        SteamPlatform::initialize();
        Animator::initialize();

        sdk::BaseNetworkable::initialize();
        sdk::BaseEntity::initialize();
        sdk::BaseCombatEntity::initialize();
        sdk::BaseMountable::initialize();
        sdk::BasePlayer::initialize();
        sdk::LocalPlayer::initialize();

        WorldItem::initialize();
        Workbench::initialize();
        BaseViewModel::initialize();
        ViewModel::initialize();
        ViewmodelBob::initialize();
        ViewmodelLower::initialize();
        ViewmodelSway::initialize();
        StashContainer::initialize();
        PlayerTick::initialize();
        AssetNameCache::initialize();
        sdk::Projectile::initialize();
        sdk::SkinnedMultiMesh::initialize();
        sdk::PlayerModel::initialize();
        sdk::PlayerInventory::initialize();
        sdk::PlayerInput::initialize();
        //sdk::PlayerTeam::initialize();
        //sdk::PlayerTeamMember::initialize();
        sdk::PlayerEyes::initialize();
        sdk::Model::initialize();
        sdk::MapInterface::initialize();
        sdk::UIInventory::initialize();
        sdk::MainCamera::initialize();
        sdk::Magazine::initialize();
        sdk::LootableCorpse::initialize();
        sdk::BuildingBlock::initialize();
        sdk::BuildingPrivlidge::initialize();
        sdk::DroppedItemContainer::initialize();
        sdk::GameManager::initialize();
        sdk::GamePhysics::initialize();
        sdk::GameTrace::initialize();
        sdk::ConsoleSystem::initialize();
        sdk::BaseMelee::initialize();
        sdk::RecoilProperties::initialize();
        sdk::HeldEntity::initialize();
        sdk::HitInfo::initialize();
        sdk::HitTest::initialize();
        sdk::InputMessage::initialize();
        sdk::InputState::initialize();
        sdk::Item::initialize();
        sdk::ItemContainer::initialize();
        sdk::ItemDefinition::initialize();
        sdk::ItemIcon::initialize();
        sdk::GameManifest::initialize();
        sdk::TranslatePhrase::initialize();
        sdk::ItemModProjectile::initialize();

        unity::Object::initialize();

        TOD_AmbientParameters::initialize();
        TOD_CycleParameters::initialize();
        TOD_DayParameters::initialize();
        TOD_NightParameters::initialize();
        TOD_SunParameters::initialize();
        TOD_StarsParameters::initialize();
        TOD_MoonParameters::initialize();
        TOD_CloudParameters::initialize();
        TOD_WorldParameters::initialize();
        TOD_AtmosphereParameters::initialize();
        TOD_Sky::initialize();
        
        protobuf::Attack::initialize();
        protobuf::Entity::initialize();
        protobuf::Environment::initialize();
        protobuf::PlayerAttack::initialize();
        protobuf::PlayerNameID::initialize();
        protobuf::PlayerProjectileAttack::initialize();
        protobuf::PlayerTeam::initialize();
        protobuf::TeamMember::initialize();
        protobuf::Projectile::initialize();
        protobuf::ProjectileShoot::initialize();
        protobuf::PlayerProjectileUpdate::initialize();
        
        sys::String::initialize();
        
        unity::Object::initialize();
        unity::GameObject::initialize();
        unity::Component::initialize();
        unity::Camera::initialize();
        unity::Transform::initialize();
        unity::Behaviour::initialize();
        unity::RigidBody::initialize();
        unity::AssetBundle::initialize();
        unity::Screen::initialize();
        unity::Shader::initialize();
        unity::Material::initialize();
        unity::Renderer::initialize();
        unity::Quaternion::initialize();
        unity::Time::initialize();
        unity::Input::initialize();
        unity::Physics::initialize();
        unity::Sprite::initialize();
        unity::Collider::initialize();
        unity::CapsuleCollider::initialize();
        unity::BoxCollider::initialize();
        unity::SphereCollider::initialize();
        unity::Resources::initialize();
    }

    inline unity::Camera* camera;

    inline vec3_t local_position;
    inline BasePlayer* local_player;

    inline int server_hour = 12;
    inline int server_minute = 0;

    inline void clear()
    {
        camera = nullptr;
        local_player = nullptr;

        info.clear();

        bundles::destroy_all();
    }
}
