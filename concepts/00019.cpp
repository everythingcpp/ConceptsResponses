#include <vector>

// Using concepts syntax of your choice:
// Reimplement std::min
template<typename T>
concept bool less_comparable = requires (T t) {
    t < t; // Supports less-than operator
};

template <less_comparable T>
T min(T left, T right) {
    return left < right ? left : right;
};

// Reimplement std::for_each
/*
template<typename T>
concept bool pre_incrementable = requires(T t) {
    ++t;
};

template<typename T>
concept bool not_equal_comparable = requires(T t) {
    t != t;
};

template<typename T>
concept bool dereferencable = requires(T t) {
    *t;
};

// Not really sure how to "combind" concepts?
// This really should be possible if it isn't.
template<typename T>
concept bool fwd_iterator =
requires pre_incrementable<T>, not_equal_compable<T>, deferencable<T>;
*/

// Alternatively define a fwd_iterator by how it's used?
// Why not do the simple things simply.
template<typename T>
concept bool fwd_iterator = requires(T it) {
    *it; // Can be dereferenced
    ++it; // Can be pre_incremented
    it != it; // Can be compared to it's own type
};

// I tried to write a generic concept of being callable_with
// But as seen below, I couldn't figure out how to get the
// deferenced value into the actual for_each requirements
template<typename F, class ... A>
concept bool callable_with = requires(F f, A ... a) {
    f(a...);
};

template<fwd_iterator IT, class OP>
// Attempt one... clearly wouldn't work.
//requires callable_with<OP, *IT>

// Attempt two... after I realized even if this did work, it would work
// For more basic more iterators which don't have a templated `type`.
//requires callable_with<OP, typename IT::type>

// Attempt Three... after some googling, but I realized IT might not
// always be a pointer, while this compiles sometimes it isn't correct.
//requires callable_with<OP, typename std::remove_pointer<IT>::type>

// At this point I consider giving up, but then I realized I could
// do an inline requires like you showed in the video.  I don't want
// to do one, but it might just work.
requires
    requires (IT it, OP op) { op(*it); }
OP for_each(IT start, IT end, OP f) {
    while(start != end) {
        f(*start);
        ++start;
    }
    return f;
}


// Just some test cases for me.
int main()
{
    min(4, 7);

    auto add_one = [](int &v) { v += 1; };

    std::vector<int> test_vector{4, 5, 6};
    for_each(test_vector.begin(), test_vector.end(), add_one);

    int test_array[3] = {4, 5, 6};
    for_each(test_array, test_array + 3, add_one);
}

