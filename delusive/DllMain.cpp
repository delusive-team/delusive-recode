#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/imgui/stb_image.h"
#include "DllMain.h"
#include "core/renderer.h"
#include "core/unloader.h"

#pragma comment( linker, "/SECTION:.blfp4s,ERW" )
#pragma comment( linker, "/merge:.text=.blfp4s" )
#pragma comment( linker, "/merge:.data=.text"   )
#pragma comment( linker, "/merge:.pdata=.rdata" )
#pragma comment( linker, "/merge:.CRT=.rdata"   )
#pragma comment( linker, "/merge:.rdata=.data"  )

std::string     g_discord_username  = "";
std::string     g_discord_id        = "";
void*           g_discord_avatar    = nullptr;
volatile bool   g_discord_ready     = false;

ID3D11Device*           g_pd3dDevice            = nullptr;
ID3D11DeviceContext*    g_device_context        = nullptr;
ID3D11RenderTargetView* g_render_target_view    = nullptr;
IDXGISwapChain*         g_swap_chain            = nullptr;
void**                  g_swap_chain_vtable     = nullptr;

WNDPROC         g_original_wndproc          = nullptr;
present_t       g_original_present          = nullptr;
resize_buffers_t g_original_resize_buffers  = nullptr;

bool    g_renderer_initialized   = false;
bool    g_interfaces_initialized = false;
HMODULE g_module                 = nullptr;

bool LoadTextureFromFile(const char* filename, ID3D11Device* d3dDevice, ID3D11ShaderResourceView** out_srv)
{
    return renderer::load_texture(filename, d3dDevice, out_srv);
}
namespace core {
    volatile bool g_unloading = false;
}

#define unref( param ) UNREFERENCED_PARAMETER( param )

static DWORD WINAPI main_thread( LPVOID )
{
#ifdef _DEBUG
    logger::initialize();
#endif

    if ( !renderer::hook_swap_chain() ) {
        LOG_ERROR( "hook_swap_chain failed, unloading" );
        FreeLibraryAndExitThread( g_module, 1 );
        return 1;
    }

    HANDLE watcher = CreateThread( nullptr, 0, unloader::watcher, nullptr, 0, nullptr );
    if ( watcher ) CloseHandle( watcher );

    return 0;
}

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
    unref( lpvReserved );

    if ( fdwReason == DLL_PROCESS_ATTACH ) {
        if ( !memory::set_image_info( reinterpret_cast<uintptr_t>(hinstDLL) ) || !secure.initialize() || !safe.initialize() )
            return FALSE;

        g_module = hinstDLL;
        DisableThreadLibraryCalls( g_module );

        HANDLE t = CreateThread( nullptr, 0, main_thread, nullptr, 0, nullptr );
        if ( t ) CloseHandle( t );
    }
    else if ( fdwReason == DLL_PROCESS_DETACH ) {
        if ( !core::g_unloading )
            unloader::run( true );
    }

    return TRUE;
}