#include <type_traits>
#include <initializer_list>
#include <iterator>
//#include <execution> not available yet

// Using concepts syntax of your choice:

// Reimplement std::min
template <typename T>
concept bool LessThanComparable = requires (T x) {
    { x < x } -> bool;
};
template <typename T>
concept bool CopyConstructible =
    std::is_copy_constructible<T>();
template <typename C, typename T>
concept bool Compare = requires (C x, T y) {
    { x(y, y) } -> bool;
};

// 1
template <LessThanComparable T>
constexpr const T& min(const T& x, const T& y) {
    return (y < x) ? y : x;
}
// 2
template <typename T, typename C>
requires Compare<C,T>
constexpr const T& min(const T& x, const T& y, C comp) {
    return comp(y, x) ? y : x;
}
// 3
template <LessThanComparable T>
requires CopyConstructible<T>
constexpr T min(std::initializer_list<T> il) {
    if (il.size() == 0) return T();
    auto it = il.begin();
    auto min_it = it;
    for (; it != il.end(); it++) {
        if (*it < *min_it) min_it = it;
    }
    return *min_it;
}
// 4
template <CopyConstructible T, typename C>
requires Compare<C,T>
constexpr T min(std::initializer_list<T> il, C comp) {
    if (il.size() == 0) return T();
    auto it = il.begin();
    auto min_it = it;
    for (; it != il.end(); it++) {
        if (comp(*it, *min_it)) min_it = it;
    }
    return *min_it;
}

// Reimplement std::for_each
template <typename T>
concept bool MoveConstructible =
    std::is_move_constructible<T>() ||
    CopyConstructible<T>;
template <typename T>
concept bool DefaultConstructible =
    std::is_default_constructible<T>();
template <typename T>
concept bool EqualityComparable = requires (T x) {
    { x == x } -> bool;
};
template <typename T>
concept bool Iterator = requires (T x) {
    *x;
    { ++x } -> T&;
};
template <typename T>
concept bool InputIterator = requires (T x) {
    requires Iterator<T>;
    requires EqualityComparable<T>;
    { x != x } -> bool;
    { *x } -> typename std::iterator_traits<T>::reference;
    x++;
    { *x++ } -> typename std::iterator_traits<T>::value_type;
};
template <typename T>
concept bool OutputIterator = requires (T x) {
    requires Iterator<T>;
    { x++ } -> const T&;
};
template <typename T, typename U>
concept bool Same = std::is_same<T,U>();
template <typename T>
concept bool ForwardIterator = 
    InputIterator<T> && 
    DefaultConstructible<T> &&
    ((Same<typename std::iterator_traits<T>::reference, T&> && 
      OutputIterator<T>) ||
     Same<typename std::iterator_traits<T>::reference, const T&>
    ) && requires (T x) {
    { x++ } -> T;
};
// Parallelism isn't part of libstdc++ yet
template <typename> struct is_execution_policy {
    bool operator()() { return true; }
};
template <typename T>
concept bool ExecutionPolicy = 
    is_execution_policy<std::decay_t<T>>();

// 1
template <MoveConstructible UnaryFunction>
UnaryFunction for_each(InputIterator first, InputIterator last,
                       UnaryFunction f) {
    for (; first != last; first++) {
        f(*first);
    }
    return f;
}
// 2
template <CopyConstructible UnaryFunction>
void for_each(ExecutionPolicy&& policy, ForwardIterator first,
              ForwardIterator last, UnaryFunction f) {
    // Who needs parallelism anyway
    for (; first != last; first++) {
        f(*first);
    }
}

