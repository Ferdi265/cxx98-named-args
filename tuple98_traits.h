#pragma once

#include <cstddef>
#include "tuple98.h"

namespace tuple98_traits {
    using tuple98::nil;
    using tuple98::cons;

    // prepend a type U to a tuple T
    template <typename T, typename U>
    struct prepend;

    template <typename U>
    struct prepend<nil, U> {
        USING(type, cons<U, nil>);
    };

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

        static type value() {
            return nil();
        }
    };

    template <typename T, typename R, template <typename U> class M>
    struct map_value<cons<T, R>, M> {
        USING(type, TYPENAME_T(map_type<cons<T, R>, M>));

        static type value() {
            return tuple98::make_tuple(M<T>::value, map_value<R, M>::value());
        }
    };

    // map a value fn mapper template M<U> over a tuple T
    template <typename T, template <typename U> class M>
    struct map_value_fn;

    template <template <typename U> class M>
    struct map_value_fn<nil, M> {
        USING(type, nil);

        static type value() {
            return nil();
        }
    };

    template <typename T, typename R, template <typename U> class M>
    struct map_value_fn<cons<T, R>, M> {
        USING(type, TYPENAME_T(map_type<cons<T, R>, M>));

        static type value() {
            return tuple98::make_tuple(M<T>::value(), map_value<R, M>::value());
        }
    };

    // template wrapper structs for use as a mapper
    template <typename T>
    struct type_of {
        USING(type, TYPENAME_T(T));
    };

    template <typename T>
    struct value_of {
        static TYPENAME_T(T) value() {
            return T::value;
        }
    };

    template <typename T>
    struct value_fn_of {
        static TYPENAME_T(T) value() {
            return T::value();
        }
    };

    // get the types of a tuple T of type wrappers
    template <typename T>
    struct types {
        USING(type, TYPENAME_T(map_type<T, type_of>));
    };

    // get the values of a tuple T of value wrappers
    template <typename T>
    struct values {
        static TYPENAME_T(map_type<T, type_of>) value() {
            return map_value<T, value_of>::value;
        }
    };

    // get the values of a tuple T of value fn wrappers
    template <typename T>
    struct value_fns {
        static TYPENAME_T(map_type<T, type_of>) value() {
            return map_value<T, value_fn_of>::value();
        }
    };
}
