#pragma once

#include <cstddef>
#include "type_traits.h"
#include "tuple98.h"
#include "tuple98_traits.h"

#define STATIC_ASSERT(cond, msg) typedef TYPENAME_T(type_traits::conditional<(cond), int, void>) __static_assert_ ## __COUNTER__ [0]

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

    template <typename T>
    struct manual_def_arg {
        const static bool required = false;

        USING(type, T);
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
            USING(missing_req_args, TYPENAME_T(detail::missing_req_args<K, A>));
            USING(duplicate_args, TYPENAME_T(detail::duplicate_args<K, A>));
            USING(invalid_args, TYPENAME_T(detail::invalid_args<K, A>));
            const static bool valid =
                detail::missing_req_args<K, A>::empty &&
                detail::duplicate_args<K, A>::empty &&
                detail::invalid_args<K, A>::empty;
            const static named_args::error<missing_req_args, duplicate_args, invalid_args, valid> error;
        };

        // check args and then call result_of
        template <typename I, typename K, typename A, bool = check_args<K, A>::valid>
        struct result_of_check;

        template <typename I, typename K, typename A>
        struct result_of_check<I, K, A, true> {
            USING(type, TYPENAME_T(type_traits::result_of<I>));
        };

        // base class for named function type
        // implements forwarding to implementation
        template <typename I, I impl, typename K1 = void, typename K2 = void, typename K3 = void, typename K4 = void, typename K5 = void, typename K6 = void, typename K7 = void, typename K8 = void, typename K9 = void, typename K10 = void>
        struct function_base;

        template <typename I, I impl>
        struct function_base<I, impl> {
        protected:
            USING(kinds_t, TUPLE());

            template <typename args_t>
            TYPENAME_T(type_traits::result_of<I>) apply_impl(args_t args) const {
                (void)args;

                USING(rest_kinds_t, TYPENAME_T(detail::missing_non_req_args<kinds_t, args_t>));
                USING(rest_values_t, TYPENAME_T(tuple98_traits::types<rest_kinds_t>));

                rest_values_t rest = tuple98_traits::values<rest_kinds_t>::value();
                return impl();
            }
        };

        template <typename I, I impl, typename K1>
        struct function_base<I, impl, K1> {
        protected:
            USING(kinds_t, TEMPLATE_TUPLE(K1));

            template <typename args_t>
            TYPENAME_T(type_traits::result_of<I>) apply_impl(args_t args) const {
                USING(rest_kinds_t, TYPENAME_T(detail::missing_non_req_args<kinds_t, args_t>));
                USING(rest_values_t, TYPENAME_T(tuple98_traits::types<rest_kinds_t>));

                rest_values_t rest = tuple98_traits::values<rest_kinds_t>::value();
                return impl(
                    detail::select_single<K1, args_t, rest_kinds_t>::select(args, rest).value
                );
            }
        };

        template <typename I, I impl, typename K1, typename K2>
        struct function_base<I, impl, K1, K2> {
        protected:
            USING(kinds_t, TEMPLATE_TUPLE(K1, K2));

            template <typename args_t>
            TYPENAME_T(type_traits::result_of<I>) apply_impl(args_t args) const {
                USING(rest_kinds_t, TYPENAME_T(detail::missing_non_req_args<kinds_t, args_t>));
                USING(rest_values_t, TYPENAME_T(tuple98_traits::types<rest_kinds_t>));

                rest_values_t rest = tuple98_traits::values<rest_kinds_t>::value();
                return impl(
                    detail::select_single<K1, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K2, args_t, rest_kinds_t>::select(args, rest).value
                );
            }
        };

        template <typename I, I impl, typename K1, typename K2, typename K3>
        struct function_base<I, impl, K1, K2, K3> {
        protected:
            USING(kinds_t, TEMPLATE_TUPLE(K1, K2, K3));

            template <typename args_t>
            TYPENAME_T(type_traits::result_of<I>) apply_impl(args_t args) const {
                USING(rest_kinds_t, TYPENAME_T(detail::missing_non_req_args<kinds_t, args_t>));
                USING(rest_values_t, TYPENAME_T(tuple98_traits::types<rest_kinds_t>));

                rest_values_t rest = tuple98_traits::values<rest_kinds_t>::value();
                return impl(
                    detail::select_single<K1, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K2, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K3, args_t, rest_kinds_t>::select(args, rest).value
                );
            }
        };

        template <typename I, I impl, typename K1, typename K2, typename K3, typename K4>
        struct function_base<I, impl, K1, K2, K3, K4> {
        protected:
            USING(kinds_t, TEMPLATE_TUPLE(K1, K2, K3, K4));

            template <typename args_t>
            TYPENAME_T(type_traits::result_of<I>) apply_impl(args_t args) const {
                USING(rest_kinds_t, TYPENAME_T(detail::missing_non_req_args<kinds_t, args_t>));
                USING(rest_values_t, TYPENAME_T(tuple98_traits::types<rest_kinds_t>));

                rest_values_t rest = tuple98_traits::values<rest_kinds_t>::value();
                return impl(
                    detail::select_single<K1, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K2, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K3, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K4, args_t, rest_kinds_t>::select(args, rest).value
                );
            }
        };

        template <typename I, I impl, typename K1, typename K2, typename K3, typename K4, typename K5>
        struct function_base<I, impl, K1, K2, K3, K4, K5> {
        protected:
            USING(kinds_t, TEMPLATE_TUPLE(K1, K2, K3, K4, K5));

            template <typename args_t>
            TYPENAME_T(type_traits::result_of<I>) apply_impl(args_t args) const {
                USING(rest_kinds_t, TYPENAME_T(detail::missing_non_req_args<kinds_t, args_t>));
                USING(rest_values_t, TYPENAME_T(tuple98_traits::types<rest_kinds_t>));

                rest_values_t rest = tuple98_traits::values<rest_kinds_t>::value();
                return impl(
                    detail::select_single<K1, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K2, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K3, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K4, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K5, args_t, rest_kinds_t>::select(args, rest).value
                );
            }
        };

        template <typename I, I impl, typename K1, typename K2, typename K3, typename K4, typename K5, typename K6>
        struct function_base<I, impl, K1, K2, K3, K4, K5, K6> {
        protected:
            USING(kinds_t, TEMPLATE_TUPLE(K1, K2, K3, K4, K5, K6));

            template <typename args_t>
            TYPENAME_T(type_traits::result_of<I>) apply_impl(args_t args) const {
                USING(rest_kinds_t, TYPENAME_T(detail::missing_non_req_args<kinds_t, args_t>));
                USING(rest_values_t, TYPENAME_T(tuple98_traits::types<rest_kinds_t>));

                rest_values_t rest = tuple98_traits::values<rest_kinds_t>::value();
                return impl(
                    detail::select_single<K1, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K2, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K3, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K4, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K5, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K6, args_t, rest_kinds_t>::select(args, rest).value
                );
            }
        };

        template <typename I, I impl, typename K1, typename K2, typename K3, typename K4, typename K5, typename K6, typename K7>
        struct function_base<I, impl, K1, K2, K3, K4, K5, K6, K7> {
        protected:
            USING(kinds_t, TEMPLATE_TUPLE(K1, K2, K3, K4, K5, K6, K7));

            template <typename args_t>
            TYPENAME_T(type_traits::result_of<I>) apply_impl(args_t args) const {
                USING(rest_kinds_t, TYPENAME_T(detail::missing_non_req_args<kinds_t, args_t>));
                USING(rest_values_t, TYPENAME_T(tuple98_traits::types<rest_kinds_t>));

                rest_values_t rest = tuple98_traits::values<rest_kinds_t>::value();
                return impl(
                    detail::select_single<K1, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K2, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K3, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K4, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K5, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K6, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K7, args_t, rest_kinds_t>::select(args, rest).value
                );
            }
        };

        template <typename I, I impl, typename K1, typename K2, typename K3, typename K4, typename K5, typename K6, typename K7, typename K8>
        struct function_base<I, impl, K1, K2, K3, K4, K5, K6, K7, K8> {
        protected:
            USING(kinds_t, TEMPLATE_TUPLE(K1, K2, K3, K4, K5, K6, K7, K8));

            template <typename args_t>
            TYPENAME_T(type_traits::result_of<I>) apply_impl(args_t args) const {
                USING(rest_kinds_t, TYPENAME_T(detail::missing_non_req_args<kinds_t, args_t>));
                USING(rest_values_t, TYPENAME_T(tuple98_traits::types<rest_kinds_t>));

                rest_values_t rest = tuple98_traits::values<rest_kinds_t>::value();
                return impl(
                    detail::select_single<K1, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K2, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K3, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K4, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K5, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K6, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K7, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K8, args_t, rest_kinds_t>::select(args, rest).value
                );
            }
        };

        template <typename I, I impl, typename K1, typename K2, typename K3, typename K4, typename K5, typename K6, typename K7, typename K8, typename K9>
        struct function_base<I, impl, K1, K2, K3, K4, K5, K6, K7, K8, K9> {
        protected:
            USING(kinds_t, TEMPLATE_TUPLE(K1, K2, K3, K4, K5, K6, K7, K8, K9));

            template <typename args_t>
            TYPENAME_T(type_traits::result_of<I>) apply_impl(args_t args) const {
                USING(rest_kinds_t, TYPENAME_T(detail::missing_non_req_args<kinds_t, args_t>));
                USING(rest_values_t, TYPENAME_T(tuple98_traits::types<rest_kinds_t>));

                rest_values_t rest = tuple98_traits::values<rest_kinds_t>::value();
                return impl(
                    detail::select_single<K1, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K2, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K3, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K4, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K5, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K6, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K7, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K8, args_t, rest_kinds_t>::select(args, rest).value,
                    detail::select_single<K9, args_t, rest_kinds_t>::select(args, rest).value
                );
            }
        };
    }

    // named argument function type
    template <typename I, I impl, typename K1 = void, typename K2 = void, typename K3 = void, typename K4 = void, typename K5 = void, typename K6 = void, typename K7 = void, typename K8 = void, typename K9 = void, typename K10 = void,
        bool all_defaultable = detail::missing_req_args<TEMPLATE_TUPLE(K1, K2, K3, K4, K5, K6, K7, K8, K9, K10), TUPLE()>::empty
    >
    struct function;

    template <typename I, I impl, typename K1, typename K2, typename K3, typename K4, typename K5, typename K6, typename K7, typename K8, typename K9>
    struct function<I, impl, K1, K2, K3, K4, K5, K6, K7, K8, K9, void, false>
        : public detail::function_base<I, impl, K1, K2, K3, K4, K5, K6, K7, K8, K9, void>
    {
    private:
        USING(super, detail::function_base<I, impl, K1, K2, K3, K4, K5, K6, K7, K8, K9, void>);
        USING(kinds_t, TEMPLATE_TUPLE(K1, K2, K3, K4, K5, K6, K7, K8, K9));

    public:
        template <typename A1>
        __attribute__((always_inline))
        TYPENAME_T(detail::result_of_check<I, kinds_t, TEMPLATE_TUPLE(A1)>) operator()(A1 a1) const {
            return super::apply_impl(tuple98::make_tuple(a1));
        }

        template <typename A1, typename A2>
        __attribute__((always_inline))
        TYPENAME_T(detail::result_of_check<I, kinds_t, TEMPLATE_TUPLE(A1, A2)>) operator()(A1 a1, A2 a2) const {
            return super::apply_impl(tuple98::make_tuple(a1, a2));
        }

        template <typename A1, typename A2, typename A3>
        __attribute__((always_inline))
        TYPENAME_T(detail::result_of_check<I, kinds_t, TEMPLATE_TUPLE(A1, A2, A3)>) operator()(A1 a1, A2 a2, A3 a3) const {
            return super::apply_impl(tuple98::make_tuple(a1, a2, a3));
        }

        template <typename A1, typename A2, typename A3, typename A4>
        __attribute__((always_inline))
        TYPENAME_T(detail::result_of_check<I, kinds_t, TEMPLATE_TUPLE(A1, A2, A3, A4)>) operator()(A1 a1, A2 a2, A3 a3, A4 a4) const {
            return super::apply_impl(tuple98::make_tuple(a1, a2, a3, a4));
        }

        template <typename A1, typename A2, typename A3, typename A4, typename A5>
        __attribute__((always_inline))
        TYPENAME_T(detail::result_of_check<I, kinds_t, TEMPLATE_TUPLE(A1, A2, A3, A4, A5)>) operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const {
            return super::apply_impl(tuple98::make_tuple(a1, a2, a3, a4, a5));
        }

        template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        __attribute__((always_inline))
        TYPENAME_T(detail::result_of_check<I, kinds_t, TEMPLATE_TUPLE(A1, A2, A3, A4, A5, A6)>) operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const {
            return super::apply_impl(tuple98::make_tuple(a1, a2, a3, a4, a5, a6));
        }

        template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        __attribute__((always_inline))
        TYPENAME_T(detail::result_of_check<I, kinds_t, TEMPLATE_TUPLE(A1, A2, A3, A4, A5, A6, A7)>) operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const {
            return super::apply_impl(tuple98::make_tuple(a1, a2, a3, a4, a5, a6, a7));
        }

        template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        __attribute__((always_inline))
        TYPENAME_T(detail::result_of_check<I, kinds_t, TEMPLATE_TUPLE(A1, A2, A3, A4, A5, A6, A7, A8)>) operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const {
            return super::apply_impl(tuple98::make_tuple(a1, a2, a3, a4, a5, a6, a7, a8));
        }

        template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        __attribute__((always_inline))
        TYPENAME_T(detail::result_of_check<I, kinds_t, TEMPLATE_TUPLE(A1, A2, A3, A4, A5, A6, A7, A8, A9)>) operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const {
            return super::apply_impl(tuple98::make_tuple(a1, a2, a3, a4, a5, a6, a7, a8, a9));
        }
    };

    template <typename I, I impl, typename K1, typename K2, typename K3, typename K4, typename K5, typename K6, typename K7, typename K8, typename K9>
    struct function<I, impl, K1, K2, K3, K4, K5, K6, K7, K8, K9, void, true>
        : public function<I, impl, K1, K2, K3, K4, K5, K6, K7, K8, K9, void, false>
    {
    private:
        USING(super, detail::function_base<I, impl, K1, K2, K3, K4, K5, K6, K7, K8, K9, void>);
        USING(kinds_t, TUPLE());

    public:
        __attribute__((always_inline))
        TYPENAME_T(detail::result_of_check<I, kinds_t, TUPLE()>) operator()() const {
            return super::apply_impl(tuple98::make_tuple());
        }
    };
}
