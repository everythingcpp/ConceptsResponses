#include <initializer_list>
#include <vector>
#include <type_traits>
#include <algorithm>

// Reimplement std::min

// 1.
// template< class T > 
// constexpr const T& min( const T& a, const T& b );

// 2.
// template< class T, class Compare >
// constexpr const T& min( const T& a, const T& b, Compare comp );

// 3.
// template< class T >
// constexpr T min( std::initializer_list<T> ilist );

// 4.
// template< class T, class Compare >
// constexpr T min( std::initializer_list<T> ilist, Compare comp );

template <typename T>
concept bool LessThanComparable = requires(T a, T b) {
    { a < b } -> bool;
};

template <typename F, typename T>
concept bool Compare = requires(T a, T b, F f) {
    { f(a, b) } -> bool;
};

template <typename T>
concept bool CopyConstructible = std::is_copy_constructible_v<T>;

// 1.
LessThanComparable { T }
constexpr auto & min(const T & a, const T & b) {
    return a < b ? a : b;
}

// 2.
template <typename T>
// Wish I could write 
//   Compare<T> { F }
// here and use F instead of Compare<T> below
// for consistency's sake
// Also I find it confusing that Compare<T> works by using T as the second template parameter of the Compare concept
// I wrote template <typename T, typename F> instead of template <typename F, typename T> at first
constexpr auto & min(const T & a, const T & b, Compare<T> comp) {
    return comp(a, b) ? a : b;
}

// 3.
template <typename T>
requires LessThanComparable<T> && CopyConstructible<T>
// Wish I could write
//   LessThanComparable { T } && CopyConstructible { T }
// instead of the 2 lines the above
// for consistency's sake
constexpr T min(std::initializer_list<T> ilist) {
    return *std::min_element(ilist.begin(), ilist.end());
}

// 4.
template <typename T>
requires CopyConstructible<T>
// Same remark as 2.
constexpr T min(std::initializer_list<T> ilist, Compare<T> comp) {
    return *std::min_element(ilist.begin(), ilist.end(), comp);
}

template <typename T>
struct Uncopyable {
    Uncopyable(T t): val_ { t } { }

    constexpr auto operator<(const Uncopyable & other) const {
        return val_ < other.val_;
    }

    constexpr auto operator>(const Uncopyable & other) const {
        return val_ > other.val_;
    }

    Uncopyable(const Uncopyable &) = delete;
    Uncopyable & operator=(const Uncopyable &) = delete;

private:
    T val_;
};

// NOTE: scoping this in `min_tests` results in a compiler segfault
constexpr auto greater = [](auto a, auto b) { return a > b; };

void min_tests() {
    struct NonComparable { };
    
    ::min(1, 2);
    // ::min(NonComparable{}, NonComparable{}); // Error

    // ::min(1, 2, [](auto a, auto b) { return a > b; }); // Compiler Segfault
    ::min(1, 2, greater);
    // ::min(1, 2, NonComparable{}); // Error
    // ::min(1, 2, [](auto a) { return true; }); // Error

    ::min({ 1, 2, 3, 4 });
    // ::min({ Uncopyable { 1 }, Uncopyable { 2 }, Uncopyable { 2 }, Uncopyable { 2 } }); // Error
    // ::min({ NonComparable{}, NonComparable{}, NonComparable{}, NonComparable{} }); // Error

    // ::min({ 1, 2, 3, 4 }, [](auto a, auto b) { return a > b; }); // Compiler Segfault
    ::min({ 1, 2, 3, 4 }, greater);
    // ::min({ Uncopyable { 1 }, Uncopyable { 2 }, Uncopyable { 2 }, Uncopyable { 2 } }, greater); // Error
    // ::min({ 1, 2, 3, 4 }, NonComparable{}); // Error
    // ::min({ 1, 2, 3, 4 }, [](auto a) { return true; }); // Error
}

//
//
//

// Reimplement std::for_each

// 1.
// template< class InputIt, class UnaryFunction >
// UnaryFunction for_each( InputIt first, InputIt last, UnaryFunction f );

template <typename It>
concept bool InputIterator = requires(It it) {
    // That might be cheating a bit
    // but I didn't really feel like implementing all the InputIterator requirements found here
    // http://en.cppreference.com/w/cpp/concept/InputIterator
    { typename std::iterator_traits<It>::iterator_category { } } -> std::input_iterator_tag;
};

template <typename F, typename T>
concept bool UnaryFunction = requires(T t, F f) {
    { f(t) } -> void;
};

InputIterator { It }
// Wish I could write 
//  InputIterator { It } && UnaryFunction<typename std::iterator_traits<It>::value_type> { F }
// and use `F` for the last parameter below
auto for_each(It first, It last, UnaryFunction<typename std::iterator_traits<It>::value_type> f) {
    for (; first != last; ++first) {
        f(*first);
    }
    return f;
}

// NOTE: scoping the generic lambdas below in `for_each_tests` results in a compiler segfault
constexpr auto do_nothing = [](auto) { };
constexpr auto increment = [](auto & n) { ++n; };

void for_each_tests() {
    auto nums = { 1, 2, 3, 42, 1337 };
    auto mutable_nums = std::vector { 1, 2, 3, 42, 1337 };
    int raw_array_of_nums[] = { 1, 2, 3, 42, 1337 };
    constexpr auto raw_array_size = sizeof(raw_array_of_nums) / sizeof(*raw_array_of_nums);

    // ::for_each(nums.begin(), nums.end(), [](auto num) { (void)num; }); // Compiler Segfault
    ::for_each(nums.begin(), nums.end(), do_nothing);
    ::for_each(mutable_nums.begin(), mutable_nums.end(), do_nothing);
    ::for_each(raw_array_of_nums, raw_array_of_nums + raw_array_size, do_nothing);
    // ::for_each(0, 1, do_nothing); // Error
    ::for_each(mutable_nums.begin(), mutable_nums.end(), increment);
    ::for_each(raw_array_of_nums, raw_array_of_nums + raw_array_size, increment);
    // ::for_each(mutable_nums.begin(), mutable_nums.end(), [](auto, auto){}); // Error
}

