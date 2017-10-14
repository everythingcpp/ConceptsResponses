// Using concepts syntax of your choice:

// Reimplement std::min

template<typename T>
concept bool Ordered = requires (const T& a, const T& b) {
    a < b;
    a > b;
    a <= b;
    a >= b;
};

template<Ordered T>
constexpr const T& min(const T& a, const T& b) {
    if(b < a) {
        return b;
    } else {
        return a;
    }
}

// Reimplement std::for_each

#include <utility>
#include <iterator>
#include <type_traits>

template<typename S>
concept bool InputSequence = requires (S seq) {
    std::begin(seq);
    std::end(seq);
    ++std::declval<decltype(std::begin(seq))&>();
    *std::begin(seq);
    std::begin(seq) != std::end(seq);
};

template<typename Sequence>
using SequenceIteratorDereferenced = decltype(*std::begin(std::declval<Sequence>()));

template<typename InSeq, typename UnaryFunction>
requires InputSequence<InSeq&&>
    && std::is_invocable_v<UnaryFunction, SequenceIteratorDereferenced<InSeq&&>>
UnaryFunction for_each(InSeq&& seq, UnaryFunction f) {
    for(auto&& element : seq) {
        f(std::forward<decltype(element)>(element));
    }
    return std::move(f);
}

