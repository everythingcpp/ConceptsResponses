// Summary:
//  - concepts are ugly, it is hard to write common requirements without hacks
//  - using concepts is artificial and unintuitive
//      - as if it is shoehorning desired functionality into existed tool instead of making specialized language feature
//  - it is unintuitive which requirements need to be written for specific situation
//      - as if you need to write standard proposal-like "standardese"

#include <functional>
#include <iterator>

// Using concepts syntax of your choice:
namespace concepts {

// This is my preferred syntax, not supported unfortunately :(
// concept</*implicit typename if no type provided, can't be anonymous*/ T> PredicateResult = requires (T t) {
//    t? 1 : 1; // can be used in "if"
//    {(bool) t} // can direct initialize bool
//};


// Explicit boolean convertable
template<typename T>
concept bool PredicateResult = requires (T t) {
    t? 1 : 1; // can be used in "if"
    {(bool) t} // can direct initialize bool
};

// Variadic predicate
template<typename Pred, typename... Args>
concept bool Predicate = requires(Pred p, Args... args) {
    {p(args...)}->PredicateResult; // callable with args, result is PredicateResult
};

// Variadic callable
template<typename F, typename... Args>
concept bool Callable = requires(F p, Args... args) {
    {p(args...)}; // callable with args
};

// Provides operator<, so can be stored in map
template<typename T>
concept bool OrderedOnly = requires(const T a) {
    { a < a } -> PredicateResult;
};

template<typename T>
concept bool Value = std::is_copy_constructible_v<T>
   && std::is_copy_assignable_v<T>
   && std::is_move_constructible_v<T>
   && std::is_move_assignable_v<T>
   && std::is_destructible_v<T>;

template<Value Pred, typename... Args>
concept bool PredicateValue = Predicate<Pred, Args...>;

template<class T, class U>
concept bool Exactly = std::is_same<T, U>::value;

template<Value T>
concept bool Iterator = requires (T i) {
    typename std::iterator_traits<T>::iterator_category;
    { ++i } -> Exactly<T&>;
    { *i } -> Exactly<typename std::iterator_traits<T>::reference>;
    { *i } -> typename std::iterator_traits<T>::value_type;
};

template<Iterator It, typename Tag>
concept bool IteratorAtLeast = std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<It>::iterator_category>;

template<typename Tag, IteratorAtLeast<Tag> Start, Value End>
concept bool DenoteRange = requires(Start s, End e) {
    { s != e } -> PredicateResult;
    { s == e } -> PredicateResult;
};

template<Value F, typename Arg>
concept bool UnaryFunctionValue = Callable<F, Arg>;

}


// Reimplement std::min

constexpr const concepts::OrderedOnly& min(const concepts::OrderedOnly& a, const concepts::OrderedOnly& b) {
    return b < a ? b : a;
}

// Unfortunatly cannot use concept introducer with parameter
// packs and same type twice
// concepts::Predicate{Comparator, T, T}
// constexpr const T& min(const T& a, const T& b, Comparator comp) {
//    return comp(b, a) ? b : a;
// }

template<typename T>
constexpr const T& min(const T& a, const T& b, concepts::PredicateValue<T, T> comp) {
   return comp(b, a) ? b : a;
}

// Reimplement std::for_each

// Unfortunatly following code cannot be used:
// concepts::DenoteRange<std::input_iterator_tag>{InputIt, Sentinel}

namespace foo {
template<typename InputIt, typename Sentinel, concepts::UnaryFunctionValue<typename std::iterator_traits<InputIt>::reference> F>
requires concepts::DenoteRange<std::input_iterator_tag, InputIt, Sentinel>
F for_each( InputIt first, Sentinel last, F f ) {
    for(; first != last; ++first) {
        f(*first);
    }
    return std::move(f);
}
}

#include <vector>
int test51() {
    std::vector<int> a {1, 2, 3};
    std::vector<float> b {1, 2, 3};
    int res = 0;
    foo::for_each(a.begin(), a.end(), [&](const int& a){
        res += a;
    });
    return res;
}

// Testing

struct explicit_bool_conv {
    explicit operator bool() {
        return true;
    }
};

struct bool_conv {
    operator bool() {
        return true;
    }
};

struct target {};

struct target_comparator {
    explicit_bool_conv operator()(const target& t1, const target& t2) {
        return {};
    }
};

int test_min11() {
    return min(5, 3);
}

int test_min21() {
    return min(5., 1.);
}

int test_min31() {
    return min(5.f, 2.f);
}


int test_min1() {
    return min(5, 3, std::less<>{});
}

int test_min2() {
    return min(5., 1., std::less<>{});
}

int test_min3() {
    return min(5.f, 2.f, std::less<>{});
}

target test_min4() {
    return min(target{}, target{}, target_comparator{});
}


int test() {
    //return concepts::PredicateResult<bool_conv>;
    //return concepts::Predicate<decltype(ff::f), int, int>;
}

