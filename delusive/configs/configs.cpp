#include "configs.h"
#include <sstream>
#include <algorithm>
#include "../vcruntime/crypto/base64.h"
namespace {
    std::wstring utf8_to_wstring(const std::string& str) {
        if (str.empty()) return {};
        int n = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
        std::wstring r(n, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &r[0], n);
        return r;
    }

    std::string wstring_to_utf8(const std::wstring& wstr) {
        if (wstr.empty()) return {};
        int n = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(),
            nullptr, 0, nullptr, nullptr);
        std::string r(n, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(),
            &r[0], n, nullptr, nullptr);
        return r;
    }

    const wchar_t* REG_PATH = L"Software\\Microsoft\\Windows\\CurrentVersion\\Config";

    static void ensure_path(const std::string& path) {
        HRESULT hr = SHCreateDirectoryExA(nullptr, path.c_str(), nullptr);
        if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS)
            && hr != HRESULT_FROM_WIN32(ERROR_FILE_EXISTS)) {
            char msg[256];
            sprintf_s(msg, "ensure_path failed for '%s', HRESULT: 0x%08X", path.c_str(), hr);
            OutputDebugStringA(msg);
        }
    }

    void write_registry_string(const std::string& name, const std::string& value) {
        HKEY hKey;
        if (RegCreateKeyExW(HKEY_CURRENT_USER, REG_PATH, 0, nullptr, 0,
            KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
            auto wn = utf8_to_wstring(name);
            auto wv = utf8_to_wstring(value);
            RegSetValueExW(hKey, wn.c_str(), 0, REG_SZ,
                (BYTE*)wv.c_str(), (DWORD)((wv.size() + 1) * sizeof(wchar_t)));
            RegCloseKey(hKey);
        }
    }

    std::string read_registry_string(const std::string& name, const std::string& def) {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_PATH, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            wchar_t buf[512];
            DWORD sz = sizeof(buf);
            auto wn = utf8_to_wstring(name);
            if (RegQueryValueExW(hKey, wn.c_str(), nullptr, nullptr,
                (BYTE*)buf, &sz) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return wstring_to_utf8(buf);
            }
            RegCloseKey(hKey);
        }
        return def;
    }

    void write_registry_int(const std::string& name, int value) {
        HKEY hKey;
        if (RegCreateKeyExW(HKEY_CURRENT_USER, REG_PATH, 0, nullptr, 0,
            KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
            auto wn = utf8_to_wstring(name);
            RegSetValueExW(hKey, wn.c_str(), 0, REG_DWORD,
                (BYTE*)&value, sizeof(DWORD));
            RegCloseKey(hKey);
        }
    }

    int read_registry_int(const std::string& name, int def) {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_PATH, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            DWORD v;
            DWORD sz = sizeof(DWORD);
            auto wn = utf8_to_wstring(name);
            if (RegQueryValueExW(hKey, wn.c_str(), nullptr, nullptr,
                (BYTE*)&v, &sz) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return (int)v;
            }
            RegCloseKey(hKey);
        }
        return def;
    }

    void write_registry_bool(const std::string& name, bool value) {
        write_registry_int(name, value ? 1 : 0);
    }

    bool read_registry_bool(const std::string& name, bool def) {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_PATH, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            DWORD v;
            DWORD sz = sizeof(DWORD);
            auto wn = utf8_to_wstring(name);
            if (RegQueryValueExW(hKey, wn.c_str(), nullptr, nullptr,
                (BYTE*)&v, &sz) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return v != 0;
            }
            RegCloseKey(hKey);
        }
        return def;
    }

    void write_registry_float(const std::string& name, float value) {
        HKEY hKey;
        if (RegCreateKeyExW(HKEY_CURRENT_USER, REG_PATH, 0, nullptr, 0,
            KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
            auto wn = utf8_to_wstring(name);
            RegSetValueExW(hKey, wn.c_str(), 0, REG_BINARY,
                (BYTE*)&value, sizeof(float));
            RegCloseKey(hKey);
        }
    }

    float read_registry_float(const std::string& name, float def) {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_PATH, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            float v;
            DWORD sz = sizeof(float);
            auto wn = utf8_to_wstring(name);
            if (RegQueryValueExW(hKey, wn.c_str(), nullptr, nullptr,
                (BYTE*)&v, &sz) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return v;
            }
            RegCloseKey(hKey);
        }
        return def;
    }

    static constexpr uint8_t s_cipher_key[] = {
        0xD3, 0x1A, 0x7F, 0xC4, 0x89, 0x2E, 0x56, 0xB0,
        0x4F, 0x93, 0xE7, 0x1C, 0x68, 0xA5, 0x3D, 0xF2,
        0x07, 0xBB, 0x44, 0x9E, 0x61, 0xD8, 0x2A, 0x75,
        0xFC, 0x18, 0x8D, 0x50, 0xC3, 0x6B, 0xA9, 0x34
    };

    std::string encrypt_blob(const std::string& plain) {
        std::string buf = plain;
        constexpr size_t klen = sizeof(s_cipher_key);
        uint8_t rolling = 0xA7;
        for (size_t i = 0; i < buf.size(); ++i) {
            const uint8_t b = static_cast<uint8_t>(buf[i]);
            const uint8_t k = s_cipher_key[i % klen]
                ^ rolling
                ^ static_cast<uint8_t>(i & 0xFF);
            buf[i] = static_cast<char>(b ^ k);
            rolling = static_cast<uint8_t>(b ^ static_cast<uint8_t>((i * 0x1D) & 0xFF));
        }
        return base64::encode(buf);
    }

    std::string decrypt_blob(const std::string& b64) {
        std::string buf = base64::decode(b64);
        if (buf.empty()) return {};
        constexpr size_t klen = sizeof(s_cipher_key);
        uint8_t rolling = 0xA7;
        for (size_t i = 0; i < buf.size(); ++i) {
            const uint8_t b = static_cast<uint8_t>(buf[i]);
            const uint8_t k = s_cipher_key[i % klen]
                ^ rolling
                ^ static_cast<uint8_t>(i & 0xFF);
            const uint8_t plain_b = b ^ k;
            buf[i] = static_cast<char>(plain_b);
            rolling = static_cast<uint8_t>(plain_b ^ static_cast<uint8_t>((i * 0x1D) & 0xFF));
        }
        return buf;
    }

    uint32_t fnv1a(const std::string& data) {
        uint32_t h = 2166136261u;
        for (uint8_t c : data)
            h = (h ^ c) * 16777619u;
        return h;
    }

}

rconfig::rconfig(std::string file_path) : path(std::move(file_path)) {}

void rconfig::read() {
    std::wstring wpath = utf8_to_wstring(path);
    HANDLE file = CreateFileW(wpath.c_str(), GENERIC_READ, FILE_SHARE_READ,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;

    DWORD file_size = GetFileSize(file, nullptr);
    if (file_size == INVALID_FILE_SIZE || file_size == 0) {
        CloseHandle(file);
        return;
    }

    raw_cfg.resize(file_size);
    DWORD bytes_read = 0;
    if (!ReadFile(file, raw_cfg.data(), file_size, &bytes_read, nullptr)) {
        CloseHandle(file);
        return;
    }
    CloseHandle(file);
    raw_cfg.resize(bytes_read);

    last_hash = fnv1a(raw_cfg);

    auto lines = rconfig_detail::split(raw_cfg, "\n");
    for (auto& l : lines)
        if (!l.empty() && l.back() == '\r') l.pop_back();

    bool is_encrypted = false;
    for (const auto& line : lines) {
        if (line.empty()) continue;
        if (line[0] == '#') {
            if (line.find("What u doing here") != std::string::npos)
                is_encrypted = true;
            continue;
        }
        break;
    }

    records.clear();

    if (is_encrypted) {
        std::string blob;
        for (const auto& line : lines) {
            if (!line.empty() && line[0] != '#') {
                blob = line;
                break;
            }
        }

        if (blob.empty()) { m_dirty = false; return; }

        const std::string plain = decrypt_blob(blob);
        if (plain.empty()) { m_dirty = false; return; }

        auto plain_lines = rconfig_detail::split(plain, "\n");
        for (auto& line : plain_lines) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;

            auto pos = line.find('=');
            if (pos == std::string::npos) continue;

            auto key_str = line.substr(0, pos);
            auto value = line.substr(pos + 1);

            char* end;
            uint32_t hash = std::strtoul(key_str.c_str(), &end, 10);
            if (end != key_str.c_str() && *end == '\0')
                set_record(hash, value);
        }
    }
    else {
        for (const auto& line : lines) {
            if (line.empty() || line[0] == '#') continue;

            auto pos = line.find('=');
            if (pos == std::string::npos) continue;

            auto key_str = line.substr(0, pos);
            auto value = line.substr(pos + 1);

            char* end;
            uint32_t hash = std::strtoul(key_str.c_str(), &end, 10);
            if (end != key_str.c_str() && *end == '\0')
                set_record(hash, value);
        }

        m_dirty = true;
        return;
    }

    m_dirty = false;
}

void rconfig::write() {
    {
        std::string dir = path.substr(0, path.find_last_of("/\\") + 1);

        if (!dir.empty() && dir != path) {
            std::string dir_clean = dir;
            if (!dir_clean.empty() && (dir_clean.back() == '\\' || dir_clean.back() == '/'))
                dir_clean.pop_back();

            SHCreateDirectoryExA(nullptr, dir_clean.c_str(), nullptr);
        }
    }

    std::wstring wpath = utf8_to_wstring(path);

    if (!wpath.empty() && (wpath.back() == L'\\' || wpath.back() == L'/')) {
        OutputDebugStringA("rconfig::write() — path ends with separator, aborting");
        return;
    }

    HANDLE file = CreateFileW(wpath.c_str(), GENERIC_WRITE, 0,
        nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    std::string plain;
    plain.reserve(records.size() * 32);
    for (const auto& [hash, value] : records) {
        plain += std::to_string(hash);
        plain += '=';
        plain += value;
        plain += '\n';
    }

    const std::string blob = encrypt_blob(plain);
    const std::string content = "# Delusive Config File - What u doing here??\n"
        + blob + "\n";

    DWORD written = 0;
    WriteFile(file, content.c_str(), static_cast<DWORD>(content.size()), &written, nullptr);
    CloseHandle(file);

    last_hash = fnv1a(content);
    m_dirty = false;
}
bool rconfig::is_stale() {
    std::wstring wpath = utf8_to_wstring(path);
    HANDLE file = CreateFileW(wpath.c_str(), GENERIC_READ, FILE_SHARE_READ,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;

    DWORD file_size = GetFileSize(file, nullptr);
    if (file_size == INVALID_FILE_SIZE) { CloseHandle(file); return false; }

    std::string cur;
    cur.resize(file_size);
    DWORD bytes_read = 0;
    const bool ok = ReadFile(file, cur.data(), file_size, &bytes_read, nullptr);
    CloseHandle(file);
    if (!ok) return false;
    cur.resize(bytes_read);

    return fnv1a(cur) != last_hash;
}

std::string rconfig::export_base64() const {
    std::string result = "# Delusive Config Export\n";
    for (const auto& [hash, value] : records)
        result += std::to_string(hash) + "=" + value + "\n";
    return base64::encode(result);
}

bool rconfig::import_base64(const std::string& b64) {
    const auto decoded = base64::decode(b64);
    if (decoded.empty()) return false;

    records.clear();
    auto lines = rconfig_detail::split(decoded, "\n");
    for (const auto& line : lines) {
        if (line.empty() || line[0] == '#') continue;

        auto pos = line.find('=');
        if (pos == std::string::npos) continue;

        auto key_str = line.substr(0, pos);
        auto value = line.substr(pos + 1);

        char* end;
        uint32_t hash = std::strtoul(key_str.c_str(), &end, 10);
        if (end != key_str.c_str() && *end == '\0')
            set_record(hash, value);
    }
    m_dirty = true;
    return true;
}

void rconfig::write_meta(const std::string& author,
    const std::string& created,
    const std::string& discord_id) {
    meta.author = author;
    meta.created = created;
    meta.discord_user_id = discord_id;

    set_record(CFG_HASH("__meta_author"), author);
    set_record(CFG_HASH("__meta_created"), created);
    set_record(CFG_HASH("__meta_discord_user_id"), discord_id);
    set_record(CFG_HASH("__meta_version"), meta.version);
}

void rconfig::read_meta() {
    for (const auto& record : records) {
        const uint32_t    hash = record.first;
        const std::string& val = record.second;

        if (hash == CFG_HASH("__meta_author"))             meta.author = val;
        else if (hash == CFG_HASH("__meta_created"))            meta.created = val;
        else if (hash == CFG_HASH("__meta_version"))            meta.version = val;
        else if (hash == CFG_HASH("__meta_description"))        meta.description = val;
        else if (hash == CFG_HASH("__meta_discord_user_id"))    meta.discord_user_id = val;
        else if (hash == CFG_HASH("__meta_config_id"))          meta.config_id = val;
        else if (hash == CFG_HASH("__meta_last_sync_timestamp")) {
            char* end;
            uint64_t ts = std::strtoull(val.c_str(), &end, 10);
            if (end != val.c_str()) meta.last_sync_timestamp = ts;
        }
        else if (hash == CFG_HASH("__meta_is_cloud_synced")) {
            meta.is_cloud_synced = (val == "true" || val == "1");
        }
    }
}

std::string rconfig::get_record(uint32_t hash) const {
    for (const auto& record : records)
        if (record.first == hash) return record.second;
    return {};
}

void rconfig::set_record(uint32_t hash, const std::string& value) {
    for (auto& record : records) {
        if (record.first == hash) {
            record.second = value;
            m_dirty = true;
            return;
        }
    }
    records.emplace_back(hash, value);
    m_dirty = true;
}

uint32_t rconfig::get_config_hash() const {
    uint32_t hash = 2166136261u;
    for (const auto& [key, value] : records)
        for (char c : value)
            hash = (hash ^ static_cast<uint8_t>(c)) * 16777619u;
    return hash;
}

namespace config {
    namespace manager {

        void init() {
            config::register_all_items(); 

            std::string base = get_base_path();
            SHCreateDirectoryExA(nullptr, base.c_str(), nullptr);

            load_settings();
            refresh_list(); 

            bool has_default = false;
            for (const auto& cfg : config_list)
                if (cfg == "Default") { has_default = true; break; }

            if (config_list.empty() || !has_default)
                save("Default");

            std::string default_cfg = config::globals::settings_default_config;
            if (default_cfg.empty() || !config_exists(default_cfg))
                default_cfg = "Default";

            load(default_cfg);
        }

        void save(const std::string& name) {
            ensure_path(get_base_path());

            const std::string  full_path = get_base_path() + name + get_ext();
            const std::wstring wfull_path = utf8_to_wstring(full_path);
            const bool is_new_config = (GetFileAttributesW(wfull_path.c_str()) == INVALID_FILE_ATTRIBUTES);

            config.set_path(full_path);
            config.clear_records();

            if (is_new_config) {
                config.meta = config_meta{};
            }
            else if (config.meta.created.empty()) {
                config.meta = get_meta(name);
            }

            if (config.meta.created.empty()) {
                const std::string dc_name = (name == "Default")
                    ? "Admin"
                    : (g_discord_username.empty() ? _("Unknown User") : g_discord_username);
                const std::string dc_id = (name == "Default")
                    ? "0"
                    : (g_discord_id.empty() ? _("0") : g_discord_id);

                SYSTEMTIME st;
                GetLocalTime(&st);
                char tbuf[64], dbuf[64];
                GetTimeFormatA(LOCALE_USER_DEFAULT, 0, &st, nullptr, tbuf, sizeof(tbuf));
                GetDateFormatA(LOCALE_USER_DEFAULT, 0, &st, nullptr, dbuf, sizeof(dbuf));

                config.write_meta(dc_name,
                    std::string(dbuf) + _(" ") + std::string(tbuf),
                    dc_id);
            }

            for (auto& item : get_items())
                item->save();

            config.write();
            current_config_name = name;
            refresh_list();
        }
        void load(const std::string& name) {
            const std::string  full_path = std::string(get_base_path()) + name + get_ext();
            const std::wstring wfull_path = utf8_to_wstring(full_path);

            if (GetFileAttributesW(wfull_path.c_str()) == INVALID_FILE_ATTRIBUTES) return;

            config.set_path(full_path);
            config.read();
            config.read_meta();

            for (auto& item : get_items())
                item->read();

            current_config_name = name;
        }

        void refresh_list() {
            config_list.clear();
            ensure_path(std::string(get_base_path()));

            const std::string  search_path = std::string(get_base_path()) + _("*") + get_ext();
            const std::wstring wsearch_path = utf8_to_wstring(search_path);

            WIN32_FIND_DATAW find_data;
            HANDLE find_handle = FindFirstFileW(wsearch_path.c_str(), &find_data);
            if (find_handle == INVALID_HANDLE_VALUE) return;

            do {
                if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

                std::string filename = wstring_to_utf8(find_data.cFileName);
                const size_t ext_pos = filename.find_last_of('.');
                if (ext_pos == std::string::npos) continue;

                std::string cfg_name = filename.substr(0, ext_pos);
                if (!cfg_name.empty())
                    config_list.push_back(std::move(cfg_name));

            } while (FindNextFileW(find_handle, &find_data));

            FindClose(find_handle);

            if (!config_list.empty()) {
                std::sort(config_list.begin(), config_list.end(),
                    [](const std::string& a, const std::string& b) {
                        if (a == "Default") return true;
                        if (b == "Default") return false;
                        return a < b;
                    });
            }
        }

        void tick_autosave(float dt) {
            if (!autosave_enabled) {
                autosave_timer = 0.f;
                return;
            }
            if (!config.is_dirty()) {
                autosave_timer = 0.f;
                return;
            }
            autosave_timer += dt;
            if (autosave_timer >= AUTOSAVE_INTERVAL) {
                autosave_timer = 0.f;
                autosave();
            }
        }

        bool config_exists(const std::string& name) {
            const std::string  full_path = std::string(get_base_path()) + name + get_ext();
            const std::wstring wfull_path = utf8_to_wstring(full_path);
            return GetFileAttributesW(wfull_path.c_str()) != INVALID_FILE_ATTRIBUTES;
        }

        bool is_protected(const std::string& name) {
            return name == "Default";
        }

        void save_settings() {
            write_registry_bool("settings_first_time", config::globals::settings_first_time);
            write_registry_string("settings_default_config", config::globals::settings_default_config);

            write_registry_bool("menu_hover_highlight", config::menu::hover_highlight.value);
            write_registry_bool("menu_shadow_enabled", config::menu::shadow_enabled.value);
            write_registry_float("menu_shadow_size", config::menu::shadow_size.value);
            write_registry_float("menu_shadow_alpha", config::menu::shadow_alpha.value);
            write_registry_int("menu_dpi_scale_idx", config::menu::dpi_scale_idx.value);
            write_registry_bool("menu_lock_layout", config::menu::lock_layout.value);
        }

        void load_settings() {
            config::globals::settings_first_time = read_registry_bool("settings_first_time", true);
            config::globals::settings_default_config = read_registry_string("settings_default_config", "Default");

            config::menu::hover_highlight.value = read_registry_bool("menu_hover_highlight", true);
            config::menu::shadow_enabled.value = read_registry_bool("menu_shadow_enabled", false);
            config::menu::shadow_size.value = read_registry_float("menu_shadow_size", 30.f);
            config::menu::shadow_alpha.value = read_registry_float("menu_shadow_alpha", 0.3f);
            config::menu::dpi_scale_idx.value = read_registry_int("menu_dpi_scale_idx", 0);
            config::menu::lock_layout.value = read_registry_bool("menu_lock_layout", false);
        }

        void delete_config(const std::string& name) {
            if (is_protected(name)) return;

            const std::string  full_path = std::string(get_base_path()) + name + get_ext();
            const std::wstring wfull_path = utf8_to_wstring(full_path);

            if (DeleteFileW(wfull_path.c_str())) {
                refresh_list();
            }
            else {
                char msg[128];
                sprintf_s(msg, "DeleteFileW failed, error: %lu", GetLastError());
                OutputDebugStringA(msg);
            }
        }

        config_meta get_meta(const std::string& name) {
            const std::string  full_path = std::string(get_base_path()) + name + get_ext();
            const std::wstring wfull_path = utf8_to_wstring(full_path);

            if (GetFileAttributesW(wfull_path.c_str()) == INVALID_FILE_ATTRIBUTES)
                return config_meta{};

            rconfig temp(full_path);
            temp.read();
            temp.read_meta();
            return temp.meta;
        }

        std::string export_config_full(const std::string& name) {
            const std::string  full_path = std::string(get_base_path()) + name + get_ext();
            const std::wstring wfull_path = utf8_to_wstring(full_path);

            if (GetFileAttributesW(wfull_path.c_str()) == INVALID_FILE_ATTRIBUTES)
                return {};

            rconfig temp(full_path);
            temp.read();
            temp.read_meta();

            const std::string base64_data = temp.export_base64();
            return _("DLCFG|") + name + _("|")
                + temp.meta.author + _("|")
                + temp.meta.created + _("|")
                + base64_data;
        }

        bool import_config_full(const std::string& encoded) {
            auto parts = rconfig_detail::split(encoded, "|");
            if (parts.size() < 5 || parts[0] != "DLCFG") return false;

            const std::string& name = parts[1];
            const std::string& author = parts[2];
            const std::string& created = parts[3];
            const std::string& base64_data = parts[4];

            const std::string full_path = std::string(get_base_path()) + name + get_ext();
            rconfig temp(full_path);
            if (!temp.import_base64(base64_data)) return false;

            temp.write_meta(author, created, "0");
            temp.write();

            refresh_list();
            return true;
        }

    } // namespace manager
} // namespace config