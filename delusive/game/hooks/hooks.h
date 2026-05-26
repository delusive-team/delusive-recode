#pragma once
#include <array>
#include <unordered_map>
#include "../sdk/sdk.h"
#include "../../vcruntime/memory/memory.h"
#include "../../vcruntime/logger/logger.h"

namespace core {
    extern volatile bool g_unloading;
}

namespace memory {
    class c_virtual_hook {
        uint8_t* _original     = nullptr;  
        uint8_t* _replace      = nullptr;  
        uint8_t* _vtable_entry = nullptr;  

        bool _initialized = false;

    public:
        c_virtual_hook() = default;

        template <typename T, typename _ty2>
        __forceinline c_virtual_hook(T procedure, _ty2 replace, const char* klass = nullptr, const char* method = nullptr) {
            if (!procedure) {
                LOG_WARN("c_virtual_hook - procedure is null, hook skipped");
                return;
            }

            _replace      = reinterpret_cast<uint8_t*>(replace);
            _vtable_entry = reinterpret_cast<uint8_t*>(procedure);
            _original     = mem::read<uint8_t*>(procedure);

            if (!_replace || !_original) {
                LOG_WARN("c_virtual_hook - replace or original is null");
                return;
            }

            mem::write<_ty2>(procedure, replace);
            _initialized = true;

        }

        template <typename T>
        __forceinline auto call(T /*fn*/) {
            return reinterpret_cast<T>(_original);
        }

        __forceinline bool unhook() {
            if (_vtable_entry && _original) {
                mem::write<uint8_t*>(reinterpret_cast<uintptr_t>(_vtable_entry), _original);
                LOG("c_virtual_hook - unhooked vtable entry %p", _vtable_entry);
            }
            _initialized = false;
            return true;
        }

        __forceinline void reset() {
            _original     = nullptr;
            _replace      = nullptr;
            _vtable_entry = nullptr;
            _initialized  = false;
        }

        __forceinline uint8_t* get()          { return _original; }
        __forceinline bool&    initialized()   { return _initialized; }
        __forceinline bool&    intialized()    { return _initialized; }
    };
} 

#pragma section(".shared", read, write, shared)
__declspec(allocate(".shared")) inline volatile bool g_hooks_initialized = false;

struct hooks_t {

    bool is_initialized() const {
        return g_hooks_initialized;
    }

    void mark_initialized() {
        g_hooks_initialized = true;
        LOG("hooks_t - marked initialized");
    }

    template <typename T>
    memory::c_virtual_hook& get(T hk_addr) {
        return *_hooks.at(reinterpret_cast<uintptr_t>(hk_addr));
    }

    template <typename T>
    T call(T hk_addr) {
        auto it = _hooks.find(reinterpret_cast<uintptr_t>(hk_addr));
        if (it == _hooks.end())
            return hk_addr;
        return reinterpret_cast<T>(it->second->get());
    }

    template <typename T, typename _ty2>
    memory::c_virtual_hook& add(T procedure, _ty2 replace, const char* klass = nullptr, const char* method = nullptr) {
        auto existing = _hooks.find(reinterpret_cast<uintptr_t>(replace));
        if (existing != _hooks.end()) {
            if (klass && method) {
                LOG_WARN("hooks_t::add - detour %s::%s already registered, skipping", klass, method);
            } else {
                LOG_WARN("hooks_t::add - detour %p already registered, skipping", replace);
            }
            return *existing->second;
        }

        if (!procedure) {
            LOG_ERROR("hooks_t::add - procedure is null, hook not added");
            static memory::c_virtual_hook dummy;
            return dummy;
        }

        for (auto& hk : reserve) {
            if (hk.intialized())
                continue;

            hk = memory::c_virtual_hook{ procedure, replace, klass, method };
            _hooks.insert({ reinterpret_cast<uintptr_t>(replace), &hk });
            if (klass && method) {
                LOG("hooks_t::add - hook added %s::%s, total: %zu", klass, method, _hooks.size());
            } else {
                LOG("hooks_t::add - hook added, total: %zu", _hooks.size());
            }
            break;
        }

        auto it = _hooks.find(reinterpret_cast<uintptr_t>(replace));
        if (it == _hooks.end()) {
            LOG_ERROR("hooks_t::add - reserve full (max 30 hooks), could not add hook");
            static memory::c_virtual_hook dummy;
            return dummy;
        }

        return *it->second;
    }

    template <typename T, typename _ty2>
    bool remove(T /*procedure*/, _ty2 replace) {
        const auto key = reinterpret_cast<uintptr_t>(replace);
        auto it = _hooks.find(key);
        if (it == _hooks.end()) {
            LOG_WARN("hooks_t::remove - detour %p not found", replace);
            return false;
        }

        it->second->unhook();
        it->second->reset();
        _hooks.erase(it);
        LOG("hooks_t::remove - hook removed, total: %zu", _hooks.size());
        return true;
    }

    void destroy_all() {
        LOG("hooks_t::destroy_all - removing %zu hooks", _hooks.size());

        for (auto& [key, hook] : _hooks) {
            hook->unhook();
            hook->reset();
        }

        _hooks.clear();
        g_hooks_initialized = false;

        LOG("hooks_t::destroy_all - done");
    }

private:
    std::array<memory::c_virtual_hook, 30>       reserve{};
    std::unordered_map<uintptr_t, memory::c_virtual_hook*> _hooks;
};

inline hooks_t g_hooks;

#define HOOK_ADD(ns, klass, method, detour) \
    do { \
        const char* _k = klass; \
        const char* _m = method; \
        g_hooks.add(il2cpp::get_virtual_method(ns, _k, _m), detour, _k, _m); \
    } while(0)

#define HOOK_REMOVE(ns, klass, method, detour) \
    g_hooks.remove(0, detour)

#define HOOK_ADD_N(ns, klass, method, arg_count, detour) \
    do { \
        const char* _k = klass; \
        const char* _m = method; \
        g_hooks.add(il2cpp::get_virtual_method(ns, _k, _m, arg_count), detour, _k, _m); \
    } while(0)

#define HOOK_REMOVE_N(ns, klass, method, arg_count, detour) \
    g_hooks.remove(0, detour)

#define HOOK_ADD_DIRECT(ns, klass, method, detour) \
    do { \
        const char* _k = klass; \
        const char* _m = method; \
        g_hooks.add(il2cpp::get_method<uintptr_t>(il2cpp::get_class(ns, _k), _m), detour, _k, _m); \
    } while(0)

#define HOOK_REMOVE_DIRECT(ns, klass, method, detour) \
    g_hooks.remove(0, detour)

#define HOOK_ADD_DIRECT_N(ns, klass, method, arg_count, detour) \
    do { \
        const char* _k = klass; \
        const char* _m = method; \
        g_hooks.add(il2cpp::get_method<uintptr_t>(il2cpp::get_class(ns, _k), _m, arg_count), detour, _k, _m); \
    } while(0)

#define HOOK_REMOVE_DIRECT_N(ns, klass, method, arg_count, detour) \
    g_hooks.remove(0, detour)

#include "impl/item_icon.h"
#include "impl/base_player.h"
#include "impl/client_late_update.h"
#include "impl/base_melee.h"
#include "impl/steam_platform.h"
#include "impl/client_on_disconnected.h"

namespace hooks {
    inline void initialize() {
        HOOK_ADD(_(""), _("ItemIcon"), _("TryToMove"), item_icon::try_to_move);
        HOOK_ADD(_(""), _("BasePlayer"), _("ClientInput"), base_player::client_input);
        HOOK_ADD(_(""), _("BasePlayer"), _("BlockSprint"), base_player::block_sprint);
        HOOK_ADD(_(""), _("PlayerWalkMovement"), _("ClientInput"), base_player::player_walk_movement_client_input);
        HOOK_ADD_DIRECT_N(_(""), _("Client"), _("LateUpdate"), 0, client::late_update);
        HOOK_ADD(_(""), _("BaseMelee"), _("DoAttack"), base_melee::do_attack);
        HOOK_ADD(_(""), _("BaseMelee"), _("ProcessAttack"), base_melee::process_attack);
        HOOK_ADD(_("Rust.Platform.Steam"), _("SteamPlatform"), _("Update"), steam_platform::steam_platform_update);
        HOOK_ADD_DIRECT_N(_("Network"), _("Client"), _("OnDisconnected"), 1, client::on_disconnected);
    }

    inline void detach() {
        convar::Graphics::set_fov(90.f);
        
        HOOK_REMOVE(_(""), _("ItemIcon"), _("TryToMove"), item_icon::try_to_move);
        HOOK_REMOVE(_(""), _("BasePlayer"), _("ClientInput"), base_player::client_input);
        HOOK_REMOVE(_(""), _("BasePlayer"), _("BlockSprint"), base_player::block_sprint);
        HOOK_REMOVE(_(""), _("PlayerWalkMovement"), _("ClientInput"), base_player::player_walk_movement_client_input);
        HOOK_REMOVE_DIRECT_N(_(""), _("Client"), _("LateUpdate"), 0, client::late_update);
        HOOK_REMOVE(_(""), _("BaseMelee"), _("DoAttack"), base_melee::do_attack);
        HOOK_REMOVE(_(""), _("BaseMelee"), _("ProcessAttack"), base_melee::process_attack);
        HOOK_REMOVE(_("Rust.Platform.Steam"), _("SteamPlatform"), _("Update"), steam_platform::steam_platform_update);
        HOOK_REMOVE_DIRECT_N(_("Network"), _("Client"), _("OnDisconnected"), 1, client::on_disconnected);
    }
}
