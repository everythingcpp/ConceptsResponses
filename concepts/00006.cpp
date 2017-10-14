#include <functional>

template <class T>
concept bool LessThanComparable = requires (T const v) {
    { v < v } -> bool;
};

template <class F, class T>
concept bool BinaryPredicate = requires (F f, T const v) {
    { std::invoke(f, v, v) } -> bool;
};

template <LessThanComparable T>
T const& min(T const& a, T const& b) {
    return (a < b) ? a : b;
}

template <class T, BinaryPredicate<T> F>
T const& min(T const& a, T const& b, F f) {
    return std::invoke(f, a, b) ? a : b;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

template <class T, class... Args>
concept bool Constructible = requires(Args... args) {
    T(std::forward<Args>(args)...);
};

template <class T>
concept bool CopyConstructible = Constructible<T, T const&>;

template <class T, class From>
concept bool Assignable = requires(T t, From from) {
    { t = from } -> T&;
};

template <class T>
concept bool CopyAssignable = Assignable<T, T const&>;

template <class T>
concept bool Destructible = requires(T t) {
    { t.~T() } noexcept;
};

namespace adl_swap {
    using std::swap;
    template <class U>
    auto try_swap(U& a, U& b) -> decltype(swap(a,b));
}

template <class T>
concept bool Swappable = requires(T t) {
    adl_swap::try_swap(t,t);
};

template <class T>
concept bool Iterator =
    CopyConstructible<T> &&
    CopyAssignable<T> &&
    Destructible<T> &&
    Swappable<T> &&
    requires {
        typename std::iterator_traits<T>::value_type;
        typename std::iterator_traits<T>::difference_type;
        typename std::iterator_traits<T>::reference;
        typename std::iterator_traits<T>::pointer;
        typename std::iterator_traits<T>::iterator_category;
    };

template <class T>
concept bool EqualityComparable = requires (T const v) {
    { v == v } -> bool;
    { v != v } -> bool;
};

template <class T>
concept bool InputIterator = 
    Iterator<T> &&
    EqualityComparable<T> &&
    requires(T const iter) {
        { *iter } -> typename std::iterator_traits<T>::reference;
        { ++iter } -> T&;
        iter++;
    };

template <class F, class I>
concept bool IndirectInvocable = 
    CopyConstructible<F> && 
    requires(F f, I i) {
        std::invoke(f, *i);
    };

template <InputIterator I, IndirectInvocable<I> F>
F for_each(I first, I last, F f) {
    for (; first != last; ++first) {
        std::invoke(f, *first);
    }
    return f;
}

