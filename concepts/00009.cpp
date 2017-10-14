#include <type_traits>
#include <iostream>

template <typename T>
concept bool is_comparable = requires(T t) {
    t < t;
};

constexpr auto min(is_comparable a, is_comparable b)
    -> decltype(a)
{
    return (b < a) ? b : a;
}

template <typename C, typename T>
concept bool is_comparator = requires(C comp, T t) {
    comp(t, t);
    // Not sure how (if possible) to verify that comp is a strict weak order as per:
    // http://en.cppreference.com/w/cpp/concept/Compare
};

constexpr auto min(is_comparable a, is_comparable b, is_comparator<decltype(a)> comp)
    -> decltype(a)
{
    return (comp(b, a)) ? b : a;
}

constexpr is_comparable min_v2(is_comparable a, is_comparable b, is_comparator<decltype(a)> comp)
{
    return (comp(b, a)) ? b : a;
}

constexpr is_comparable min_v3(is_comparable a, is_comparable b, is_comparator<is_comparable> comp)
{
    return (comp(b, a)) ? b : a;
}

int main()
{
    std::cout << min(1, 2) << std::endl;
    std::cout << min(3.5f, 1.0f) << std::endl;

    return min(1, 2, [](int a, int b) { return a < b;});
    
    // In the below usages, why does the first pass and the second fail?
    //return min_v2(1, 2, [](int a, int b) { return a < b;}); SUCCESS
    //return min_v3(1, 2, [](int a, int b) { return a < b;}); FAIL
}

