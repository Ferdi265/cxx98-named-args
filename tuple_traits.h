#pragma once

#include "tuple98.h"

namespace tuple_traits {
    using tuple98::nil;
    using tuple98::cons;

    // prepend a type U to a tuple T
    template <typename T, typename U>
    struct prepend;

    template <typename T, typename R, typename U>
    struct prepend<cons<T, R>, U> {
        USING(type, cons<U, cons<T, R> >);
    };

    // check if a type U is contained in a tuple T
    template <typename T, typename U>
    struct contains;

    template <typename U>
    struct contains<nil, U> {
        const static bool value = false;
    };

    template <typename R, typename U>
    struct contains<cons<U, R>, U> {
        const static bool value = true;
    };

    template <typename T, typename R, typename U>
    struct contains<cons<T, R>, U> {
        const static bool value = contains<R, U>::value;
    };

    // count the number of occurrences of a type U in a tuple T
    template <typename T, typename U>
    struct count;

    template <typename U>
    struct count<nil, U> {
        const static size_t value = 0;
    };

    template <typename R, typename U>
    struct count<cons<U, R>, U> {
        const static size_t value = 1 + count<R, U>::value;
    };

    template <typename T, typename R, typename U>
    struct count<cons<T, R>, U> {
        const static size_t value = count<R, U>::value;
    };

    // find the index of a type U in a tuple T
    template <typename T, typename U>
    struct index;

    template <typename R, typename U>
    struct index<cons<U, R>, U> {
        const static size_t value = 0;
    };

    template <typename T, typename R, typename U>
    struct index<cons<T, R>, U> {
        const static size_t value = 1 + index<R, U>::value;
    };

    // get the Nth type of a tuple T
    template <typename T, size_t N>
    struct nth;

    template <typename T, typename R>
    struct nth<cons<T, R>, 0> {
        USING(type, T);
    };

    template <typename T, typename R, size_t N>
    struct nth<cons<T, R>, N> {
        USING(type, TYPENAME_T(nth<R, N - 1>));
    };

    // map a type mapper template M<U> over a tuple T
    template <typename T, template <typename U> class M>
    struct map_type;

    template <template <typename U> class M>
    struct map_type<nil, M> {
        USING(type, nil);
    };

    template <typename T, typename R, template <typename U> class M>
    struct map_type<cons<T, R>, M> {
        USING(type, cons<TYPENAME_T(M<T>), TYPENAME_T(map_type<R, M>)>);
    };

    // map a value mapper template M<U> over a tuple T
    template <typename T, template <typename U> class M>
    struct map_value;

    template <template <typename U> class M>
    struct map_value<nil, M> {
        USING(type, nil);
        const static type value;
    };

    template <template <typename U> class M>
    const nil map_value<nil, M>::value = nil();

    template <typename T, typename R, template <typename U> class M>
    struct map_value<cons<T, R>, M> {
        USING(type, TYPENAME_T(map_type<cons<T, R>, M>));
        const static type value;
    };

    template <typename T, typename R, template <typename U> class M>
    const TYPENAME_T(map_value<cons<T, R>, M>) map_value<cons<T, R>, M>::value = tuple98::make_tuple(M<T>::value, map_value<R, M>::value);

    // TODO: rest of tuple_traits
    // TODO: check if the const members actually work (initialization order?)
    // TODO: check if I actually want to do this
}
