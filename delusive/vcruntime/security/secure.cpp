#include "secure.h"

#ifndef _DEBUG
bool c_secure::validate_cookie( uint64_t reg_cookie ) {
    bytes_t _cookie{ reg_cookie };
    _cookie.u32[ 1 ] ^= 0xDEADC0DE;
    _cookie.u64 = ~_cookie.u64;
    return _cookie.u64 == cookie;
}

uint64_t c_secure::get_hashed_time( ) {
    auto type = REG_QWORD;
    DWORD size = 8;
    uintptr_t val = 0;
    uintptr_t _cookie = 0;
    HKEY key;

    if ( proc_call( RegOpenKeyExA )(
        HKEY_CURRENT_USER, _( "SOFTWARE\\Microsoft\\" ), 0, KEY_ALL_ACCESS, &key )
        != ERROR_SUCCESS )
        return 0ull;

    if ( proc_call( RegQueryValueExA )(
        key, _( "SysData" ), 0, &type, reinterpret_cast< uint8_t* >( &val ), &size )
        != ERROR_SUCCESS )
        return 0ull;

    if ( proc_call( RegDeleteValueA )( key, _( "SysData" ) ) != ERROR_SUCCESS )
        return 0ull;

    if ( proc_call( RegQueryValueExA )(
        key, _( "SessionKey" ), 0, &type, reinterpret_cast< uint8_t* >( &_cookie ), &size )
        != ERROR_SUCCESS )
        return 0ull;

    if ( proc_call( RegDeleteValueA )( key, _( "SessionKey" ) ) != ERROR_SUCCESS )
        return 0ull;

    if ( !validate_cookie( _cookie ) )
        return 0ull;

    if ( proc_call( RegCloseKey )( key ) != ERROR_SUCCESS )
        return 0ull;

    return val;
}

std::pair< HANDLE, NTSTATUS > c_secure::mutant_check( ) {
    auto m_handle = INVALID_HANDLE_VALUE;
    OBJECT_ATTRIBUTES oa = { };
    HANDLE root;
    std::wstring mutex_name = _( L"Local\\SM0:" );
    bytes_t _cookie{ cookie };
    mutex_name += std::to_wstring( ~_cookie.u32[ 0 ] );
    mutex_name += _( L":304:WilStaging_02" );
    us.Length = mutex_name.length( ) * 2;
    us.Buffer = mutex_name.data( );
    us.MaximumLength = us.Length * 2;

    _proc_call( HANDLE, "BaseGetNamedObjectDirectory", &root );
    InitializeObjectAttributes( &oa, &us, 0, root, nullptr );
    auto status = _proc_call( NTSTATUS, "NtCreateMutant", &m_handle, MUTANT_ALL_ACCESS, &oa, 0 );

    return { m_handle, status };
}

bool c_secure::registry_check( ) {
    auto hash = get_hashed_time( );

    if ( hash <= 0 )
        return false;

    bytes_t time{ hash };
    bytes_t enc_key{ cookie };

    for ( auto i = 0; i < sizeof( time ); i++ )
        time.u8[ i ] ^= enc_key.u8[ i % 4 ];

    auto curr_time = GET_TIMESTAMP( system_time );
    if ( curr_time <= 0 || curr_time >= MAX_TIMESTAMP )
        return false;

    auto reg_time = GET_TIMESTAMP( time.u64 );
    if ( reg_time <= 0 || reg_time >= MAX_TIMESTAMP )
        return false;

    auto max_time = reg_time + 300;
    if ( max_time >= MAX_TIMESTAMP )
        return false;

    if ( curr_time <= reg_time || curr_time >= max_time || curr_time >= MAX_TIMESTAMP )
        return false;

    return true;
}

void c_secure::show_raise_message( std::wstring message, std::wstring caption, uint32_t flags ) {
    UNICODE_STRING msg_body = { };
    UNICODE_STRING msg_caption = { };
    uint32_t error_response;

    msg_body.Length = message.length( ) * 2;
    msg_body.Buffer = message.data( );
    msg_body.MaximumLength = msg_body.Length * 2;

    msg_caption.Length = caption.length( ) * 2;
    msg_caption.Buffer = caption.data( );
    msg_caption.MaximumLength = msg_caption.Length * 2;

    const uintptr_t msg_params[] = { reinterpret_cast< uintptr_t >( &msg_body ),
      reinterpret_cast< uintptr_t >( &msg_caption ), flags | MB_TOPMOST };

    _proc_call( NTSTATUS, "NtRaiseHardError", 0x50000018, 0x3, 3, msg_params, NULL, &error_response );
}
#endif // _DEBUG

bool c_secure::initialize( ) {
#ifdef _DEBUG
    // Bypass all security and anti-debug checks in DEBUG mode.
    return true;
#else
    // 1. Check for basic debugger presence
    auto peb = reinterpret_cast<PPEB>(__readgsqword(0x60));
    if (peb->BeingDebugged) {
        show_raise_message(_(L"Debugger detected! Please unload any debugging tools and try again."), _(L"Security Alert"), MB_ICONERROR);
        return false;
    }

    // 2. Check for remote debugger via NtQueryInformationProcess
    uintptr_t debug_port = 0;
    _proc_call(NTSTATUS, "NtQueryInformationProcess", (HANDLE)-1, 7 /*ProcessDebugPort*/, &debug_port, sizeof(debug_port), nullptr);
    if (debug_port != 0) {
        show_raise_message(_(L"Remote debugger detected! Please unload any debugging tools and try again."), _(L"Security Alert"), MB_ICONERROR);
        return false;
    }

    // 3. Mutex and License check
    auto mutant = mutant_check( );
    if ( mutant.first != INVALID_HANDLE_VALUE || !mutant.second || !registry_check( ) ) {
        show_raise_message(
            _( L"File integrity check failed or corrupted license.\n\n"
               L"This program has been launched incorrectly. "
               L"Please create ticket support and provide the error code: 5633." ),
            _(L"Integrity Violation"), MB_ICONERROR );
        return false;
    }

    return true;
#endif
}