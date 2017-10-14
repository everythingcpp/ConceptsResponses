// Using concepts syntax of your choice:
#include <initializer_list>
namespace std
{
    // Reimplement std::min
    template <class T>
    concept bool LessOperComparable() 
    { 
            return requires(T a, T b) { {a < b} -> bool; };
    }

    template <class T, class U>
    concept bool MinFuntComparable() 
    { 
        return requires(T a, T b) { {U(a,  b)} -> bool; };
    }

    template< class T > requires LessOperComparable
    constexpr const T& min( const T& a, const T& b )
    {
        if(a < b)
            return a;
        return b;
    }

    template< class T > requires LessOperComparable
    constexpr const T min( std::initializer_list<T> ilist )
    {
        auto a = *ilist.begin();
        for(const auto& val : ilist)
        {
            if(val < a)
            {
                a = val;
            }
        }
        return a;
    }

    template< class T, class U > requires MinFuntComparable
    constexpr const T& min( const T& a, const T& b, U fun )
    {
        if(fun(a, b))
            return a;
        return b;
    }

    template< class T, class U > requires MinFuntComparable
    constexpr const T min( std::initializer_list<T> ilist, U fun )
    {
        auto a = *ilist.begin();
        for(const auto& val : ilist)
        {
            if(fun(val, a))
            {
                a = val;
            }
        }
        return a;
    }

    // Reimplement std::for_each
    template <class InputIt, class UnaryFunction>
    concept bool ValidForEach() 
    {
        return requires(InputIt a, InputIt b) { {UnaryFunction(*a) } -> bool; {++a} -> InputIt; {a != b} -> bool};
    }

    template< class InputIt, class UnaryFunction > requires ValidForEach
    void for_each( InputIt first, InputIt last, UnaryFunction f )
    {
        for (; first != last; ++first) 
        {
            f(*first);
        }
    }
}    
