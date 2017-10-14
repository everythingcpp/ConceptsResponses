#include <functional>
#include <vector>
#include <iostream>
#include <type_traits>

// Using concepts syntax of your choice:
template <typename T>
concept bool is_fwd_it = requires(T a, T b) {
    a != b;
    a == b;
    {++a} -> T&;
    *a;
};

template <typename F, typename T>
concept bool is_unary = requires(F f, T a) {
    f(a);
};

template <typename F, typename T>
concept bool is_compare = requires(F f, T a, T b) {
    {f(a,b)} -> bool;
};

// Reimplement std::min
template <typename T>
T my_min(const T& a, const T& b, is_compare<T> comp) {
    return comp(a,b) ? a : b;
}

// Reimplement std::for_each
template <typename Iter>
using iter_val_type = typename std::iterator_traits<Iter>::value_type;

auto my_for_each(is_fwd_it first, is_fwd_it last, is_unary<iter_val_type<is_fwd_it>> f) {
    for (; first != last; ++first) {
        f(*first);
    }
    return f;
}

// Test
int main(int argc, char* argv[]) {
    std::vector<int> v{1,2,3,4,5,6};
    my_for_each(v.begin(), v.end(), [](auto e){});
    //my_for_each(1, 3, [](auto e, auto f){});

    my_min(3,2, std::less<int>());
    //my_min(2,4, [](auto e){});
    //my_min(v, v, std::less<int>());
}

