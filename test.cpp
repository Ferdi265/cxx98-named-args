#include <cassert>

#include "tuple98.h"
#include "tuple_traits.h"

int main() {
    TUPLE(int, int, float) nice = tuple98::make_tuple(1, 2, 3.14159f);

    size_t pi1 = tuple98::get<2>(nice);
    size_t pi2 = tuple98::get<float>(nice);
    assert(pi1 == pi2);

    size_t v1 = tuple_traits::count<TUPLE(int, int, float), int>::value;
    assert(v1 == 2);
}
