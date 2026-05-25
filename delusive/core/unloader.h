#pragma once
#include "../DllMain.h"
#include "renderer.h"
#include "../game/features/visuals/visuals.h"

namespace unloader {

    static bool g_unloaded = false;

    static void run( bool from_detach = false )
    {
        if ( g_unloaded ) return;
        g_unloaded       = true;
        if (g_interfaces_initialized && !core::g_unloading) {
            convar::Admin::set_admintime(-1.0f);
            visuals::restore_all();
        }

        core::g_unloading = true;

        Sleep( 300 );

        g_hooks.destroy_all();
        LOG( "hooks destroyed" );

        scripts::manager::shutdown();
        Discord::GetInstance().Unload();

        if ( g_discord_avatar ) {
            reinterpret_cast<ID3D11ShaderResourceView*>( g_discord_avatar )->Release();
            g_discord_avatar = nullptr;
        }

        if ( g_original_wndproc ) {
            DXGI_SWAP_CHAIN_DESC desc = {};
            if ( g_swap_chain ) g_swap_chain->GetDesc( &desc );
            if ( desc.OutputWindow )
                SetWindowLongPtrW( desc.OutputWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( g_original_wndproc ) );
        }

        if ( g_renderer_initialized ) {
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
            renderer::cleanup_render_target();
        }

        if ( g_device_context ) { g_device_context->Release(); g_device_context = nullptr; }

        renderer::unpatch_vtable( k_present,       g_original_present );
        renderer::unpatch_vtable( k_resize_buffers, g_original_resize_buffers );

        Sleep( 200 );

#ifdef _DEBUG
        HWND console_wnd = GetConsoleWindow();
        if ( console_wnd ) ShowWindow( console_wnd, SW_HIDE );
        
        FILE* fp = nullptr;
        freopen_s(&fp, "NUL", "w", stdout);
        freopen_s(&fp, "NUL", "r", stdin);
        
        FreeConsole();
#endif

        if ( !from_detach )
            FreeLibraryAndExitThread( g_module, 0 );
    }

    static DWORD WINAPI watcher( LPVOID )
    {
        int last_key = config::menu::unload_key.value;
        while ( !core::g_unloading ) {
            int current_key = config::menu::unload_key.value;
            if ( current_key != last_key ) {
                if ( current_key != 0 ) {
                    GetAsyncKeyState( current_key );
                }
                last_key = current_key;
            }

            if ( current_key != 0 &&
                 GetAsyncKeyState( current_key ) & 1 )
                break;
            Sleep( 50 );
        }
        run();
        return 0;
    }

} // namespace unloader
