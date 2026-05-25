#pragma once

#include "../vcruntime.h"

#include "hash.h"

#include <array>

#include <type_traits>

namespace skc
{
    template<class _Ty>
    using clean_type = typename std::remove_const_t<std::remove_reference_t<_Ty>>;

    template <int _size, char _key1, char _key2, typename T>
    class skCrypter
    {
    public:
        __forceinline constexpr skCrypter(T* data)
        {
            crypt(data);
        }

        __forceinline T* get()
        {
            return _storage;
        }

        __forceinline int size()
        {
            return _size;
        }

        __forceinline char key()
        {
            return _key1;
        }

        __forceinline  T* encrypt()
        {
            if (!is_encrypted())
                crypt(_storage);

            return _storage;
        }

        __forceinline  T* decrypt()
        {
            if (is_encrypted())
                crypt(_storage);

            return _storage;
        }

        __forceinline bool is_encrypted()
        {
            return _storage[_size - 1] != 0;
        }

        __forceinline void clear()
        {
            for (int i = 0; i < _size; i++)
                _storage[i] = 0;
        }

        __forceinline operator T* ()
        {
            decrypt();

            return _storage;
        }

    private:
        __forceinline constexpr void crypt(T* data)
        {
            for (int i = 0; i < _size; i++)
                _storage[i] = data[i] ^ (_key1 + i % (1 + _key2));
        }

        T _storage[_size]{};
    };
}

#define XOR_STRING(str) XOR_KEY(str, __TIME__[4], __TIME__[7])
#define XOR_KEY(str, key1, key2) []() { \
			constexpr auto crypted = skc::skCrypter \
				<sizeof(str) / sizeof(str[0]), key1, key2, skc::clean_type<decltype(str[0])>>((skc::clean_type<decltype(str[0])>*)str); \
					return crypted; }()

#ifdef _DEBUG
#define _( str ) str
#else
#define _( str ) XOR_STRING( str ).decrypt( )
#endif

#ifdef _MSC_VER
#define XORSTR_FORCEINLINE __forceinline
#else
#define XORSTR_FORCEINLINE __attribute__((always_inline)) inline
#endif