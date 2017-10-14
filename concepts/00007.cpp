#include <iostream>
#include <iterator>
#include <vector>
#include <type_traits>

// Reimplement std::min
template<typename Comparable>
concept bool is_comparable = requires(Comparable c) {
    //I suppose you cannot expect primitive to implement operator
    //You can expect to have stupid constraints
    c < c;
};

const decltype(auto)& min( const is_comparable& a, const is_comparable& b ) {
    return (b < a) ? b : a;
}

// Reimplement std::for_each
template<typename Function>
concept bool is_function = requires(Function f) {
    std::is_function<Function>::value;
};
template<typename Iterable>
concept bool is_iterable = requires(Iterable i) {
    std::advance(i, 1);
};

//It would be the best to be able to use Abstract classes as constraints
//Or just classes so that compiler would identify if passed argument to template
//is inherited from this class (or just a class). E.g:
//void for_each(std::input_iterator_tag begin, std::input_iterator_tag end, is_function fn) {
decltype(auto) for_each(is_iterable begin, is_iterable end, is_function fn) {
    while (begin != end) {
        fn (*begin);
        std::advance(begin, 1);
    }
    return fn;
}

int main() {
    int left = 5;
    long int right = 6;
    std::cout << "min:" << min(left, left) << "\n";

    std::vector<int> ints{1, 2, 3, 4};
    for_each(ints.begin(), ints.end(), [](const int& n) { std::cout << " " << n; });
}

