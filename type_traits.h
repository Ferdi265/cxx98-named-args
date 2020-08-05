#pragma once

#include <cstddef>

// macros to reduce the amount of typename bloat
#define TYPE_T(...) __VA_ARGS__::type
#define TYPENAME_T(...) typename __VA_ARGS__::type
#define USING(T, ...) typedef TYPENAME_T(::type_traits::ident<__VA_ARGS__>) T

namespace type_traits {
    // identity type trait for bloat reduction macros
    template <typename T>
    struct ident {
        typedef T type;
    };

    // type comparison
    template <typename T, typename U>
    struct is_same {
        const static bool value = false;
    };

    template <typename T>
    struct is_same<T, T> {
        const static bool value = true;
    };

    // conditional type selection
    template <bool C, typename T, typename U>
    struct conditional;

    template <typename T, typename U>
    struct conditional<true, T, U> {
        USING(type, T);
    };

    template <typename T, typename U>
    struct conditional<false, T, U> {
        USING(type, U);
    };
}
