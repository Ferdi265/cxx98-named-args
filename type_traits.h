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

    // get the type that is returned as a result of calling a function
    template <typename T>
    struct result_of;

    template <typename T>
    struct result_of<T*> {
        USING(type, TYPENAME_T(result_of<T>));
    };

    template <typename T>
    struct result_of<T&> {
        USING(type, TYPENAME_T(result_of<T>));
    };

    template <typename R>
    struct result_of<R()> {
        USING(type, R);
    };

    template <typename R, typename A1>
    struct result_of<R(A1)> {
        USING(type, R);
    };

    template <typename R, typename A1, typename A2>
    struct result_of<R(A1, A2)> {
        USING(type, R);
    };

    template <typename R, typename A1, typename A2, typename A3>
    struct result_of<R(A1, A2, A3)> {
        USING(type, R);
    };

    template <typename R, typename A1, typename A2, typename A3, typename A4>
    struct result_of<R(A1, A2, A3, A4)> {
        USING(type, R);
    };

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
    struct result_of<R(A1, A2, A3, A4, A5)> {
        USING(type, R);
    };

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    struct result_of<R(A1, A2, A3, A4, A5, A6)> {
        USING(type, R);
    };

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    struct result_of<R(A1, A2, A3, A4, A5, A6, A7)> {
        USING(type, R);
    };

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    struct result_of<R(A1, A2, A3, A4, A5, A6, A7, A8)> {
        USING(type, R);
    };

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    struct result_of<R(A1, A2, A3, A4, A5, A6, A7, A8, A9)> {
        USING(type, R);
    };
}
