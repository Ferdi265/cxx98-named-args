#pragma once

#include <cstddef>

// macros to reduce the amount of typename bloat
#define TYPE_T(...) __VA_ARGS__::type
#define TYPENAME_T(...) typename __VA_ARGS__::type
#define USING(T, ...) typedef TYPENAME_T(::tuple98::ident<__VA_ARGS__>) T

namespace tuple98 {
    // identity type trait for bloat reduction macros
    template <typename T>
    struct ident {
        typedef T type;
    };

    // empty tuple
    struct nil {
        nil() {}
    };

    // nonempty tuple
    template <typename T, typename R = nil>
    struct cons {
        T first;
        R rest;

        cons() {}
        cons(const cons& t) : first(t.first), rest(t.rest) {}
        cons(T t, R r) : first(t), rest(r) {}
    };

    // convenience tuple type trait
    template <typename T1 = void, typename T2 = void, typename T3 = void, typename T4 = void, typename T5 = void, typename T6 = void, typename T7 = void, typename T8 = void, typename T9 = void, typename T10 = void>
    struct tuple;

    #define TUPLE(...) TYPE_T(::tuple98::tuple<__VA_ARGS__>)
    #define TEMPLATE_TUPLE(...) TYPENAME_T(::tuple98::tuple<__VA_ARGS__>)

    template <>
    struct tuple<> {
        typedef nil type;
    };

    template <typename T1>
    struct tuple<T1> {
        USING(type, cons<T1, nil>);
    };

    template <typename T1, typename T2>
    struct tuple<T1, T2> {
        USING(type, cons<T1, TEMPLATE_TUPLE(T2)>);
    };

    template <typename T1, typename T2, typename T3>
    struct tuple<T1, T2, T3> {
        USING(type, cons<T1, TEMPLATE_TUPLE(T2, T3)>);
    };

    template <typename T1, typename T2, typename T3, typename T4>
    struct tuple<T1, T2, T3, T4> {
        USING(type, cons<T1, TEMPLATE_TUPLE(T2, T3, T4)>);
    };

    template <typename T1, typename T2, typename T3, typename T4, typename T5>
    struct tuple<T1, T2, T3, T4, T5> {
        USING(type, cons<T1, TEMPLATE_TUPLE(T2, T3, T4, T5)>);
    };

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    struct tuple<T1, T2, T3, T4, T5, T6> {
        USING(type, cons<T1, TEMPLATE_TUPLE(T2, T3, T4, T5, T6)>);
    };

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    struct tuple<T1, T2, T3, T4, T5, T6, T7> {
        USING(type, cons<T1, TEMPLATE_TUPLE(T2, T3, T4, T5, T6, T7)>);
    };

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    struct tuple<T1, T2, T3, T4, T5, T6, T7, T8> {
        USING(type, cons<T1, TEMPLATE_TUPLE(T2, T3, T4, T5, T6, T7, T8)>);
    };

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
    struct tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9> {
        USING(type, cons<T1, TEMPLATE_TUPLE(T2, T3, T4, T5, T6, T7, T8, T9)>);
    };

    namespace detail {
        template <typename T>
        struct ident {
            typedef T type;
        };

        // get the value of the first element of type U in tuple T
        template <typename T, typename U>
        struct get;

        template <typename T, typename R>
        struct get<cons<T, R>, T> {
            static T& get_value(cons<T, R>& t) {
                return t.first;
            }

            static const T& get_value(const cons<T, R>& t) {
                return t.first;
            }
        };

        template <typename U, typename R, typename T>
        struct get<cons<U, R>, T> {
            static T& get_value(cons<U, R>& t) {
                return get<R, T>::get_value(t.rest);
            }

            static const T& get_value(const cons<U, R>& t) {
                return get<R, T>::get_value(t.rest);
            }
        };

        // get the type and value of the Ith element of tuple T
        template <typename T, size_t I>
        struct nth;

        template <typename T, typename R>
        struct nth<cons<T, R>, 0> {
            USING(type, T);

            static T& nth_value(cons<T, R>& t) {
                return t.first;
            }

            static const T& nth_value(const cons<T, R>& t) {
                return t.first;
            }
        };

        template <typename U, typename R, size_t I>
        struct nth<cons<U, R>, I> {
            USING(type, TYPENAME_T(nth<R, I - 1>));

            static type& nth_value(cons<U, R>& t) {
                return nth<R, I - 1>::nth_value(t.rest);
            }

            static const type& nth_value(const cons<U, R>& t) {
                return nth<R, I - 1>::nth_value(t.rest);
            }
        };
    }

    // actually useful tuple constructor
    TUPLE() make_tuple() {
        return nil();
    }

    template <typename T1>
    TEMPLATE_TUPLE(T1) make_tuple(T1 t1) {
        return TEMPLATE_TUPLE(T1)(t1, make_tuple());
    }

    template <typename T1, typename T2>
    TEMPLATE_TUPLE(T1, T2) make_tuple(T1 t1, T2 t2) {
        return TEMPLATE_TUPLE(T1, T2)(t1, make_tuple(t2));
    }

    template <typename T1, typename T2, typename T3>
    TEMPLATE_TUPLE(T1, T2, T3) make_tuple(T1 t1, T2 t2, T3 t3) {
        return TEMPLATE_TUPLE(T1, T2, T3)(t1, make_tuple(t2, t3));
    }

    template <typename T1, typename T2, typename T3, typename T4>
    TEMPLATE_TUPLE(T1, T2, T3, T4) make_tuple(T1 t1, T2 t2, T3 t3, T4 t4) {
        return TEMPLATE_TUPLE(T1, T2, T3, T4)(t1, make_tuple(t2, t3, t4));
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5>
    TEMPLATE_TUPLE(T1, T2, T3, T4, T5) make_tuple(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) {
        return TEMPLATE_TUPLE(T1, T2, T3, T4, T5)(t1, make_tuple(t2, t3, t4, t5));
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    TEMPLATE_TUPLE(T1, T2, T3, T4, T5, T6) make_tuple(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6) {
        return TEMPLATE_TUPLE(T1, T2, T3, T4, T5, T6)(t1, make_tuple(t2, t3, t4, t5, t6));
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    TEMPLATE_TUPLE(T1, T2, T3, T4, T5, T6, T7) make_tuple(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7) {
        return TEMPLATE_TUPLE(T1, T2, T3, T4, T5, T6, T7)(t1, make_tuple(t2, t3, t4, t5, t6, t7));
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    TEMPLATE_TUPLE(T1, T2, T3, T4, T5, T6, T7, T8) make_tuple(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8) {
        return TEMPLATE_TUPLE(T1, T2, T3, T4, T5, T6, T7, T8)(t1, make_tuple(t2, t3, t4, t5, t6, t7, t8));
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
    TEMPLATE_TUPLE(T1, T2, T3, T4, T5, T6, T7, T8, T9) make_tuple(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9) {
        return TEMPLATE_TUPLE(T1, T2, T3, T4, T5, T6, T7, T8, T9)(t1, make_tuple(t2, t3, t4, t5, t6, t7, t8, t9));
    }

    // tuple accessor by type
    template <typename T, typename Tuple>
    T& get(Tuple& t) {
        return detail::get<Tuple, T>::get_value(t);
    }

    template <typename T, typename Tuple>
    const T& get(const Tuple& t) {
        return detail::get<Tuple, T>::get_value(t);
    }

    // tuple accessor by index
    template <size_t I, typename Tuple>
    TYPENAME_T(detail::nth<Tuple, I>)& get(Tuple& t) {
        return detail::nth<Tuple, I>::nth_value(t);
    }

    template <size_t I, typename Tuple>
    const TYPENAME_T(detail::nth<Tuple, I>)& get(const Tuple& t) {
        return detail::nth<Tuple, I>::nth_value(t);
    }
}
