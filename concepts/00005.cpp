#include <cstdio>
#include <vector>
// Using concepts syntax of your choice:

// Reimplement std::min
template<typename T>
concept bool LessThanComparable = requires(T t1, T t2)
{
    t1 < t2;
};

template<LessThanComparable T>
const T &min(const T &left, const T &right)
{
    return (left < right) ? left : right;
}

// Reimplement std::for_each
template<typename T>
concept bool Iterable = requires(T t, T t2)
{
    ++t;
    *t;
    t != t2;
};

template<typename T, Iterable V>
concept bool IteratorConsumable = requires(T t, V v)
{
    t(*v);
};

template<Iterable T, IteratorConsumable<T> V>
void for_each(T begin, T end, const V &func)
{
    for(;begin != end; ++begin)
    {
        func(*begin);
    }
}

int main()
{
    printf("%d\n", min(8,7));
    /*Uncomment for error:
    struct NonComparable {int i, j;};
    printf("%d\n", min(NonComparable{1,2}, NonComparable{3,4}));
    */
    std::vector<int> vec = {1,2,3,4,5};   
    for_each(vec.begin(), vec.end(), [](int i)
    {
        printf("%d\n", i);
    });

    //Uncomment for error: for_each(vec.begin(), vec.end(), "wat");
}

