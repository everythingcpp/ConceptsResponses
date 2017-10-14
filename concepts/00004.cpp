#include <iostream>
#include <utility>
#include <vector>

// Using concepts syntax of your choice:

// Reimplement std::min
template <typename T>
concept bool sortable = requires(T t) {
    t < t;
};

sortable min(const sortable & a, const sortable & b)
{
    return (a < b ? a : b);
}

// Reimplement std::for_each
template <typename F>
concept bool callable = requires(F f)
{
    f.operator();
};

template <typename T>
concept bool comparable = requires(T t)
{
    t != t;
    t == t;
};

template <comparable T>
concept bool iterable = requires(T t)
{
    t++;
    ++t;
    *t;
};

callable for_each(iterable begin, comparable end, callable&& f)
{
    while (begin != end)
    {
        f(*begin++);
    }
    return std::move(f);
}

struct S {};
int main()
{
    std::cout << min(2, 4) << '\n';
    //std::cout << min(S{}, S{});

    std::vector<int> vec = {1, 2, 3, 4};

    for_each(vec.begin(), vec.end(), [](int i) {
        std::cout << i << '\n';
    });
    return 0;
}

