/* 
everythingcpp@gmail.com

Implement:
    std::min()
    std::for_each()
   ...with Concepts.

    Note: I have chosen to /not/ cheat and peek at Stepanov & McJones. My
    suspicion is that my implementation will be a bit naieve.

    Note: I will not implement every variation of these, just "the basics".
*/

#include <iterator>
#include <type_traits>

template <typename T, typename U>
concept bool LessThanEqualityComparable = requires(T a, U b) 
{
 { a <= b } -> bool;
};

// I sense there's more nuance to this:
template <typename T, typename U>
requires LessThanEqualityComparable<T, U>
auto min(const T& a, const U& b)
{
 return a <= b ? a : b;
}

////

template <typename IterT>
concept bool ForwardIterator = 
    std::is_same_v<std::forward_iterator_tag, typename IterT::iterator_category>;

template <typename ValueT, typename FnT>
concept bool UnaryCallable = requires(ValueT a, FnT f)
{
 { f(a) };
};

template <typename IteratorT, typename CallableT>
requires ForwardIterator<IteratorT> && 
         UnaryCallable<typename IteratorT::value_type, CallableT>
auto for_each(IteratorT b, IteratorT e, CallableT f)
{
 for(; e != b; ++b)
  f(*b);

 return f;
}

////

#include <iostream>
#include <forward_list>

int main()
{
 using namespace std;

 // min():
 int x = 0, y = 1;

 cout << min(x, y) << '\n';
 cout << min(9, 10) << '\n';

 // ...also ok, std::min() need not be limited to numeric types:
 string a("hello"), b("world");
 cout << min(a, b) << '\n';

 // ...and that implies we should allow:
 cout << min("hi", "there") << '\n';

 // for_each():
 forward_list<int> xs { 1, 2, 3, 4, 5 };
 for_each(begin(xs), end(xs), 
          [](const auto& x) { cout << x << '\n'; }); 
}

