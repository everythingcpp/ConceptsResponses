// Using concepts syntax of your choice:

// Reimplement std::min
template<class T>
concept bool LessThanComparable = requires(const T& v) {
    {v < v} -> bool;
};


template<class T>
    requires LessThanComparable<T>
const T& min(const T& lhs, const T& rhs) {
    if(rhs < lhs) return rhs;
    return lhs;
}

// Reimplement std::for_each
#include <type_traits>
#include <iterator>

template<class T>
concept bool Iterator =
    std::is_copy_constructible_v<T> &&
    std::is_copy_assignable_v<T> &&
    std::is_destructible_v<T> &&
    std::is_swappable_v<T> &&

requires(T it) {
    typename std::iterator_traits<T>::value_type;
    typename std::iterator_traits<T>::difference_type;
    typename std::iterator_traits<T>::reference;
    typename std::iterator_traits<T>::pointer; 
    typename std::iterator_traits<T>::iterator_category;

    {++it} -> T&;
    {*it}
};


template<class T>
concept bool InputIterator =
    Iterator<T> &&
requires(T it, const T cit) {
    {cit == cit} -> bool;
    {cit != cit} -> bool;

    {*cit} -> typename std::iterator_traits<T>::reference;
    {*it++} -> typename std::iterator_traits<T>::value_type;
};

template<class It, class UnaryFunction>
    requires InputIterator<It> &&
    requires(It it, UnaryFunction f) { {f(*it)}; }
UnaryFunction for_each(It begin, It end, UnaryFunction f) 
{
    for(;begin != end; ++begin) {
        f(*begin);
    }
    return f;
}

// Test :-)
#include <iostream>
int main()
{
    auto list = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for_each(list.begin(), list.end(),
        [](int v){ std::cout << min(5, v) << '\n'; }
    );
}

