// Type your code here, or load an example.
template<typename T>
concept bool has_less_than =
requires (T a) { a < a; };

constexpr const has_less_than &min(const has_less_than &a, const has_less_than &b) {
    return a < b ? a : b;
}

void run_min() {
   min(5, 7);
   min('a', 'z');
   min(4.0, -3.0);
}

template<typename T>
concept bool iterable =
requires (T a) { *a; a++; a < a; };

template<typename T, typename U>
concept bool callable =
requires (T a, U b) { a(b); };

// "error: 'callable' is not a type"
// But hey, callable looks like iterable and has_less_than
void for_each(const iterable &begin, const iterable &end, callable func) {
    while (begin < end) {
        func(*begin++);
    }
}

