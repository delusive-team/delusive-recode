#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <tchar.h>
#include <thread>
#include <urlmon.h>
#include <string>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "urlmon.lib")

#include "framework/settings/functions.h"
#include "framework/data/fonts.h"
#include "framework/data/images.h"

#include "thirdparty/imgui/imgui.h"
#include "thirdparty/imgui/backends/imgui_impl_win32.h"
#include "thirdparty/imgui/backends/imgui_impl_dx11.h"
#include "thirdparty/imgui/imgui_freetype.h"
#include "thirdparty/discord/discord.h"

#include "configs/configs.h"
#include "scripts/scripts.h"
#include "vcruntime/logger/logger.h"
#include "vcruntime/memory/memory.h"
#include "vcruntime/security/secure.h"
#include "game/il2cpp/il2cpp.h"
#include "game/sdk/sdk.h"
#include "game/hooks/hooks.h"

extern std::string      g_discord_username;
extern std::string      g_discord_id;
extern void*            g_discord_avatar;
extern volatile bool    g_discord_ready;

extern ID3D11Device*            g_pd3dDevice;
extern ID3D11DeviceContext*     g_device_context;
extern ID3D11RenderTargetView*  g_render_target_view;
extern IDXGISwapChain*          g_swap_chain;
extern void**                   g_swap_chain_vtable;

extern WNDPROC          g_original_wndproc;

using present_t         = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
using resize_buffers_t  = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

extern present_t        g_original_present;
extern resize_buffers_t g_original_resize_buffers;

constexpr UINT_PTR k_present        = 8;
constexpr UINT_PTR k_resize_buffers = 13;

extern bool g_renderer_initialized;
extern bool g_interfaces_initialized;
extern HMODULE g_module;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);