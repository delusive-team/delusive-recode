#pragma once
#include "../DllMain.h"
#include "../game/features/esp/esp.h"

namespace renderer {

    static bool patch_vtable( void* object, size_t index, void* detour, void** original )
    {
        if ( !object || !detour ) return false;

        auto** vtable = *reinterpret_cast<void***>( object );
        if ( !vtable ) return false;

        if ( !g_swap_chain_vtable ) g_swap_chain_vtable = vtable;
        if ( original ) *original = vtable[index];

        DWORD old = 0;
        if ( !VirtualProtect( &vtable[index], sizeof( void* ), PAGE_EXECUTE_READWRITE, &old ) )
            return false;

        vtable[index] = detour;
        VirtualProtect( &vtable[index], sizeof( void* ), old, &old );
        return true;
    }

    static void unpatch_vtable( size_t index, void* original )
    {
        if ( !g_swap_chain_vtable || !original ) return;

        __try {
            DWORD old = 0;
            VirtualProtect( &g_swap_chain_vtable[index], sizeof( void* ), PAGE_EXECUTE_READWRITE, &old );
            g_swap_chain_vtable[index] = original;
            VirtualProtect( &g_swap_chain_vtable[index], sizeof( void* ), old, &old );
        }
        __except ( EXCEPTION_EXECUTE_HANDLER ) {}
    }

    static void create_render_target()
    {
        ID3D11Texture2D* back = nullptr;
        g_swap_chain->GetBuffer( 0, IID_PPV_ARGS( &back ) );
        if ( back ) {
            g_pd3dDevice->CreateRenderTargetView( back, nullptr, &g_render_target_view );
            back->Release();
        }
    }

    static void cleanup_render_target()
    {
        if ( g_render_target_view ) {
            g_render_target_view->Release();
            g_render_target_view = nullptr;
        }
    }

    static bool load_texture( const char* filename, ID3D11Device* device, ID3D11ShaderResourceView** out_srv )
    {
        int w = 0, h = 0;
        unsigned char* data = stbi_load( filename, &w, &h, nullptr, 4 );
        if ( !data ) return false;

        D3D11_TEXTURE2D_DESC desc    = {};
        desc.Width                   = w;
        desc.Height                  = h;
        desc.MipLevels               = 1;
        desc.ArraySize               = 1;
        desc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count        = 1;
        desc.Usage                   = D3D11_USAGE_DEFAULT;
        desc.BindFlags               = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA sub   = {};
        sub.pSysMem                  = data;
        sub.SysMemPitch              = w * 4;

        ID3D11Texture2D* tex = nullptr;
        device->CreateTexture2D( &desc, &sub, &tex );

        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc    = {};
        srv_desc.Format                             = DXGI_FORMAT_R8G8B8A8_UNORM;
        srv_desc.ViewDimension                      = D3D11_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels                = 1;

        device->CreateShaderResourceView( tex, &srv_desc, out_srv );
        tex->Release();
        stbi_image_free( data );
        return true;
    }

    static LRESULT WINAPI hooked_wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (var && var->gui.menu_opened) {
            if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
                return true;
        }
        return CallWindowProcW(g_original_wndproc, hWnd, msg, wParam, lParam);
    }
    static bool initialize_imgui(IDXGISwapChain* swap_chain)
    {
        DXGI_SWAP_CHAIN_DESC desc = {};
        swap_chain->GetDesc(&desc);
        HWND hwnd = desc.OutputWindow;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(g_pd3dDevice, g_device_context);

        const float screen_h = static_cast<float>(desc.BufferDesc.Height > 0
            ? desc.BufferDesc.Height : 1080);
        const float intro_fs = screen_h / 30.f; 

        ImFontConfig cfg;
        cfg.FontDataOwnedByAtlas = false;

        cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_MonoHinting;
        var->font.icons[0] = io.Fonts->AddFontFromMemoryTTF(section_icons_hex, sizeof section_icons_hex, 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        var->font.icons[1] = io.Fonts->AddFontFromMemoryTTF(icons_hex, sizeof icons_hex, 5.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        var->font.tahoma = io.Fonts->AddFontFromMemoryTTF(tahoma_hex, sizeof tahoma_hex, 13.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

        cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint;
        var->font.segoe_ui = io.Fonts->AddFontFromMemoryTTF(segoe_ui, sizeof segoe_ui, intro_fs, &cfg, io.Fonts->GetGlyphRangesCyrillic());

        io.FontDefault = var->font.tahoma;

        g_original_wndproc = reinterpret_cast<WNDPROC>(
            SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hooked_wndproc)));

        return true;
    }
#ifdef _DEBUG
    static DWORD WINAPI console_connecting_anim( LPVOID )
    {
        while ( !g_discord_ready && !core::g_unloading )
            Sleep( 400 );
        return 0;
    }
#endif

    static void on_discord_ready( const DiscordUser* user )
    {
        g_discord_username = user->username;
        g_discord_id       = user->userId;
        g_discord_ready    = true;

        if ( user->avatar && user->avatar[0] != '\0' ) {
            std::string url = "https://cdn.discordapp.com/avatars/"
                + std::string( user->userId ) + "/"
                + std::string( user->avatar ) + ".png";

            std::thread( [url]() {
                char cache[MAX_PATH];
                if ( SUCCEEDED( URLDownloadToCacheFileA( nullptr, url.c_str(), cache, MAX_PATH, 0, nullptr ) ) ) {
                    ID3D11ShaderResourceView* tex = nullptr;
                    if ( load_texture( cache, g_pd3dDevice, &tex ) )
                        g_discord_avatar = tex;
                }
            } ).detach();
        }
    }

    static bool initialize( IDXGISwapChain* swap_chain )
    {
        g_swap_chain = swap_chain;

        ID3D11Device* device = nullptr;
        swap_chain->GetDevice( __uuidof( ID3D11Device ), reinterpret_cast<void**>( &device ) );
        if ( !device ) { LOG( "Failed to get D3D11Device from swap chain" ); return false; }

        g_pd3dDevice = device;
        device->GetImmediateContext( &g_device_context );
        device->Release();

        create_render_target();

        if ( !initialize_imgui( swap_chain ) ) return false;

        Discord::GetInstance().Initialize( on_discord_ready );

#ifdef _DEBUG
        HANDLE anim = CreateThread( nullptr, 0, console_connecting_anim, nullptr, 0, nullptr );
        if ( anim ) CloseHandle( anim );
#endif

        if ( !il2cpp::initialize() ) {
            LOG_ERROR( "il2cpp::initialize failed — game features unavailable" );
        }
        else {
            sdk::initialize();
            hooks::initialize();
            g_hooks.mark_initialized();

            if ( il2cpp::resolve_errors > 0 )
                LOG_ERROR( "SDK initialized not successfully, have %d error(s)", il2cpp::resolve_errors );
            else
                LOG_OK( "All SDK initialized successfully" );
        }

        g_renderer_initialized    = true;
        g_interfaces_initialized  = true;
        return true;
    }

    HRESULT __stdcall hk_present( IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags )
    {
        if ( core::g_unloading ) {
            ClipCursor(NULL);
            while (ShowCursor(TRUE) < 0);
            return g_original_present( swap_chain, sync_interval, flags );
        }

        if ( !g_interfaces_initialized ) {
            DXGI_SWAP_CHAIN_DESC desc = {};
            swap_chain->GetDesc( &desc );

            if ( desc.OutputWindow && IsWindow( desc.OutputWindow ) &&
                 desc.BufferDesc.Width > 0 && desc.BufferDesc.Height > 0 ) {
                if ( !initialize( swap_chain ) )
                    return g_original_present( swap_chain, sync_interval, flags );
            }
            else {
                return g_original_present( swap_chain, sync_interval, flags );
            }
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        Discord_RunCallbacks();

        static bool was_menu_opened = false;
        if ( var->gui.menu_opened ) {

            if (!was_menu_opened) {
                ClipCursor(NULL);
                while (ShowCursor(TRUE) < 0);
                was_menu_opened = true;
            }
            SetCursor(LoadCursor(NULL, IDC_ARROW));
        } else {
            if (was_menu_opened) {
                while (ShowCursor(TRUE) < 0);
                was_menu_opened = false;
            }
        }

        if (!gui->intro_completed) {
            gui->render();
        } else {
            scripts::manager::draw();
            //esp_features::render();
            gui->render_keybinds_list();
            gui->render_reload_indicator();
            gui->render_flyhack_indicator();
            gui->render_traps_indicator();
            gui->watermark();
            gui->render();
        }

        ImGui::Render();

        g_device_context->OMSetRenderTargets( 1, &g_render_target_view, nullptr );
        ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );

        return g_original_present( swap_chain, sync_interval, flags );
    }

    HRESULT __stdcall hk_resize_buffers( IDXGISwapChain* swap_chain, UINT buffer_count,
                                          UINT width, UINT height, DXGI_FORMAT format, UINT flags )
    {
        cleanup_render_target();
        HRESULT hr = g_original_resize_buffers( swap_chain, buffer_count, width, height, format, flags );
        if ( SUCCEEDED( hr ) ) create_render_target();
        return hr;
    }

    static bool hook_swap_chain()
    {
        HWND game_window = nullptr;
        for ( int i = 0; i < 300 && !game_window; ++i ) {
            game_window = FindWindowA( "UnityWndClass", nullptr );
            if ( !game_window ) Sleep( 100 );
        }
        if ( !game_window ) { LOG_ERROR( "Game window not found" ); return false; }

        DXGI_SWAP_CHAIN_DESC sd    = {};
        sd.BufferCount             = 1;
        sd.BufferDesc.Format       = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferUsage             = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow            = game_window;
        sd.SampleDesc.Count        = 1;
        sd.Windowed                = TRUE;
        sd.SwapEffect              = DXGI_SWAP_EFFECT_DISCARD;

        IDXGISwapChain* dummy_sc   = nullptr;
        ID3D11Device*   dummy_dev  = nullptr;
        D3D_FEATURE_LEVEL fl       = D3D_FEATURE_LEVEL_11_0;

        HRESULT hr = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                                    0, nullptr, 0, D3D11_SDK_VERSION,
                                                    &sd, &dummy_sc, &dummy_dev, &fl, nullptr );
        if ( FAILED( hr ) ) { LOG_ERROR( "D3D11CreateDeviceAndSwapChain failed: 0x%X", hr ); return false; }

        bool ok_p  = patch_vtable( dummy_sc, k_present,       (void*)hk_present,       (void**)&g_original_present );
        bool ok_rb = patch_vtable( dummy_sc, k_resize_buffers, (void*)hk_resize_buffers, (void**)&g_original_resize_buffers );

        dummy_dev->Release();
        dummy_sc->Release();

        if ( !ok_p || !ok_rb ) { LOG_ERROR( "Failed to hook SwapChain vtable" ); return false; }
        return true;
    }

} // namespace renderer
