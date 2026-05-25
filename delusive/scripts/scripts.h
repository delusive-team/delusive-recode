#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <Windows.h>
#include <d3d11.h>
#include <thread>
#include <mmsystem.h>

#include "../thirdparty/imgui/imgui.h"
#include "../../framework/settings/functions.h"
#include "../../framework/elements/notifications.h"
#include "../vcruntime/logger/logger.h"
#pragma comment(lib, "winmm.lib")

extern bool LoadTextureFromFile(const char* filename, ID3D11Device* d3dDevice,
    ID3D11ShaderResourceView** out_srv);
extern ID3D11Device* g_pd3dDevice;
extern std::string    g_discord_username;

extern "C" {
#include "Api/include/lua.h"
#include "Api/include/lualib.h"
#include "Api/include/lauxlib.h"
#include "Api/include/luaconf.h"
#undef lua_objlen
    LUA_API size_t(lua_objlen)(lua_State* L, int idx);
}
#pragma comment(lib, "lua51.lib")

namespace scripts {

    inline const std::string BASE_PATH = "C:\\Delusive\\Rust\\Scripts\\";
    inline bool lua_security_enabled = true;
    constexpr const char* SCRIPTS_EXT = ".lua";
    inline int lua_active_subtab = 0;

    struct script_entry {
        std::string name, path, content, size_str, last_error;
        bool        loaded = false;
        lua_State* L = nullptr;
        bool        has_draw = false;
        bool        has_init = false;

        std::vector<int>                       owned_tabs;
        std::vector<ID3D11ShaderResourceView*> loaded_textures;
    };

    namespace manager {
        inline std::vector<script_entry> script_list;
        static void unload(script_entry& sc);
    }

    namespace lua_api {

        inline float current_panel_width = 0.f;
        inline int   current_panel_col = 0;  

        inline std::unordered_map<lua_State*, int> panel_depth;

        static bool safe_pcall(lua_State* L, int nargs, int nret,
            std::string& out_error)
        {
            // -1 за саму функцию, которую pcall тоже снимает со стека
            int top_before = lua_gettop(L) - nargs - 1;

            int rc = lua_pcall(L, nargs, nret, 0);
            if (rc != 0) {
                if (lua_gettop(L) > top_before && lua_isstring(L, -1))
                    out_error = lua_tostring(L, -1);
                else
                    out_error = "unknown lua error (rc=" + std::to_string(rc) + ")";

                lua_settop(L, top_before); 
                return false;
            }
            return true;
        }

        static void push_error_notify(const std::string& script_name,
            const std::string& where,
            const std::string& msg)
        {
            static std::map<std::string, ULONGLONG> last_notify;
            ULONGLONG now = GetTickCount64();
            std::string key = script_name + "::" + where;

            auto it = last_notify.find(key);
            if (it != last_notify.end() && now - it->second < 1000ULL)
                return;
            last_notify[key] = now;

            std::string title = "[Lua] " + script_name + " :: " + where;

            // [!] FIX: Защита от краша при инициализации DLL.
            // Уведомление на экран пойдет только если ImGui уже начал рендерить кадры.
            if (ImGui::GetCurrentContext() && ImGui::GetFrameCount() > 0) {
                notifications::push(title.c_str(), msg.c_str(), notifications::ERROR_T, 5.f);
            }

            // В лог пишем всегда, это безопасно
            LOG_ERROR("[Lua] %s :: %s | %s", script_name.c_str(),
                where.c_str(), msg.c_str());
        }

        // ─────────────────────────────────────────────────────────────────────
        // Вспомогательная: читаем RGBA с позиции first_arg
        // ─────────────────────────────────────────────────────────────────────
        static ImU32 read_color(lua_State* L, int first_arg,
            int dr = 255, int dg = 255,
            int db = 255, int da = 255)
        {
            int r = (int)luaL_optinteger(L, first_arg, dr);
            int g = (int)luaL_optinteger(L, first_arg + 1, dg);
            int b = (int)luaL_optinteger(L, first_arg + 2, db);
            int a = (int)luaL_optinteger(L, first_arg + 3, da);
            return IM_COL32(r, g, b, a);
        }

        // ── renderer ─────────────────────────────────────────────────────────

        static int l_screen_size(lua_State* L) {
            ImVec2 sz = ImGui::GetIO().DisplaySize;
            lua_pushnumber(L, sz.x);
            lua_pushnumber(L, sz.y);
            return 2;
        }

        static int l_text(lua_State* L) {
            float x = (float)luaL_checknumber(L, 1);
            float y = (float)luaL_checknumber(L, 2);
            ImU32 col = read_color(L, 3);
            const char* t = luaL_checkstring(L, 7);
            ImGui::GetForegroundDrawList()->AddText({ x, y }, col, t);
            return 0;
        }

        static int l_text_size(lua_State* L) {
            ImVec2 sz = ImGui::CalcTextSize(luaL_checkstring(L, 1));
            lua_pushnumber(L, sz.x);
            lua_pushnumber(L, sz.y);
            return 2;
        }

        static int l_line(lua_State* L) {
            float x1 = (float)luaL_checknumber(L, 1), y1 = (float)luaL_checknumber(L, 2);
            float x2 = (float)luaL_checknumber(L, 3), y2 = (float)luaL_checknumber(L, 4);
            ImU32 col = read_color(L, 5);
            float t = (float)luaL_optnumber(L, 9, 1.0f);
            ImGui::GetForegroundDrawList()->AddLine({ x1,y1 }, { x2,y2 }, col, t);
            return 0;
        }

        static int l_rect(lua_State* L) {
            float x = (float)luaL_checknumber(L, 1), y = (float)luaL_checknumber(L, 2);
            float w = (float)luaL_checknumber(L, 3), h = (float)luaL_checknumber(L, 4);
            ImU32 col = read_color(L, 5);
            float rounding = (float)luaL_optnumber(L, 9, 0.0f);
            float thick = (float)luaL_optnumber(L, 10, 1.0f);
            ImGui::GetForegroundDrawList()->AddRect({ x,y }, { x + w,y + h }, col, rounding, 0, thick);
            return 0;
        }

        static int l_rect_filled(lua_State* L) {
            float x = (float)luaL_checknumber(L, 1), y = (float)luaL_checknumber(L, 2);
            float w = (float)luaL_checknumber(L, 3), h = (float)luaL_checknumber(L, 4);
            ImU32 col = read_color(L, 5);
            float rounding = (float)luaL_optnumber(L, 9, 0.0f);
            ImGui::GetForegroundDrawList()->AddRectFilled({ x,y }, { x + w,y + h }, col, rounding);
            return 0;
        }

        static int l_rect_gradient(lua_State* L) {
            float x = (float)luaL_checknumber(L, 1), y = (float)luaL_checknumber(L, 2);
            float w = (float)luaL_checknumber(L, 3), h = (float)luaL_checknumber(L, 4);
            ImU32 c_ul = IM_COL32((int)luaL_optinteger(L, 5, 255), (int)luaL_optinteger(L, 6, 255),
                (int)luaL_optinteger(L, 7, 255), (int)luaL_optinteger(L, 8, 255));
            ImU32 c_ur = IM_COL32((int)luaL_optinteger(L, 9, 255), (int)luaL_optinteger(L, 10, 255),
                (int)luaL_optinteger(L, 11, 255), (int)luaL_optinteger(L, 12, 255));
            ImU32 c_lr = IM_COL32((int)luaL_optinteger(L, 13, 255), (int)luaL_optinteger(L, 14, 255),
                (int)luaL_optinteger(L, 15, 255), (int)luaL_optinteger(L, 16, 255));
            ImU32 c_ll = IM_COL32((int)luaL_optinteger(L, 17, 255), (int)luaL_optinteger(L, 18, 255),
                (int)luaL_optinteger(L, 19, 255), (int)luaL_optinteger(L, 20, 255));
            ImGui::GetForegroundDrawList()->AddRectFilledMultiColor(
                { x,y }, { x + w,y + h }, c_ul, c_ur, c_lr, c_ll);
            return 0;
        }

        static int l_circle(lua_State* L) {
            float x = (float)luaL_checknumber(L, 1), y = (float)luaL_checknumber(L, 2);
            float r = (float)luaL_checknumber(L, 3);
            ImU32 col = read_color(L, 4);
            float thick = (float)luaL_optnumber(L, 8, 1.0f);
            int   segs = (int)luaL_optinteger(L, 9, 32);
            ImGui::GetForegroundDrawList()->AddCircle({ x,y }, r, col, segs, thick);
            return 0;
        }

        static int l_circle_filled(lua_State* L) {
            float x = (float)luaL_checknumber(L, 1), y = (float)luaL_checknumber(L, 2);
            float r = (float)luaL_checknumber(L, 3);
            ImU32 col = read_color(L, 4);
            int   segs = (int)luaL_optinteger(L, 8, 32);
            ImGui::GetForegroundDrawList()->AddCircleFilled({ x,y }, r, col, segs);
            return 0;
        }

        static int l_triangle(lua_State* L) {
            ImVec2 a = { (float)luaL_checknumber(L,1),(float)luaL_checknumber(L,2) };
            ImVec2 b = { (float)luaL_checknumber(L,3),(float)luaL_checknumber(L,4) };
            ImVec2 c = { (float)luaL_checknumber(L,5),(float)luaL_checknumber(L,6) };
            ImU32 col = read_color(L, 7);
            float t = (float)luaL_optnumber(L, 11, 1.0f);
            ImGui::GetForegroundDrawList()->AddTriangle(a, b, c, col, t);
            return 0;
        }

        static int l_triangle_filled(lua_State* L) {
            ImVec2 a = { (float)luaL_checknumber(L,1),(float)luaL_checknumber(L,2) };
            ImVec2 b = { (float)luaL_checknumber(L,3),(float)luaL_checknumber(L,4) };
            ImVec2 c = { (float)luaL_checknumber(L,5),(float)luaL_checknumber(L,6) };
            ImU32 col = read_color(L, 7);
            ImGui::GetForegroundDrawList()->AddTriangleFilled(a, b, c, col);
            return 0;
        }

        static int l_capsule(lua_State* L) {
            float x1 = (float)luaL_checknumber(L, 1), y1 = (float)luaL_checknumber(L, 2);
            float x2 = (float)luaL_checknumber(L, 3), y2 = (float)luaL_checknumber(L, 4);
            float t = (float)luaL_checknumber(L, 5);
            ImU32 col = read_color(L, 6);
            ImGui::GetForegroundDrawList()->AddLine({ x1,y1 }, { x2,y2 }, col, t);
            return 0;
        }

        static std::map<std::string, ID3D11ShaderResourceView*> texture_cache;

        static int l_image(lua_State* L) {
            if (!g_pd3dDevice) {
                LOG_ERROR("[Lua] l_image: g_pd3dDevice is null");
                return 0;
            }
            const char* path = luaL_checkstring(L, 1);
            float x = (float)luaL_checknumber(L, 2), y = (float)luaL_checknumber(L, 3);
            float w = (float)luaL_checknumber(L, 4), h = (float)luaL_checknumber(L, 5);

            ID3D11ShaderResourceView* tex = nullptr;
            std::string key(path);

            auto it = texture_cache.find(key);
            if (it != texture_cache.end()) {
                tex = it->second;
            }
            else if (LoadTextureFromFile(path, g_pd3dDevice, &tex) && tex) {
                texture_cache[key] = tex;
                for (auto& sc : scripts::manager::script_list) {
                    if (sc.L == L) { sc.loaded_textures.push_back(tex); break; }
                }
                LOG("[Lua] Loaded texture: %s", path);
            }
            else {
                LOG_ERROR("[Lua] l_image: failed to load texture '%s'", path);
            }

            if (tex)
                ImGui::GetBackgroundDrawList()->AddImage(
                    (ImTextureID)tex, { x,y }, { x + w,y + h });
            return 0;
        }

        static int l_get_menu_position(lua_State* L) {
            auto* window = ImGui::FindWindowByName("main");
            ImVec2 pos = window ? window->Pos : ImVec2(0.f, 0.f);
            float  width = window ? window->Size.x : 0.f;
            lua_pushnumber(L, pos.x);
            lua_pushnumber(L, pos.y);
            lua_pushnumber(L, width);
            return 3;
        }

        // ── input ─────────────────────────────────────────────────────────────

        static int l_mouse_pos(lua_State* L) {
            ImVec2 p = ImGui::GetMousePos();
            lua_pushnumber(L, p.x); lua_pushnumber(L, p.y);
            return 2;
        }
        static int l_mouse_delta(lua_State* L) {
            ImVec2 d = ImGui::GetIO().MouseDelta;
            lua_pushnumber(L, d.x); lua_pushnumber(L, d.y);
            return 2;
        }
        static int l_scroll_delta(lua_State* L) {
            lua_pushnumber(L, ImGui::GetIO().MouseWheel);
            return 1;
        }

        static bool is_mouse_key(int key) {
            return key == VK_LBUTTON || key == VK_RBUTTON ||
                key == VK_MBUTTON || key == VK_XBUTTON1 || key == VK_XBUTTON2;
        }

        static int l_is_key_down(lua_State* L) {
            int  key = (int)luaL_checkinteger(L, 1);
            bool blocked = ImGui::GetIO().WantCaptureKeyboard && !is_mouse_key(key);
            bool down = !blocked && (key > 0 && key < 256) &&
                (GetAsyncKeyState(key) & 0x8000) != 0;
            lua_pushboolean(L, down);
            return 1;
        }

        static int l_is_key_pressed(lua_State* L) {
            int  key = (int)luaL_checkinteger(L, 1);
            bool blocked = ImGui::GetIO().WantCaptureKeyboard && !is_mouse_key(key);
            bool pressed = !blocked && (key > 0 && key < 256) &&
                ImGui::IsKeyPressed(static_cast<ImGuiKey>(key), false);
            lua_pushboolean(L, pressed);
            return 1;
        }

        // ── globals ───────────────────────────────────────────────────────────

        static int l_realtime(lua_State* L) {
            lua_pushnumber(L, static_cast<double>(GetTickCount64()) / 1000.0);
            return 1;
        }
        static int l_delta_time(lua_State* L) {
            lua_pushnumber(L, ImGui::GetIO().DeltaTime);  return 1;
        }
        static int l_framerate(lua_State* L) {
            lua_pushnumber(L, ImGui::GetIO().Framerate);  return 1;
        }
        static int l_tick_count(lua_State* L) {
            lua_pushinteger(L, GetTickCount());           return 1;
        }
        static int l_username(lua_State* L) {
            lua_pushstring(L, g_discord_username.empty()
                ? "Connecting..." : g_discord_username.c_str());
            return 1;
        }

        // ── client.log / client.error → теперь используют LOG макросы ────────

        static int l_log(lua_State* L) {
            const char* msg = luaL_checkstring(L, 1);
            LOG("[Lua Script] %s", msg);
            return 0;
        }
        static int l_error(lua_State* L) {
            const char* msg = luaL_checkstring(L, 1);
            LOG_ERROR("[Lua Script] %s", msg);
            return 0;
        }

        // ── audio ─────────────────────────────────────────────────────────────

        static int l_play_radio(lua_State* L) {
            const char* url = luaL_checkstring(L, 1);
            LOG("[Lua] play_radio: %s", url);
            std::thread([url]() {
                PlaySoundA(NULL, NULL, 0);
                PlaySoundA(url, NULL, SND_ASYNC | SND_NODEFAULT);
                }).detach();
            return 0;
        }
        static int l_stop_radio(lua_State* L) {
            PlaySoundA(NULL, NULL, 0);
            return 0;
        }

        // ── ui helpers ────────────────────────────────────────────────────────

        static int l_is_menu_open(lua_State* L) {
            if (!var) {
                LOG_ERROR("[Lua] l_is_menu_open: var is null");
                lua_pushboolean(L, false);
                return 1;
            }
            lua_pushboolean(L, var->gui.menu_opened);
            return 1;
        }

        static int l_add_tab(lua_State* L) { lua_pushinteger(L, 0); return 1; }
        static int l_get_active_tab(lua_State* L) {
            lua_pushinteger(L, scripts::lua_active_subtab);
            return 1;
        }

        // ─────────────────────────────────────────────────────────────────────
        // FIX #4: begin_panel / end_panel с panel_depth трекингом.
        //
        // Старый код: begin_child всегда с (2, 1) — игнорировал текущую колонку
        // и current_panel_width.
        //
        // Новый код:
        //   - col_idx = 1 для левой колонки, 2 для правой (как в C++ коде)
        //   - panel_depth[L]++ / -- для детекции незакрытых панелей
        // ─────────────────────────────────────────────────────────────────────
        static int l_begin_panel(lua_State* L) {
            if (!gui) {
                LOG_ERROR("[Lua] l_begin_panel: gui is null");
                return 0;
            }
            const char* label = luaL_checkstring(L, 1);
            // current_panel_col: 0=left → y=1, 1=right → y=2 (соответствует C++)
            int col_idx = current_panel_col + 1;
            gui->begin_child(label, 2, col_idx);
            panel_depth[L]++;
            LOG("[Lua] begin_panel '%s' col=%d depth=%d",
                label, current_panel_col, panel_depth[L]);
            return 0;
        }

        static int l_end_panel(lua_State* L) {
            if (!gui) return 0;
            if (panel_depth.count(L) && panel_depth[L] > 0) {
                gui->end_child();
                panel_depth[L]--;
                LOG("[Lua] end_panel depth=%d", panel_depth[L]);
            }
            else {
                // Вызов end_panel без begin_panel — потенциальный краш
                LOG_ERROR("[Lua] end_panel called without matching begin_panel!");
            }
            return 0;
        }

        static int l_separator(lua_State* L) { ImGui::Separator(); return 0; }
        static int l_spacing(lua_State* L) {
            int n = (int)luaL_optinteger(L, 1, 1);
            for (int i = 0; i < n; i++) ImGui::Spacing();
            return 0;
        }
        static int l_same_line(lua_State* L) {
            if (gui) gui->sameline();
            return 0;
        }

        static int l_notify(lua_State* L) {
            const char* title = luaL_checkstring(L, 1);
            const char* message = luaL_checkstring(L, 2);
            int         type = (int)luaL_optinteger(L, 3, notifications::INFO);
            float       dur = (float)luaL_optnumber(L, 4, 3.5);
            type = ImClamp(type, 0, 3);
            notifications::push(title, message,
                static_cast<notifications::type_t>(type), dur);
            LOG("[Lua] notify: [%s] %s", title, message);
            return 0;
        }

        // ── widgets ───────────────────────────────────────────────────────────

        static int l_checkbox(lua_State* L) {
            if (!gui) {
                lua_pushboolean(L, false); lua_pushboolean(L, false);
                return 2;
            }
            const char* label = luaL_checkstring(L, 1);
            bool value = lua_toboolean(L, 2) != 0;
            bool changed = gui->checkbox(label, &value);
            lua_pushboolean(L, value);
            lua_pushboolean(L, changed);
            return 2;
        }

        static int l_slider_float(lua_State* L) {
            if (!gui) {
                lua_pushnumber(L, luaL_checknumber(L, 2));
                lua_pushboolean(L, false);
                return 2;
            }
            const char* label = luaL_checkstring(L, 1);
            float value = (float)luaL_checknumber(L, 2);
            float vmin = (float)luaL_checknumber(L, 3);
            float vmax = (float)luaL_checknumber(L, 4);
            const char* format = luaL_optstring(L, 5, "%.1f");
            bool changed = gui->slider_float(label, &value, vmin, vmax, false, format);
            lua_pushnumber(L, value);
            lua_pushboolean(L, changed);
            return 2;
        }

        // ─────────────────────────────────────────────────────────────────────
        // FIX #5: l_slider_int — был сырой ImGui::SliderInt.
        // Заменяем на gui->slider_int для правильного стиля и поведения.
        // ─────────────────────────────────────────────────────────────────────
        static int l_slider_int(lua_State* L) {
            if (!gui) {
                lua_pushinteger(L, luaL_checkinteger(L, 2));
                lua_pushboolean(L, false);
                return 2;
            }
            const char* label = luaL_checkstring(L, 1);
            int value = (int)luaL_checkinteger(L, 2);
            int vmin = (int)luaL_checkinteger(L, 3);
            int vmax = (int)luaL_checkinteger(L, 4);
            const char* format = luaL_optstring(L, 5, "%d");
            bool changed = gui->slider_int(label, &value, vmin, vmax, false, format);
            lua_pushinteger(L, value);
            lua_pushboolean(L, changed);
            return 2;
        }

        static int l_keybind(lua_State* L) {
            if (!gui) {
                lua_pushinteger(L, luaL_checkinteger(L, 2));
                lua_pushinteger(L, luaL_checkinteger(L, 3));
                return 2;
            }
            const char* label = luaL_checkstring(L, 1);
            int key = (int)luaL_checkinteger(L, 2);
            int mode = (int)luaL_checkinteger(L, 3);
            gui->keybind(label, &key, &mode);
            lua_pushinteger(L, key);
            lua_pushinteger(L, mode);
            return 2;
        }

        static int l_color_picker(lua_State* L) {
            if (!gui) {
                for (int i = 2; i <= 5; i++) lua_pushinteger(L, luaL_optinteger(L, i, 255));
                lua_pushboolean(L, false);
                return 5;
            }
            const char* label = luaL_checkstring(L, 1);
            float col[4] = {
                (float)luaL_checkinteger(L, 2) / 255.f,
                (float)luaL_checkinteger(L, 3) / 255.f,
                (float)luaL_checkinteger(L, 4) / 255.f,
                (float)luaL_optinteger(L,  5, 255) / 255.f
            };
            bool changed = gui->color_edit(label, col, true);
            for (int i = 0; i < 4; i++) lua_pushinteger(L, (int)(col[i] * 255.f));
            lua_pushboolean(L, changed);
            return 5;
        }

        static int l_combo(lua_State* L) {
            if (!gui) {
                lua_pushinteger(L, luaL_checkinteger(L, 2));
                return 1;
            }
            const char* label = luaL_checkstring(L, 1);
            int current = (int)luaL_checkinteger(L, 2) - 1;
            luaL_checktype(L, 3, LUA_TTABLE);
            int n = (int)lua_objlen(L, 3);

            std::vector<std::string> strings(n);
            std::vector<const char*> items(n);
            for (int i = 0; i < n; i++) {
                lua_rawgeti(L, 3, i + 1);
                strings[i] = lua_tostring(L, -1);
                items[i] = strings[i].c_str();
                lua_pop(L, 1);
            }
            gui->dropdown(label, &current, items.data(), n);
            lua_pushinteger(L, current + 1);
            return 1;
        }

        static int l_multi_combo(lua_State* L) {
            if (!gui) return 0;
            const char* label = luaL_checkstring(L, 1);

            luaL_checktype(L, 2, LUA_TTABLE);
            int vn = (int)lua_objlen(L, 2);
            std::vector<int> values(vn);
            for (int i = 0; i < vn; i++) {
                lua_rawgeti(L, 2, i + 1);
                values[i] = lua_toboolean(L, -1) ? 1 : 0;
                lua_pop(L, 1);
            }

            luaL_checktype(L, 3, LUA_TTABLE);
            int in_ = (int)lua_objlen(L, 3);
            std::vector<std::string> strings(in_);
            std::vector<const char*> items(in_);
            for (int i = 0; i < in_; i++) {
                lua_rawgeti(L, 3, i + 1);
                strings[i] = lua_tostring(L, -1);
                items[i] = strings[i].c_str();
                lua_pop(L, 1);
            }

            gui->multi_dropdown(label, values, items.data(), in_);
            for (int i = 0; i < vn; i++) {
                lua_pushboolean(L, values[i] != 0);
                lua_rawseti(L, 2, i + 1);
            }
            return 0;
        }

        static int l_button(lua_State* L) {
            if (!gui) { lua_pushboolean(L, false); return 1; }
            lua_pushboolean(L, gui->button(luaL_checkstring(L, 1), 1));
            return 1;
        }

        static int l_input_text(lua_State* L) {
            if (!gui) {
                lua_pushstring(L, luaL_optstring(L, 2, ""));
                lua_pushboolean(L, false);
                return 2;
            }
            const char* label = luaL_checkstring(L, 1);
            const char* init = luaL_optstring(L, 2, "");

            static std::map<std::string, std::string> buffers;
            std::string& stored = buffers[label];
            if (stored.empty() && init && init[0])
                stored = init;

            char local_buf[256] = {};
            strncpy_s(local_buf, stored.c_str(), sizeof(local_buf) - 1);

            bool changed = gui->text_field(label, local_buf, sizeof(local_buf));
            if (changed) stored = local_buf;

            lua_pushstring(L, local_buf);
            lua_pushboolean(L, changed);
            return 2;
        }

        static int l_input_int(lua_State* L) {
            const char* label = luaL_checkstring(L, 1);
            int value = (int)luaL_optinteger(L, 2, 0);
            bool changed = ImGui::InputInt(label, &value);
            lua_pushinteger(L, value);
            lua_pushboolean(L, changed);
            return 2;
        }

        static int l_ui_text(lua_State* L) {
            ImGui::TextUnformatted(luaL_checkstring(L, 1));
            return 0;
        }

        static int l_header(lua_State* L) {
            bool open = ImGui::CollapsingHeader(luaL_checkstring(L, 1));
            lua_pushboolean(L, open);
            return 1;
        }

        static int l_radio(lua_State* L) {
            const char* label = luaL_checkstring(L, 1);
            int active = (int)luaL_checkinteger(L, 2);
            int my_val = (int)luaL_checkinteger(L, 3);
            if (ImGui::RadioButton(label, active == my_val)) active = my_val;
            lua_pushinteger(L, active);
            return 1;
        }

        static int l_progress_bar(lua_State* L) {
            float       frac = (float)luaL_checknumber(L, 1);
            const char* text = luaL_optstring(L, 2, nullptr);
            float       w = (float)luaL_optnumber(L, 3, -1.0f);
            float       h = (float)luaL_optnumber(L, 4, 0.0f);
            ImGui::ProgressBar(frac, ImVec2(w, h), text);
            return 0;
        }

        static int l_tooltip(lua_State* L) {
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextUnformatted(luaL_checkstring(L, 1));
                ImGui::EndTooltip();
            }
            return 0;
        }

        static int l_set_clipboard(lua_State* L) {
            ImGui::SetClipboardText(luaL_checkstring(L, 1));
            return 0;
        }
        static int l_get_clipboard(lua_State* L) {
            const char* t = ImGui::GetClipboardText();
            lua_pushstring(L, t ? t : "");
            return 1;
        }

        static int l_save_cfg(lua_State* L) {
            const char* sname = luaL_checkstring(L, 1);
            const char* key = luaL_checkstring(L, 2);
            const char* val = luaL_checkstring(L, 3);
            std::string path = BASE_PATH + "cfg_" + sname + ".ini";
            WritePrivateProfileStringA("LuaConfig", key, val, path.c_str());
            LOG("[Lua] config.save: [%s] %s = %s", sname, key, val);
            return 0;
        }
        static int l_load_cfg(lua_State* L) {
            const char* sname = luaL_checkstring(L, 1);
            const char* key = luaL_checkstring(L, 2);
            const char* defval = luaL_optstring(L, 3, "");
            std::string path = BASE_PATH + "cfg_" + sname + ".ini";
            char buf[1024];
            GetPrivateProfileStringA("LuaConfig", key, defval, buf, sizeof(buf), path.c_str());
            lua_pushstring(L, buf);
            return 1;
        }

#define LUA_REG(tbl, name, fn) \
    lua_pushcfunction(L, fn);   \
    lua_setfield(L, -2, name);

        // ─────────────────────────────────────────────────────────────────────
        // apply_security — без изменений, только добавлены LOG
        // ─────────────────────────────────────────────────────────────────────

        static bool security_dostring(lua_State* L, const char* step, const char* code)
        {
            int top_before = lua_gettop(L);
            int rc = luaL_dostring(L, code);
            if (rc != 0) {
                const char* err = (lua_gettop(L) > top_before && lua_isstring(L, -1))
                    ? lua_tostring(L, -1) : "unknown error";
                LOG_ERROR("[Lua] apply_security step '%s' failed: %s", step, err);
                lua_settop(L, top_before);
                return false;
            }
            // гарантируем чистый стек — luaL_dostring с MULTRET может оставить значения
            lua_settop(L, top_before);
            return true;
        }

        static void apply_security(lua_State* L) {
            if (!scripts::lua_security_enabled) return;

            LOG("[Lua] Security: begin (stack=%d)", lua_gettop(L));

            LOG("[Lua] Security: step 1 - clear package.loaded");
            security_dostring(L, "clear_package_loaded",
                "if type(package) == 'table' and type(package.loaded) == 'table' then\n"
                "  local names = {'io','os','debug','ffi','package'}\n"
                "  for i = 1, #names do package.loaded[names[i]] = nil end\n"
                "end\n"
            );

            LOG("[Lua] Security: step 2 - null blocked globals");
            static const char* const k_blocked[] = {
                "io", "os", "debug", "ffi",
                "package", "load", "loadfile", "dofile", "loadstring",
                nullptr
            };
            for (int i = 0; k_blocked[i] != nullptr; ++i) {
                lua_pushnil(L);
                lua_setglobal(L, k_blocked[i]);
            }
            LOG("[Lua] Security: step 3 - block require (Lua function)");
            security_dostring(L, "block_require",
                "require = function(name)\n"
                "  local msg = 'Disable Lua Security to use require(\"' .. tostring(name or '?') .. '\")'\n"
                "  notify.push('Lua Security', msg, notify.WARNING, 6.0)\n"
                "  return nil\n" 
                "end\n"
            );

            LOG("[Lua] Security: done (stack=%d)", lua_gettop(L));
        }
        static void register_api(lua_State* L) {
            int rc = luaL_dostring(L,
                "package.path = package.path .. "
                "';C:\\\\Delusive\\\\Rust\\\\Scripts\\\\Libs\\\\?.lua'");
            if (rc != 0) {
                LOG_ERROR("[Lua] register_api: failed to set package.path");
                lua_pop(L, 1);
            }

            // renderer
            lua_newtable(L);
            LUA_REG(-1, "screen_size", l_screen_size);
            LUA_REG(-1, "text", l_text);
            LUA_REG(-1, "text_size", l_text_size);
            LUA_REG(-1, "line", l_line);
            LUA_REG(-1, "rect", l_rect);
            LUA_REG(-1, "rect_filled", l_rect_filled);
            LUA_REG(-1, "rect_gradient", l_rect_gradient);
            LUA_REG(-1, "circle", l_circle);
            LUA_REG(-1, "circle_filled", l_circle_filled);
            LUA_REG(-1, "triangle", l_triangle);
            LUA_REG(-1, "triangle_filled", l_triangle_filled);
            LUA_REG(-1, "capsule", l_capsule);
            LUA_REG(-1, "image", l_image);
            LUA_REG(-1, "get_menu_position", l_get_menu_position);
            LUA_REG(-1, "play_radio", l_play_radio);
            LUA_REG(-1, "stop_radio", l_stop_radio);
            lua_setglobal(L, "renderer");

            // input
            lua_newtable(L);
            LUA_REG(-1, "mouse_pos", l_mouse_pos);
            LUA_REG(-1, "mouse_delta", l_mouse_delta);
            LUA_REG(-1, "scroll_delta", l_scroll_delta);
            LUA_REG(-1, "is_key_down", l_is_key_down);
            LUA_REG(-1, "is_key_pressed", l_is_key_pressed);
            lua_setglobal(L, "input");

            // globals
            lua_newtable(L);
            LUA_REG(-1, "realtime", l_realtime);
            LUA_REG(-1, "delta_time", l_delta_time);
            LUA_REG(-1, "framerate", l_framerate);
            LUA_REG(-1, "tick_count", l_tick_count);
            LUA_REG(-1, "username", l_username);
            lua_setglobal(L, "globals");

            // client
            lua_newtable(L);
            LUA_REG(-1, "log", l_log);
            LUA_REG(-1, "error", l_error);
            lua_setglobal(L, "client");

            // system
            lua_newtable(L);
            LUA_REG(-1, "set_clipboard", l_set_clipboard);
            LUA_REG(-1, "get_clipboard", l_get_clipboard);
            lua_setglobal(L, "system");

            // config
            lua_newtable(L);
            LUA_REG(-1, "save", l_save_cfg);
            LUA_REG(-1, "load", l_load_cfg);
            lua_setglobal(L, "config");

            // ui (таблица открыта, не закрываем до конца)
            lua_newtable(L);
            LUA_REG(-1, "is_menu_open", l_is_menu_open);
            LUA_REG(-1, "add_tab", l_add_tab);
            LUA_REG(-1, "get_active_tab", l_get_active_tab);
            LUA_REG(-1, "begin_panel", l_begin_panel);
            LUA_REG(-1, "end_panel", l_end_panel);
            LUA_REG(-1, "separator", l_separator);
            LUA_REG(-1, "spacing", l_spacing);
            LUA_REG(-1, "same_line", l_same_line);
            LUA_REG(-1, "text", l_ui_text);
            LUA_REG(-1, "checkbox", l_checkbox);
            LUA_REG(-1, "slider_float", l_slider_float);
            LUA_REG(-1, "slider_int", l_slider_int);
            LUA_REG(-1, "keybind", l_keybind);
            LUA_REG(-1, "color_picker", l_color_picker);
            LUA_REG(-1, "combo", l_combo);
            LUA_REG(-1, "multi_combo", l_multi_combo);
            LUA_REG(-1, "button", l_button);
            LUA_REG(-1, "input_text", l_input_text);
            LUA_REG(-1, "input_int", l_input_int);
            LUA_REG(-1, "header", l_header);
            LUA_REG(-1, "radio", l_radio);
            LUA_REG(-1, "progress_bar", l_progress_bar);
            LUA_REG(-1, "tooltip", l_tooltip);

            // ui.TABS
            lua_newtable(L);
            lua_pushinteger(L, 0); lua_setfield(L, -2, "AIMBOT");
            lua_pushinteger(L, 1); lua_setfield(L, -2, "PLAYERS");
            lua_pushinteger(L, 2); lua_setfield(L, -2, "VISUALS");
            lua_pushinteger(L, 3); lua_setfield(L, -2, "MISC");
            lua_pushinteger(L, 4); lua_setfield(L, -2, "PLAYER_LIST");
            lua_setfield(L, -2, "TABS");

            // ui.COLS
            lua_newtable(L);
            lua_pushinteger(L, 0); lua_setfield(L, -2, "LEFT");
            lua_pushinteger(L, 1); lua_setfield(L, -2, "RIGHT");
            lua_setfield(L, -2, "COLS");

            lua_setglobal(L, "ui");

            // notify
            lua_newtable(L);
            LUA_REG(-1, "push", l_notify);
            lua_pushinteger(L, notifications::INFO);    lua_setfield(L, -2, "INFO");
            lua_pushinteger(L, notifications::SUCCESS); lua_setfield(L, -2, "SUCCESS");
            lua_pushinteger(L, notifications::WARNING); lua_setfield(L, -2, "WARNING");
            lua_pushinteger(L, notifications::ERROR_T); lua_setfield(L, -2, "ERROR");
            lua_setglobal(L, "notify");

            apply_security(L);
        }

#undef LUA_REG

    } // namespace lua_api

    // ─────────────────────────────────────────────────────────────────────────
    namespace manager {

        static void ensure_dir() {
            CreateDirectoryA("C:\\Delusive", nullptr);
            CreateDirectoryA("C:\\Delusive\\Rust", nullptr);
            CreateDirectoryA("C:\\Delusive\\Rust\\Scripts", nullptr);
            CreateDirectoryA("C:\\Delusive\\Rust\\Scripts\\Libs", nullptr);
        }

        static std::string read_file(const std::string& path) {
            std::ifstream f(path, std::ios::binary);
            if (!f.is_open()) return {};
            std::ostringstream ss;
            ss << f.rdbuf();
            return ss.str();
        }

        static void save_content(script_entry& sc) {
            ensure_dir();
            std::ofstream f(sc.path, std::ios::binary | std::ios::trunc);
            if (f.is_open()) f << sc.content;
        }

        static std::string format_size(size_t bytes) {
            if (bytes < 1024)        return std::to_string(bytes) + " B";
            if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + " KB";
            return                        std::to_string(bytes / (1024 * 1024)) + " MB";
        }

        static void unload(script_entry& sc) {
            if (sc.L) {
                // FIX: чистим panel_depth при выгрузке скрипта
                lua_api::panel_depth.erase(sc.L);
            }
            for (auto* tex : sc.loaded_textures)
                if (tex) tex->Release();
            sc.loaded_textures.clear();

            if (sc.L) { lua_close(sc.L); sc.L = nullptr; }
            sc.has_draw = sc.has_init = sc.loaded = false;
            sc.last_error.clear();
            LOG("[Lua] Unloaded script '%s'", sc.name.c_str());
        }

        static bool load(script_entry& sc) {
            LOG("[Lua] Loading script '%s' from '%s'", sc.name.c_str(), sc.path.c_str());
            unload(sc);

            sc.L = luaL_newstate();
            if (!sc.L) {
                sc.last_error = "luaL_newstate() returned null";
                LOG_ERROR("[Lua] load '%s': %s", sc.name.c_str(), sc.last_error.c_str());
                lua_api::push_error_notify(sc.name, "load", sc.last_error);
                return false;
            }

            luaL_openlibs(sc.L);
            lua_api::register_api(sc.L);
            // Инициализируем счётчик панелей для нового state
            lua_api::panel_depth[sc.L] = 0;

            if (luaL_loadfile(sc.L, sc.path.c_str()) != 0) {
                sc.last_error = lua_tostring(sc.L, -1);
                lua_pop(sc.L, 1);
                LOG_ERROR("[Lua] loadfile '%s': %s", sc.name.c_str(), sc.last_error.c_str());
                lua_api::push_error_notify(sc.name, "loadfile", sc.last_error);
                lua_close(sc.L); sc.L = nullptr;
                return false;
            }

            {
                std::string err;
                if (!lua_api::safe_pcall(sc.L, 0, 0, err)) {
                    sc.last_error = err;
                    LOG_ERROR("[Lua] exec '%s': %s", sc.name.c_str(), err.c_str());

                    if (ImGui::GetCurrentContext() && ImGui::GetFrameCount() > 0) {
                        if (err.find("blocked by Lua Security") != std::string::npos) {
                            notifications::push(
                                "Lua Security",
                                ("Script '" + sc.name + "' uses require(). Disable Lua Security to load it.").c_str(),
                                notifications::WARNING, 6.f);
                        }
                        else {
                            lua_api::push_error_notify(sc.name, "exec", err);
                        }
                    }

                    lua_close(sc.L); sc.L = nullptr;
                    return false;
                }
            }

            auto check_fn = [&](const char* name, bool& out) {
                lua_getglobal(sc.L, name);
                out = lua_isfunction(sc.L, -1);
                lua_pop(sc.L, 1);
                LOG("[Lua] '%s' has_%s = %s",
                    sc.name.c_str(), name, out ? "true" : "false");
                };
            check_fn("draw", sc.has_draw);
            check_fn("init", sc.has_init);

            if (sc.has_init) {
                lua_getglobal(sc.L, "init");
                std::string err;
                if (!lua_api::safe_pcall(sc.L, 0, 0, err)) {
                    sc.last_error = err;
                    LOG_ERROR("[Lua] init '%s': %s", sc.name.c_str(), err.c_str());
                    lua_api::push_error_notify(sc.name, "init", err);
                    // init ошибка — предупреждаем, но скрипт остаётся загруженным
                }
            }

            sc.loaded = true;
            LOG("[Lua] Script '%s' loaded OK", sc.name.c_str());
            return true;
        }

        static void refresh_list() {
            for (auto& sc : script_list) unload(sc);
            script_list.clear();
            ensure_dir();

            WIN32_FIND_DATAA fd;
            HANDLE h = FindFirstFileA((BASE_PATH + "*" + SCRIPTS_EXT).c_str(), &fd);
            if (h == INVALID_HANDLE_VALUE) {
                LOG("[Lua] No scripts found in '%s'", BASE_PATH.c_str());
                return;
            }

            do {
                std::string fname = fd.cFileName;
                auto pos = fname.rfind(SCRIPTS_EXT);
                if (pos == std::string::npos) continue;

                script_entry sc;
                sc.name = fname.substr(0, pos);
                sc.path = BASE_PATH + fname;
                sc.content = read_file(sc.path);
                sc.size_str = format_size(sc.content.size());
                script_list.push_back(std::move(sc));
            } while (FindNextFileA(h, &fd));

            FindClose(h);
        }

        static void draw() {
            for (auto& sc : script_list) {
                if (!sc.loaded || !sc.L) continue;

                lua_getglobal(sc.L, "draw");
                if (!lua_isfunction(sc.L, -1)) {
                    lua_pop(sc.L, 1);
                    continue;
                }

                std::string err;
                if (!lua_api::safe_pcall(sc.L, 0, 0, err)) {
                    if (sc.last_error != err) {
                        sc.last_error = err;
                        LOG_ERROR("[Lua] draw '%s': %s", sc.name.c_str(), err.c_str());
                        lua_api::push_error_notify(sc.name, "draw", err);
                    }
                }
            }
        }

        // ─────────────────────────────────────────────────────────────────────
        // FIX #6: draw_tab_column — после pcall принудительно закрываем
        // все незакрытые begin_panel чтобы не корраптить ImGui стек.
        // ─────────────────────────────────────────────────────────────────────
        static void draw_tab_column(int tab_id, int column, float column_width) {
            lua_api::current_panel_width = column_width;
            lua_api::current_panel_col = column;

            for (auto& sc : script_list) {
                if (!sc.loaded || !sc.L) continue;

                lua_getglobal(sc.L, "draw_ui");
                if (!lua_isfunction(sc.L, -1)) {
                    lua_pop(sc.L, 1);
                    continue;
                }

                lua_pushinteger(sc.L, tab_id);
                lua_pushinteger(sc.L, column);

                // Запоминаем depth ДО вызова
                int depth_before = lua_api::panel_depth.count(sc.L)
                    ? lua_api::panel_depth[sc.L] : 0;

                std::string err;
                if (!lua_api::safe_pcall(sc.L, 2, 0, err)) {
                    if (sc.last_error != err) {
                        sc.last_error = err;
                        LOG_ERROR("[Lua] draw_ui '%s' tab=%d col=%d: %s",
                            sc.name.c_str(), tab_id, column, err.c_str());
                        lua_api::push_error_notify(sc.name, "draw_ui", err);
                    }
                }

                // FIX: принудительно закрываем незакрытые begin_panel
                int& depth = lua_api::panel_depth[sc.L];
                if (depth > depth_before) {
                    LOG_ERROR("[Lua] Script '%s' leaked %d begin_panel call(s)! "
                        "Forcing end_child to prevent ImGui crash.",
                        sc.name.c_str(), depth - depth_before);
                    while (depth > depth_before) {
                        gui->end_child();
                        depth--;
                    }
                }
            }
        }

        static void init() {
            ensure_dir();
            refresh_list();
        }

        static void shutdown() {
            LOG("[Lua] Manager shutdown");
            for (auto& sc : script_list) unload(sc);
            script_list.clear();
        }

    } // namespace manager
} // namespace scripts