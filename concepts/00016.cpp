#include <algorithm>
#include <type_traits>
#include <initializer_list>
#include <iostream>
#include <string>

template<typename T>
concept bool LessThanComparable = requires(const T& a) {
    { a < a } -> bool
};

template<typename Param, typename Comp>
concept bool ComparableVia = requires(const Comp& comp, const Param& a) {
    { comp(a, a) } -> bool
};

template<typename T>
concept bool CopyConstructible = requires(T obj) {
    { T { obj } } ->  T
};

template<typename T>
requires LessThanComparable<T>
constexpr const T& min(const T& a, const T& b) { return b < a ? b : a; }

template<typename T, typename Comp>
requires ComparableVia<T, Comp> && CopyConstructible<Comp> && CopyConstructible<T>
constexpr const T& min(const T& a, const T& b, Comp compare)
{
    return compare(b, a) ? b : a;
}

template<typename T>
requires CopyConstructible<T> && LessThanComparable<T>
constexpr T min(std::initializer_list<T> values)
{
    return *std::min_element(values.begin(), values.end());
}

template<typename T, typename Comp>
requires CopyConstructible<T> && ComparableVia<T, Comp> && CopyConstructible<Comp>
constexpr T min(std::initializer_list<T> values, Comp compare)
{
    return *std::min_element(values.begin(), values.end(), compare);
}


template<typename T>
concept bool InputIterator = requires(T it) {
    { ++it } -> T
    { *it } /* This should return a value, but I don't know the type just from T */
    /* { it->sometn() }  I should be able to reasonably use operator-> on it */
};

template<typename T, typename Param>
concept bool UnaryFunction = requires(const T& func) {
    func(std::declval<Param>());
};

template<typename IT, typename Func>
requires InputIterator<IT> && UnaryFunction<Func, decltype(*std::declval<IT>())>
Func for_each(IT first, IT last, Func func)
{
    for(auto it = first; it != last; ++it) {
        func(*it);
    }
    return func;
}

int main() {
    auto numbers = { 1, 2, 3, 4, 5 };
    std::cout << "min(numbers): " << min(numbers);
    std::cout << "\nmin(4, 3): " << min(4, 3);
    std::cout << "\nnumbers:\n";

    // gcc 7.2 segfaulted for some reason at the next statement
    for_each(numbers.begin(), numbers.end(), [](auto n) {
        std::cout << n << '\n';
    });

    // this fails because int is not an InputIterator
    // for_each(1, 3, [](auto n) { std::cout << n << '\n'; });
}

