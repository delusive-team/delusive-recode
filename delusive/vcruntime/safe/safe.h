#pragma once
#include "../pe/image.h"
#include "../crypto/xor.h"

#include "../memory/memory.h"
#include "../memory/scan.h"

extern "C" __forceinline std::uint64_t safe_call_stub( std::uint64_t rcx );
extern "C" void* system_call_stub( );

class c_safe
{
    uintptr_t m_sys_gadget_addr = 0;
    uint32_t m_bad_index = 0xFFFFFFFF ;
public:
    __forceinline bool initialize( )
    {
        auto nt_allocate_virtual_memory = pe::get_procedure< uintptr_t >( HASH( "NtAllocateVirtualMemory" ) );
        if ( !memory::is_valid( nt_allocate_virtual_memory ) )
            return false;

        m_sys_gadget_addr = nt_allocate_virtual_memory + 0x12;
        return true;
    }

    template < typename result_t, class T, typename... args_t >
    __forceinline result_t call( const T& procedure, args_t... args_pack )
    {
        const auto fn = _call< result_t( * )( args_t... ) >( (uintptr_t)procedure );
        return fn( args_pack... );
    }

    template < typename _fn, class T >
    __forceinline _fn call_fn( const T& procedure )
    {
        const auto fn = _call< _fn >( uintptr_t( procedure ) );
        return fn;
    }

    template < typename result_t = NTSTATUS, typename proc = uintptr_t, typename... args_t >
    __forceinline result_t syscall( const proc& procedure, args_t... args_pack )
    {
        const auto fn = _syscall< result_t( * )( args_t... ) >( (uintptr_t)procedure );
        return fn( args_pack... );
    }

    template < typename _fn, typename proc = uintptr_t >
    __forceinline _fn syscall( const proc& procedure )
    {
        const auto fn = _syscall< _fn >( uintptr_t( procedure ) );
        return fn;
    }

    template < class T >
    __forceinline T _syscall( hash_t procedure )
    {
        auto index = get_system_index( procedure );
        if ( index == m_bad_index )
            return nullptr;

        __writegsdword( 0x000, index );
        __writegsqword( 0x150, m_sys_gadget_addr );

        return _call< T >( reinterpret_cast<uintptr_t>( &system_call_stub ) );
    }

    template < class T >
    __forceinline T _syscall( uint32_t index )
    {
        if ( index == m_bad_index )
            return nullptr;

        __writegsdword( 0x000, index );
        __writegsqword( 0x150, m_sys_gadget_addr );

        return _call< T >( reinterpret_cast<uintptr_t>( &system_call_stub ) );
    }

    const uint32_t get_system_index( hash_t procedure );

    template < class T >
    __forceinline uint32_t get_vidx( T procedure )
    {
        uint32_t virtual_index = 0xFFFFFFFF ;

        union
        {
            T unknown;
            uint8_t* pointer;
        };

        unknown = procedure;

        constexpr auto opcode_count = 0x0005;
        constexpr auto opcode_subtract = 0x0002;

        const auto opcode = *reinterpret_cast< uint16_t* >( &pointer[ opcode_count - opcode_subtract ] );
        if ( opcode == 0x60FF )
            virtual_index = *reinterpret_cast< uint8_t* >( &pointer[ opcode_count ] );
        else if ( opcode == 0xA0FF )
            virtual_index = *reinterpret_cast< uint32_t* >( &pointer[ opcode_count ] );
        else if ( opcode == 0x20FF )
            virtual_index = reinterpret_cast< uint32_t >( nullptr );

        if ( virtual_index == m_bad_index )
            return m_bad_index;

        virtual_index /= sizeof( std::uint64_t );

        return virtual_index;
    }

    template < class T >
    __forceinline void* get_vproc( void* object, T procedure )
    {
        auto virtual_index = get_vidx( procedure );
        auto virtual_table = *reinterpret_cast<LPVOID**>( object );

        return reinterpret_cast<void*>( virtual_table[ virtual_index ] );
    }

private:
    template <class T>
    __forceinline T _call( uintptr_t procedure )
    {
    #ifdef _DEBUG
        return reinterpret_cast<T>( procedure );
    #else
        return reinterpret_cast<T>( safe_call_stub( procedure ) );
    #endif
    }

    const uint32_t get_system_index( uintptr_t image, hash_t procedure );
};

inline c_safe safe;

#define proc_call( proc ) safe.call_fn< decltype( &proc ) >( pe::get_procedure( HASH( #proc ) ) )
#define _proc_call( type, proc, ... ) safe.call< type >( pe::get_procedure( HASH( #proc ) ), __VA_ARGS__ )

#define vcall( Type, Object, Name, ... ) \
  safe.call< Type >(                     \
      safe.get_vproc( Object, &std::remove_pointer< decltype( Object ) >::type::Name ), Object, __VA_ARGS__ )

#define il2cpp_call( proc ) safe.call_fn< il2cpp_##proc >( ##proc )