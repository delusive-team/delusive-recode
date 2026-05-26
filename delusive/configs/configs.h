#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <windows.h>
#include <shlobj.h>
#include "../vcruntime/crypto/xor.h"
#include "../imgui/imgui.h"

extern std::string g_discord_username;
extern std::string g_discord_id;

constexpr uint32_t cfg_hash(const char* s, uint32_t h = 2166136261u) {
    return *s ? cfg_hash(s + 1, (h ^ (uint8_t)*s) * 16777619u) : h;
}
#define CFG_HASH(s) cfg_hash(s)

static constexpr const char* BUILD_VERSION = "1.0.0 debug";
static constexpr const char* BUILD_DATE = __DATE__ " " __TIME__;

struct config_meta {
    std::string author;
    std::string created;
    std::string version = _("1.0.0");
    std::string description;
    std::string discord_user_id;
    std::string config_id;
    uint64_t    last_sync_timestamp = 0;
    bool        is_cloud_synced = false;
};

namespace rconfig_detail {
    inline std::vector<std::string> split(const std::string& str, const std::string& delim) {
        std::vector<std::string> result;
        std::string::size_type pos = 0, prev = 0;
        while ((pos = str.find(delim, prev)) != std::string::npos) {
            result.push_back(str.substr(prev, pos - prev));
            prev = pos + delim.size();
        }
        result.push_back(str.substr(prev));
        return result;
    }
}

class rconfig {
public:
    rconfig() = default;
    explicit rconfig(std::string file_path);

    void read();
    void write();
    bool is_stale();
    bool is_dirty() const { return m_dirty; }
    void clear_dirty() { m_dirty = false; }

    std::string export_base64() const;
    bool        import_base64(const std::string& b64);

    config_meta meta;
    void write_meta(const std::string& author, const std::string& created, const std::string& discord_id);
    void read_meta();

    void set_path(const std::string& new_path) { path = new_path; }
    std::string get_path() const { return path; }
    void clear_records() { records.clear(); m_dirty = true; }

    template<class T> T    get(std::string name, T def);
    template<class T> void set(std::string name, T value);

    template<> inline float get<float>(std::string name, float def) {
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end == name.c_str()) return def;
        auto r = get_record(hash);
        if (r.empty()) return def;
        char* val_end;
        float val = std::strtof(r.c_str(), &val_end);
        return (val_end == r.c_str()) ? def : val;
    }

    template<> inline void set<float>(std::string name, float value) {
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end != name.c_str()) set_record(hash, std::to_string(value));
    }

    template<> inline int get<int>(std::string name, int def) {
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end == name.c_str()) return def;
        auto r = get_record(hash);
        if (r.empty()) return def;
        char* val_end;
        long val = std::strtol(r.c_str(), &val_end, 10);
        return (val_end == r.c_str()) ? def : static_cast<int>(val);
    }

    template<> inline void set<int>(std::string name, int value) {
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end != name.c_str()) set_record(hash, std::to_string(value));
    }

    template<> inline bool get<bool>(std::string name, bool def) {
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end == name.c_str()) return def;
        auto r = get_record(hash);
        return r.empty() ? def : (r == _("true") || r == _("1"));
    }

    template<> inline void set<bool>(std::string name, bool value) {
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end != name.c_str()) set_record(hash, value ? _("true") : _("false"));
    }

    template<> inline std::string get<std::string>(std::string name, std::string def) {
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end == name.c_str()) return def;
        auto r = get_record(hash);
        return r.empty() ? def : r;
    }

    template<> inline void set<std::string>(std::string name, std::string value) {
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end != name.c_str()) set_record(hash, value);
    }

    template<> inline ImColor get<ImColor>(std::string name, ImColor def) {
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end == name.c_str()) return def;

        auto r = get_record(hash);
        if (r.empty()) return def;

        auto p = rconfig_detail::split(r, _(";"));
        if (p.size() != 4) return def;

        ImColor res;
        char* end;
        res.Value.x = std::strtof(p[0].c_str(), &end); if (end == p[0].c_str()) return def;
        res.Value.y = std::strtof(p[1].c_str(), &end); if (end == p[1].c_str()) return def;
        res.Value.z = std::strtof(p[2].c_str(), &end); if (end == p[2].c_str()) return def;
        res.Value.w = std::strtof(p[3].c_str(), &end); if (end == p[3].c_str()) return def;

        return res;
    }

    template<> inline void set<ImColor>(std::string name, ImColor value) {
        std::string s = std::to_string(value.Value.x) + _(";") + std::to_string(value.Value.y) + _(";") +
            std::to_string(value.Value.z) + _(";") + std::to_string(value.Value.w);
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end != name.c_str()) set_record(hash, s);
    }

    template<> inline std::vector<float> get<std::vector<float>>(std::string name, std::vector<float> def) {
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end == name.c_str()) return def;
        auto r = get_record(hash);
        if (r.empty()) return def;
        auto p = rconfig_detail::split(r, _(";"));
        std::vector<float> res;
        for (auto& s : p) {
            if (!s.empty()) {
                char* end;
                float val = std::strtof(s.c_str(), &end);
                if (end != s.c_str()) res.push_back(val);
            }
        }
        return res;
    }

    template<> inline void set<std::vector<float>>(std::string name, std::vector<float> value) {
        if (value.empty()) return;
        std::string s = std::to_string(value[0]);
        for (size_t i = 1; i < value.size(); i++) s += _(";") + std::to_string(value[i]);
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end != name.c_str()) set_record(hash, s);
    }

    template<> inline std::vector<int> get<std::vector<int>>(std::string name, std::vector<int> def) {
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end == name.c_str()) return def;
        auto r = get_record(hash);
        if (r.empty()) return def;
        auto p = rconfig_detail::split(r, _(";"));
        std::vector<int> res;
        for (auto& s : p) {
            if (!s.empty()) {
                char* end;
                long val = std::strtol(s.c_str(), &end, 10);
                if (end != s.c_str()) res.push_back(static_cast<int>(val));
            }
        }
        return res;
    }

    template<> inline void set<std::vector<int>>(std::string name, std::vector<int> value) {
        if (value.empty()) return;
        std::string s = std::to_string(value[0]);
        for (size_t i = 1; i < value.size(); i++) s += _(";") + std::to_string(value[i]);
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end != name.c_str()) set_record(hash, s);
    }

    template<> inline std::vector<bool> get<std::vector<bool>>(std::string name, std::vector<bool> def) {
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end == name.c_str()) return def;
        auto r = get_record(hash);
        if (r.empty()) return def;
        auto p = rconfig_detail::split(r, _(";"));
        std::vector<bool> res;
        for (auto& s : p) {
            res.push_back(s == _("1") || s == _("true"));
        }
        return res;
    }

    template<> inline void set<std::vector<bool>>(std::string name, std::vector<bool> value) {
        if (value.empty()) return;
        std::string s = value[0] ? _("1") : _("0");
        for (size_t i = 1; i < value.size(); i++)
            s += _(";") + (value[i] ? std::string(_("1")) : std::string(_("0")));
        char* name_end;
        uint32_t hash = std::strtoul(name.c_str(), &name_end, 10);
        if (name_end != name.c_str()) set_record(hash, s);
    }

private:
    std::string get_record(uint32_t hash) const;
    void        set_record(uint32_t hash, const std::string& value);
    uint32_t    get_config_hash() const;

    std::string path;
    std::string raw_cfg;
    uint32_t    last_hash = 0;
    bool        m_dirty = false;

    std::vector<std::pair<uint32_t, std::string>> records;
};

namespace config {


    class i_config_item {
    public:
        virtual void  read() = 0;
        virtual void  save() = 0;
        virtual void* get_val_ptr() = 0;
        std::string   name;
    };


    namespace manager {
        inline std::string get_base_path() { return _("C:\\Delusive\\Rust\\Configs\\"); }
        inline std::string get_ext() { return _(".dl"); }
        inline std::string get_settings_path() { return _("C:\\Delusive\\Rust\\settings.dl"); }

        inline rconfig                  config;
        inline rconfig                  settings_config;
        inline std::string              current_config_name;
        inline std::vector<std::string> config_list;
        inline float                    autosave_timer = 0.f;
        inline bool                     autosave_enabled = true;
        static constexpr float          AUTOSAVE_INTERVAL = 10.f;

        inline std::vector<i_config_item*>& get_items() {
            static std::vector<i_config_item*>* ptr = nullptr;
            if (!ptr) ptr = new std::vector<i_config_item*>();
            return *ptr;
        }

        inline std::vector<i_config_item*>& get_settings_items() {
            static std::vector<i_config_item*>* ptr = nullptr;
            if (!ptr) ptr = new std::vector<i_config_item*>();
            return *ptr;
        }

        void        init();
        void        save(const std::string& name);
        void        load(const std::string& name);
        void        save_settings();
        void        load_settings();
        void        refresh_list();
        void        tick_autosave(float dt);
        bool        config_exists(const std::string& name);
        bool        is_protected(const std::string& name);
        void        delete_config(const std::string& name);
        config_meta get_meta(const std::string& name);
        std::string export_config_full(const std::string& name);
        bool        import_config_full(const std::string& encoded);

        inline void read_config() { config.read(); }
        inline void write_config() { config.write(); }

        inline void load() {
            read_config();
            for (auto& item : get_items()) item->read();
        }

        inline void save() {
            for (auto& item : get_items()) item->save();
            write_config();
        }

        inline void autosave() {
            for (auto& item : get_items()) item->save();
            if (config.is_dirty()) {
                write_config();
            }
        }
    }

    template <class T>
    class config_item : public i_config_item {
    public:
        T           value;
        T           default_value;
        const char* item_name;

        config_item(const char* _name, T _value)
            : value(_value), default_value(_value), item_name(_name)
        {
            name = std::to_string(CFG_HASH(_name));
        }

        void register_self() { manager::get_items().push_back(this); }

        operator T() { return value; }
        operator T* () { return &value; }
        T& operator=(const T& v) { value = v; return value; }

        void  read()        override { value = manager::config.get<T>(name, default_value); }
        void  save()        override { manager::config.set<T>(name, value); }
        void* get_val_ptr() override { return &value; }
    };

    inline std::vector<i_config_item*>& get_pending_items() {
        static std::vector<i_config_item*>* ptr = nullptr;
        if (!ptr) ptr = new std::vector<i_config_item*>();
        return *ptr;
    }

    template <class T>
    class auto_config_item : public config_item<T> {
    public:
        auto_config_item(const char* _name, T _value)
            : config_item<T>(_name, _value)
        {
            get_pending_items().push_back(this);
        }
    };

    inline void register_all_items() {
        auto& pending = get_pending_items();
        auto& items = manager::get_items();
        for (auto* item : pending) items.push_back(item);
        pending.clear();
    }



#define CFG_ITEM(type, name, def_val) \
    inline config::auto_config_item<type> name { #name, def_val }
}

namespace config {
    namespace esp {

        CFG_ITEM(float, players_max_render_distance, 400.f);
        CFG_ITEM(bool, players_only_visible, false);
        CFG_ITEM(bool, players_show_npcs, false);
        CFG_ITEM(bool, players_show_sleepers, false);
        CFG_ITEM(bool, players_show_wounded, false);
        CFG_ITEM(bool, players_show_teammates, false);

        CFG_ITEM(bool, players_box, true);
        CFG_ITEM(int, players_box_type, 0); // 0 = normal, 1 = corner
        CFG_ITEM(bool, players_box_fill, false);
        CFG_ITEM(ImColor, players_box_color, ImColor(255, 255, 255, 255));
        CFG_ITEM(ImColor, players_box_fill_color, ImColor(255, 255, 255, 100));

        CFG_ITEM(bool, players_skeleton, false);
        CFG_ITEM(float, players_skeleton_thickness, 1.f);
        CFG_ITEM(bool, players_skeleton_fingers, false);
        CFG_ITEM(bool, players_skeleton_joints, false);
        CFG_ITEM(ImColor, players_skeleton_color, ImColor(255, 255, 255, 255));

        CFG_ITEM(bool, players_health, true);
        CFG_ITEM(ImColor, players_health_color, ImColor(255, 145, 45, 255));

        CFG_ITEM(bool, players_name, true);
        CFG_ITEM(int, players_name_case, 0); // 0 = normal, 1 = upper, 2 = lower
        CFG_ITEM(int, players_name_font, 0);
        CFG_ITEM(int, players_name_style, 1);
        CFG_ITEM(ImColor, players_name_color, ImColor(255, 255, 255, 255));

        CFG_ITEM(bool, players_distance, true);
        CFG_ITEM(int, players_distance_case, 0);
        CFG_ITEM(int, players_distance_font, 0);
        CFG_ITEM(int, players_distance_style, 1);
        CFG_ITEM(ImColor, players_distance_color, ImColor(200, 200, 200, 255));

        CFG_ITEM(bool, players_weapon, false);
        CFG_ITEM(int, players_weapon_type, 0); // 0 = text, 1 = icons, 2 = flat
        CFG_ITEM(int, players_weapon_case, 0);
        CFG_ITEM(int, players_weapon_font, 0);
        CFG_ITEM(int, players_weapon_style, 1);
        CFG_ITEM(ImColor, players_weapon_color, ImColor(200, 200, 200, 255));

        CFG_ITEM(bool, players_health_text, true);
        CFG_ITEM(int, players_health_text_pos, 1); // 0 = inside, 1 = outside

        CFG_ITEM(std::vector<int>, players_flags_list, {});

        // статусы и флаги (позиции: 0 = top, 1 = bot, 2 = left, 3 = right)
        CFG_ITEM(bool, players_flags, false);
        CFG_ITEM(bool, players_flag_visible, false);
        CFG_ITEM(int, players_flags_visible_pos, 3);
        CFG_ITEM(bool, players_flag_zooming, false);
        CFG_ITEM(int, players_flags_zooming_pos, 3);
        CFG_ITEM(bool, players_flag_safezone, false);
        CFG_ITEM(int, players_flags_safezone_pos, 3);
        CFG_ITEM(bool, players_flag_inside, false);
        CFG_ITEM(int, players_flags_inside_pos, 3);
        CFG_ITEM(bool, players_flag_mount, false);
        CFG_ITEM(int, players_flags_mount_pos, 3);
        CFG_ITEM(bool, players_flag_sleep, false);
        CFG_ITEM(int, players_flags_sleep_pos, 3);
        CFG_ITEM(bool, players_flag_team, false);
        CFG_ITEM(int, players_flags_team_pos, 3);
        CFG_ITEM(ImColor, players_flags_color, ImColor(200, 200, 200, 255));

        CFG_ITEM(bool, players_snapline, false);
        CFG_ITEM(int, players_snapline_origin, 1); // 0 = top, 1 = center, 2 = bottom
        CFG_ITEM(ImColor, players_snapline_color, ImColor(255, 255, 255, 255));

        CFG_ITEM(bool, players_view_direction, false);
        CFG_ITEM(ImColor, players_view_direction_color, ImColor(255, 255, 255, 255));

        CFG_ITEM(bool, players_oof_arrows, false);
        CFG_ITEM(ImColor, players_oof_arrows_color, ImColor(255, 0, 0, 255));
        CFG_ITEM(bool, players_oof_arrows_rainbow, false);
        CFG_ITEM(int, players_oof_arrows_style, 0); // 0 = flat, 1 = cursor, 2 = curved
        CFG_ITEM(float, players_oof_arrows_radius, 150.f);
        CFG_ITEM(bool, players_oof_arrows_pulse, false);

        // настройки превью (позиции: 0 = free, 1 = top, 2 = bot, 3 = left, 4 = right)
        CFG_ITEM(float, prev_name_ox, 0.f); CFG_ITEM(float, prev_name_oy, -4.f); CFG_ITEM(float, prev_name_sz, 11.f); CFG_ITEM(int, prev_name_align, 0);
        CFG_ITEM(float, prev_health_ox, -4.f); CFG_ITEM(float, prev_health_oy, 0.f); CFG_ITEM(float, prev_health_sz, 3.f); CFG_ITEM(int, prev_health_align, 2);
        CFG_ITEM(float, prev_dist_ox, 0.f); CFG_ITEM(float, prev_dist_oy, 4.f); CFG_ITEM(float, prev_dist_sz, 10.f); CFG_ITEM(int, prev_dist_align, 1);
        CFG_ITEM(float, prev_weap_ox, 0.f); CFG_ITEM(float, prev_weap_oy, 18.f); CFG_ITEM(float, prev_weap_sz, 10.f); CFG_ITEM(int, prev_weap_align, 1);

        CFG_ITEM(float, prev_flag_visible_ox, 0.f); CFG_ITEM(float, prev_flag_visible_oy, 0.f); CFG_ITEM(float, prev_flag_visible_sz, 10.f); CFG_ITEM(int, prev_flag_visible_case, 1); CFG_ITEM(int, prev_flag_visible_font, 0); CFG_ITEM(int, prev_flag_visible_style, 1); CFG_ITEM(ImColor, prev_flag_visible_color, ImColor(200, 200, 200, 255));
        CFG_ITEM(float, prev_flag_zooming_ox, 0.f); CFG_ITEM(float, prev_flag_zooming_oy, 14.f); CFG_ITEM(float, prev_flag_zooming_sz, 10.f); CFG_ITEM(int, prev_flag_zooming_case, 1); CFG_ITEM(int, prev_flag_zooming_font, 0); CFG_ITEM(int, prev_flag_zooming_style, 1); CFG_ITEM(ImColor, prev_flag_zooming_color, ImColor(200, 200, 200, 255));
        CFG_ITEM(float, prev_flag_safezone_ox, 0.f); CFG_ITEM(float, prev_flag_safezone_oy, 28.f); CFG_ITEM(float, prev_flag_safezone_sz, 10.f); CFG_ITEM(int, prev_flag_safezone_case, 1); CFG_ITEM(int, prev_flag_safezone_font, 0); CFG_ITEM(int, prev_flag_safezone_style, 1); CFG_ITEM(ImColor, prev_flag_safezone_color, ImColor(200, 200, 200, 255));
        CFG_ITEM(float, prev_flag_inside_ox, 0.f); CFG_ITEM(float, prev_flag_inside_oy, 42.f); CFG_ITEM(float, prev_flag_inside_sz, 10.f); CFG_ITEM(int, prev_flag_inside_case, 1); CFG_ITEM(int, prev_flag_inside_font, 0); CFG_ITEM(int, prev_flag_inside_style, 1); CFG_ITEM(ImColor, prev_flag_inside_color, ImColor(200, 200, 200, 255));
        CFG_ITEM(float, prev_flag_mount_ox, 0.f); CFG_ITEM(float, prev_flag_mount_oy, 56.f); CFG_ITEM(float, prev_flag_mount_sz, 10.f); CFG_ITEM(int, prev_flag_mount_case, 1); CFG_ITEM(int, prev_flag_mount_font, 0); CFG_ITEM(int, prev_flag_mount_style, 1); CFG_ITEM(ImColor, prev_flag_mount_color, ImColor(200, 200, 200, 255));
        CFG_ITEM(float, prev_flag_sleep_ox, 0.f); CFG_ITEM(float, prev_flag_sleep_oy, 70.f); CFG_ITEM(float, prev_flag_sleep_sz, 10.f); CFG_ITEM(int, prev_flag_sleep_case, 1); CFG_ITEM(int, prev_flag_sleep_font, 0); CFG_ITEM(int, prev_flag_sleep_style, 1); CFG_ITEM(ImColor, prev_flag_sleep_color, ImColor(200, 200, 200, 255));
        CFG_ITEM(float, prev_flag_team_ox, 0.f); CFG_ITEM(float, prev_flag_team_oy, 84.f); CFG_ITEM(float, prev_flag_team_sz, 10.f); CFG_ITEM(int, prev_flag_team_case, 1); CFG_ITEM(int, prev_flag_team_font, 0); CFG_ITEM(int, prev_flag_team_style, 1); CFG_ITEM(ImColor, prev_flag_team_color, ImColor(200, 200, 200, 255));

    }

    namespace shared_esp {
        CFG_ITEM(bool, enabled, false);
        CFG_ITEM(bool, box,      true);
        CFG_ITEM(bool, name,     true);
        CFG_ITEM(bool, distance, true);
        CFG_ITEM(bool, health,   true);
        CFG_ITEM(bool, snapline, false);
        CFG_ITEM(ImColor, box_color,      ImColor(255, 100, 100, 255));
        CFG_ITEM(ImColor, name_color,     ImColor(255, 100, 100, 255));
        CFG_ITEM(ImColor, distance_color, ImColor(200, 200, 200, 255));
        CFG_ITEM(ImColor, health_color,   ImColor(255, 100, 100, 255));
        CFG_ITEM(ImColor, snapline_color, ImColor(255, 100, 100, 255));
    }
    namespace globals {
        inline std::string settings_default_config = "Default";
        inline bool settings_first_time;
    }

    namespace misc {
        namespace viewmodel {
            CFG_ITEM(bool, misc_viewmodel, false);
            CFG_ITEM(bool, misc_viewmodel_changer, false);
            CFG_ITEM(float, misc_viewmodel_X, 0.f);
            CFG_ITEM(float, misc_viewmodel_Y, 0.f);
            CFG_ITEM(float, misc_viewmodel_Z, 0.f);

            CFG_ITEM(bool, misc_no_viewmodel_bob, false);
            CFG_ITEM(bool, misc_no_viewmodel_sway, false);
            CFG_ITEM(bool, misc_no_viewmodel_lower, false);
            CFG_ITEM(bool, misc_no_attack_animation, false);
        }
        namespace view {
            CFG_ITEM(bool, misc_fov_changer, false);
            CFG_ITEM(float, misc_fov_changer_amount, 90.f);
            CFG_ITEM(bool, misc_zoom_changer, false);
            CFG_ITEM(int, misc_zoom_key, 0);
            CFG_ITEM(int, misc_zoom_key_mode, 0);
            CFG_ITEM(float, misc_zoom_value, 40.f);
            CFG_ITEM(bool, misc_aspect_ratio_changer, false);
            CFG_ITEM(float, misc_aspect_ratio_value, 1.777f);
            CFG_ITEM(bool, misc_camera_mode, false);
            CFG_ITEM(int, misc_camera_mode_value, 0);
            CFG_ITEM(float, misc_camera_mode_dist, 2.f);
            CFG_ITEM(int, misc_camera_mode_key, 0);
            CFG_ITEM(int, misc_camera_mode_key_mode, 0);
            CFG_ITEM(bool, misc_camera_to_bullet, false);
            CFG_ITEM(int, misc_camera_to_bullet_key, 0);
            CFG_ITEM(int, misc_camera_to_bullet_key_mode, 0);
        }
        namespace automatic {
            CFG_ITEM(bool, misc_automatic_auto_reload, false);
            CFG_ITEM(bool, misc_automatic_auto_heal, false);
            CFG_ITEM(bool, ore_assist_farm, false);
            CFG_ITEM(bool, tree_assist_farm, false);
        }
        namespace movement {
            CFG_ITEM(bool, disable_melee_slow_down, false);
            CFG_ITEM(bool, exploits_speedhack, false);
            CFG_ITEM(bool, exploits_walk_through_trees, false);
            CFG_ITEM(bool, exploits_walk_through_players, false);
            CFG_ITEM(bool, exploits_walk_on_water, false);
            CFG_ITEM(bool, exploits_omni_sprint, false);
            CFG_ITEM(bool, misc_infinite_jump, false);
            CFG_ITEM(bool, misc_suicide, false);
            CFG_ITEM(int, misc_suicide_key, 0);
            CFG_ITEM(bool, misc_climb_assist, false);
            CFG_ITEM(bool, misc_climb_assist_bypass, false);
            CFG_ITEM(bool, misc_always_sprint, false);
        }
        namespace exploits {
            CFG_ITEM(bool, exploits_admin_flag, false);
            CFG_ITEM(bool, exploits_anti_fly_hack_kick, false);
            CFG_ITEM(bool, exploits_extended_melee, false);
            CFG_ITEM(bool, exploits_name_spoofer, false);
            CFG_ITEM(bool, misc_modify_can_attack, false);
            CFG_ITEM(bool, exploits_unlock_aim_on_jugger_set, false);
            CFG_ITEM(bool, exploits_can_attack_in_vehicles, false);

            CFG_ITEM(bool, exploits_traps_exploit, false);
            CFG_ITEM(int, exploits_traps_exploit_key, 0);
            CFG_ITEM(int, exploits_traps_exploit_keymode, 0);
            CFG_ITEM(float, exploits_traps_exploit_amount, 0.5f);

            CFG_ITEM(bool, exploits_fake_lag, false);
            CFG_ITEM(float, exploits_fake_lag_cooldown, 0.5f);
            CFG_ITEM(float, exploits_fake_lag_duration, 0.2f);
            CFG_ITEM(float, exploits_fake_lag_min, 0.1f);
            CFG_ITEM(float, exploits_fake_lag_max, 0.3f);
        }
    }

    namespace visuals {
        namespace worlds {
            CFG_ITEM(bool, visuals_time_change_time, false);
            CFG_ITEM(float, visuals_time_change_time_value, 12.0f);

            CFG_ITEM(bool, sun_size_override, false);
            CFG_ITEM(float, sun_size, 1.0f);
            CFG_ITEM(bool, sun_brightness_override, false);
            CFG_ITEM(float, sun_brightness, 1.0f);

            CFG_ITEM(bool, moon_size_override, false);
            CFG_ITEM(float, moon_size, 1.0f);
            CFG_ITEM(bool, moon_brightness_override, false);
            CFG_ITEM(float, moon_brightness, 1.0f);
            CFG_ITEM(bool, moon_phase_override, false);
            CFG_ITEM(float, moon_phase, 0.0f);

            CFG_ITEM(bool, atmosphere_contrast, false);
            CFG_ITEM(float, atmosphere_contrast_value, 1.0f);

            CFG_ITEM(bool, visuals_sky_bright_night, false);
            CFG_ITEM(float, visuals_sky_bright_night_ambient_multiplier, 1.0f);
            CFG_ITEM(float, visuals_sky_bright_night_reflection_multiplier, 1.0f);
            CFG_ITEM(bool, visuals_sky_bright_cave, false);

            CFG_ITEM(bool, night_stars, false);
            CFG_ITEM(float, night_stars_size, 1.0f);
            CFG_ITEM(float, night_stars_brightness, 1.0f);
            CFG_ITEM(int, night_stars_position, 0); // Static / Rotating

            CFG_ITEM(bool, moon_halo, false);
            CFG_ITEM(float, moon_halo_size, 1.0f);
            CFG_ITEM(float, moon_halo_brightness, 1.0f);
            CFG_ITEM(int, moon_position, 0); // OppositeToSun / Realistic

            CFG_ITEM(bool, custom_clouds, false);
            CFG_ITEM(float, cloud_size, 1.0f);
            CFG_ITEM(float, cloud_opacity, 1.0f);
            CFG_ITEM(float, cloud_coverage, 1.0f);
            CFG_ITEM(float, cloud_sharpness, 1.0f);
            CFG_ITEM(float, cloud_coloring, 1.0f);
            CFG_ITEM(float, cloud_attenuation, 1.0f);
            CFG_ITEM(float, cloud_saturation, 1.0f);
            CFG_ITEM(float, cloud_scattering, 1.0f);
            CFG_ITEM(float, cloud_brightness, 1.0f);

            CFG_ITEM(bool, custom_latitude, false);
            CFG_ITEM(float, custom_latitude_value, 0.0f);
            CFG_ITEM(bool, custom_longitude, false);
            CFG_ITEM(float, custom_longitude_value, 0.0f);

            CFG_ITEM(bool, rayleigh_multiplier, false);
            CFG_ITEM(float, rayleigh_multiplier_value, 1.0f);

            CFG_ITEM(bool, sky_light_intensity, false);
            CFG_ITEM(float, sky_day_light_intensity_value, 1.0f);
            CFG_ITEM(float, sky_night_light_intensity_value, 1.0f);

            CFG_ITEM(bool, sky_shadow_strength, false);
            CFG_ITEM(float, sky_day_shadow_strength_value, 1.0f);
            CFG_ITEM(float, sky_night_shadow_strength_value, 1.0f);

            CFG_ITEM(bool, sky_color_changer, false);
            CFG_ITEM(ImColor, sky_color_changer_color, ImColor(255, 255, 255, 255));

            CFG_ITEM(bool, cloud_color_changer, false);
            CFG_ITEM(ImColor, cloud_color_changer_color, ImColor(255, 255, 255, 255));

            CFG_ITEM(bool, fog_color_changer, false);
            CFG_ITEM(ImColor, fog_color_changer_color, ImColor(255, 255, 255, 255));

            CFG_ITEM(bool, world_color_changer, false);
            CFG_ITEM(ImColor, world_color_changer_color, ImColor(255, 255, 255, 255));
        }
        namespace weather {
            namespace removals {
                CFG_ITEM(bool, visuals_weather_no_clouds, false);
                CFG_ITEM(bool, visuals_weather_no_thunder, false);
                CFG_ITEM(bool, visuals_weather_no_wind, false);
                CFG_ITEM(bool, visuals_weather_rainbows, false);
                CFG_ITEM(bool, visuals_weather_no_fog, false);
                CFG_ITEM(bool, visuals_weather_no_rain, false);
                CFG_ITEM(bool, visuals_weather_no_sun, false);
            }
            CFG_ITEM(bool, modify_fog, false);
            CFG_ITEM(float, modify_fog_value, 0.0f);
            CFG_ITEM(bool, modify_snow, false);
            CFG_ITEM(float, modify_snow_value, 0.0f);
            CFG_ITEM(bool, modify_rain, false);
            CFG_ITEM(float, modify_rain_value, 0.0f);
        }
        namespace layers {
            CFG_ITEM(bool, layers_enable, false);
            CFG_ITEM(int, layers_bind, 0);
            CFG_ITEM(int, layers_mode, 0);
            CFG_ITEM(bool, layers_remove_water, false);
            CFG_ITEM(bool, layers_remove_ragdolls, false);
            CFG_ITEM(bool, layers_remove_buildings, false);
            CFG_ITEM(bool, layers_remove_terrain, false);
            CFG_ITEM(bool, layers_remove_cargoship, false);
            CFG_ITEM(bool, layers_remove_trees, false);
        }
        namespace interfaces {
            CFG_ITEM(float, anti_fly_hack_kick_indi_X, 10.f);
            CFG_ITEM(float, anti_fly_hack_kick_indi_Y, 300.f);
            CFG_ITEM(float, traps_indicator_X, 10.f);
            CFG_ITEM(float, traps_indicator_Y, 360.f);
        }
    }

    namespace menu {
        CFG_ITEM(ImColor, accent_color, ImColor(220, 220, 220, 255));

        CFG_ITEM(bool, hover_highlight, true);
        CFG_ITEM(bool, shadow_enabled, false);
        CFG_ITEM(float, shadow_size, 30.f);
        CFG_ITEM(float, shadow_alpha, 0.3f);
        CFG_ITEM(int, dpi_scale_idx, 0);    
        CFG_ITEM(bool, lock_layout, false);

        CFG_ITEM(std::vector<int>, overlay, (std::vector<int>{1, 1, 0, 0}));
        CFG_ITEM(int, menu_key, 45);
        CFG_ITEM(int, unload_key, VK_END);
        CFG_ITEM(bool, untrusted_enabled, false);

        CFG_ITEM(float, watermark_x, 10.f);
        CFG_ITEM(float, watermark_y, 10.f);
        CFG_ITEM(float, keybinds_x, 10.f);
        CFG_ITEM(float, keybinds_y, 100.f);
        CFG_ITEM(float, reload_x, 10.f);
        CFG_ITEM(float, reload_y, 200.f);
    }

    CFG_ITEM(bool, exploits_fast_loot, false);
}
