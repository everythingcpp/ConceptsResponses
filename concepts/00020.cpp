// Using concepts syntax of your choice:
#include<functional>
// Reimplement std::min
template<typename T>
concept bool is_LTComparable = requires (T s) { s < s;};

template<is_LTComparable T>  // This feels right to me. "T is of of concept is_LTComparable"
T const & _min(T const & x, T const & y) {
    if (x < y)
      return x;
    return y;
}
// Reimplement std::for_each

template<typename Iter, typename Doable, typename Ignorable>
requires // This also feels right to me -- like a type-class witness! :D
        // The other notation feels wrong to me, though. 
        // Concepts should be used to describe TYPES, not VALUES. 
   requires (Iter it, Doable v) {v = *it; it++;}
   // I *wanted* to do this instead, btw: 
   // requires (Iter it) {Doable v = *it; it++;}
   // But the compiler yelled at me! 
void _for_each(Iter current, Iter end, std::function<Ignorable(Doable)> f) {
// I don't like how far away the function name is
// from the start if its declaration. :(
    while (current != end) {
        f(*current);
        current++;
    }
}

