#pragma once

#include <cstddef>
#include "tuple98.h"
#include "tuple98_traits.h"

#define STATIC_ASSERT(cond, msg) typedef int __static_assert_ ## __COUNTER__ [(cond) ? 0 : -1]

namespace named_args {
    // named argument value type
    template <typename T, typename K>
    struct arg {
        T value;

        arg() {}
        arg(T t) : value(t) {}
    };

    // named argument marker type
    template <typename K>
    struct marker {
        marker() {}

        template <typename T>
        arg<T&, K> operator=(T& t) const {
            return arg<T&, K>(t);
        }

        template <typename T>
        arg<const T&, K> operator=(const T& t) const {
            return arg<const T&, K>(t);
        }
    };

    // named argument kinds
    struct req_arg {
        const static bool required = true;
    };

    template <typename T, T _default = T()>
    struct def_arg {
        const static bool required = false;

        USING(type, T);
        const static T value = _default;
    };

    namespace detail {
        using tuple98::nil;
        using tuple98::cons;

        // get the kind of an argument
        template <typename A>
        struct arg_kind {
            USING(type, void);
        };

        template <typename T, typename K>
        struct arg_kind<arg<T, K> > {
            USING(type, K);
        };

        // transform a tuple of arguments into a tuple of kinds
        template <typename A>
        struct arg_kinds {
            USING(type, TYPENAME_T(tuple98_traits::map_type<A, arg_kind>));
        };

        // check for missing required arguments
        template <typename K, typename A>
        struct missing_req_args;

        template <typename A>
        struct missing_req_args<nil, A> {
            USING(type, nil);
            const static bool empty = true;
        };

        template <typename K, typename Ks, typename A>
        struct missing_req_args<cons<K, Ks>, A> {
            USING(type, TYPENAME_T(type_traits::conditional<
                !K::required || tuple98_traits::contains<TYPENAME_T(arg_kinds<A>), K>::value,
                TYPENAME_T(missing_req_args<Ks, A>),
                TYPENAME_T(tuple98_traits::prepend<TYPENAME_T(missing_req_args<Ks, A>), K>)
            >));
            const static bool empty = type_traits::is_same<type, nil>::value;
        };

        // check for missing non-required arguments
        template <typename K, typename A>
        struct missing_non_req_args;

        template <typename A>
        struct missing_non_req_args<nil, A> {
            USING(type, nil);
            const static bool empty = true;
        };

        template <typename K, typename Ks, typename A>
        struct missing_non_req_args<cons<K, Ks>, A> {
            USING(type, TYPENAME_T(type_traits::conditional<
                K::required || tuple98_traits::contains<TYPENAME_T(arg_kinds<A>), K>::value,
                TYPENAME_T(missing_non_req_args<Ks, A>),
                TYPENAME_T(tuple98_traits::prepend<TYPENAME_T(missing_non_req_args<Ks, A>), K>)
            >));
            const static bool empty = type_traits::is_same<type, nil>::value;
        };

        // check for duplicate arguments
        template <typename K, typename A>
        struct duplicate_args;

        template <typename A>
        struct duplicate_args<nil, A> {
            USING(type, nil);
            const static bool empty = true;
        };

        template <typename K, typename Ks, typename A>
        struct duplicate_args<cons<K, Ks>, A> {
            USING(type, TYPENAME_T(type_traits::conditional<
                tuple98_traits::count<TYPENAME_T(arg_kinds<A>), K>::value <= 1,
                TYPENAME_T(duplicate_args<Ks, A>),
                TYPENAME_T(tuple98_traits::prepend<TYPENAME_T(duplicate_args<Ks, A>), K>)
            >));
            const static bool empty = type_traits::is_same<type, nil>::value;
        };

        // check for invalid arguments
        template <typename K, typename A>
        struct invalid_args;

        template <typename K>
        struct invalid_args<K, nil> {
            USING(type, nil);
            const static bool empty = true;
        };

        template <typename K, typename A, typename As>
        struct invalid_args<K, cons<A, As> > {
            USING(type, TYPENAME_T(type_traits::conditional<
                tuple98_traits::contains<K, TYPENAME_T(arg_kind<A>)>::value,
                TYPENAME_T(invalid_args<K, As>),
                TYPENAME_T(tuple98_traits::prepend<TYPENAME_T(invalid_args<K, As>), A>)
            >));
            const static bool empty = type_traits::is_same<type, nil>::value;
        };

        // get the value of the arg with kind K from args A or rest R
        template <typename K, typename A, typename R,
            bool = tuple98_traits::contains<TYPENAME_T(arg_kinds<A>), K>::value
        >
        struct select_single;

        template <typename K, typename A, typename R>
        struct select_single<K, A, R, true> {
            const static size_t arg_index = tuple98_traits::index<TYPENAME_T(arg_kinds<A>), K>::value;

            USING(type, TYPENAME_T(tuple98_traits::nth<A, arg_index>));

            USING(rest_values, TYPENAME_T(tuple98_traits::types<R>));
            static type select(A& a, rest_values& r) {
                (void)r;
                return tuple98::get<arg_index>(a);
            }
        };

        template <typename K, typename A, typename R>
        struct select_single<K, A, R, false> {
            const static size_t arg_index = tuple98_traits::index<R, K>::value;

            USING(type, arg<TYPENAME_T(K), K>);

            USING(rest_values, TYPENAME_T(tuple98_traits::types<R>));
            static type select(A& a, rest_values& r) {
                (void)a;
                return arg<TYPENAME_T(K), K>(tuple98::get<arg_index>(r));
            }
        };

        // get the type the named argument function implementation returns
        template <typename I>
        struct impl_return;

        template <typename R>
        struct impl_return<R()> {
            USING(type, R);
        };

        template <typename R, typename A1>
        struct impl_return<R(A1)> {
            USING(type, R);
        };

        template <typename R, typename A1, typename A2>
        struct impl_return<R(A1, A2)> {
            USING(type, R);
        };

        template <typename R, typename A1, typename A2, typename A3>
        struct impl_return<R(A1, A2, A3)> {
            USING(type, R);
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4>
        struct impl_return<R(A1, A2, A3, A4)> {
            USING(type, R);
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        struct impl_return<R(A1, A2, A3, A4, A5)> {
            USING(type, R);
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        struct impl_return<R(A1, A2, A3, A4, A5, A6)> {
            USING(type, R);
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        struct impl_return<R(A1, A2, A3, A4, A5, A6, A7)> {
            USING(type, R);
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        struct impl_return<R(A1, A2, A3, A4, A5, A6, A7, A8)> {
            USING(type, R);
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        struct impl_return<R(A1, A2, A3, A4, A5, A6, A7, A8, A9)> {
            USING(type, R);
        };
    }

    // error reporting type
    template <typename missing_req_args, typename duplicate_args, typename invalid_args, bool valid = false>
    struct error {
        STATIC_ASSERT(valid, "named_args::error<missing_req_args, duplicate_args, invalid_args>: missing, duplicate, or invalid arguments");
    };

    namespace detail {
        // instantiation of error checks
        template <typename K, typename A>
        struct check_args {
            USING(missing_req_args, detail::missing_req_args<K, A>);
            USING(duplicate_args, detail::duplicate_args<K, A>);
            USING(invalid_args, detail::invalid_args<K, A>);
            const static bool valid =
                detail::missing_req_args<K, A>::empty &&
                detail::duplicate_args<K, A>::empty &&
                detail::invalid_args<K, A>::empty;
            const static named_args::error<missing_req_args, duplicate_args, invalid_args, valid> error;
        };

        // check args and then call impl_return
        template <typename I, typename K, typename A, bool = check_args<K, A>::valid>
        struct impl_return_check;

        template <typename I, typename K, typename A>
        struct impl_return_check<I, K, A, true> {
            USING(type, TYPENAME_T(impl_return<I>));
        };
    }

    // named argument function type
    // TODO: 10 partial specializations
    // TODO: 10 overloads for each partial specialization
    // TODO: check if I really want this
}
