// Using concepts syntax of your choice:

#include <type_traits>
#include <initializer_list>
#include <functional>

template<typename T>
concept bool Ordered =
	requires(const T val1, const T val2) {
		val1 < val2;
	};

template<typename T>
concept bool Comparable =
	requires(const T val1, const T val2) {
		 { val1 == val2 } -> bool;
		 { val1 != val2 } -> bool;
	};

template<typename T>
concept bool CopyConstructible = 
	requires (const T val) { T(val); };

template<typename Func, typename T>
concept bool Comparator =
	requires (Func f, const T lhs, const T rhs)
	{
		{ f(lhs, rhs) } -> bool;
	};

template<typename Func, typename... Ts>
concept bool Callable =
	requires (Func f, Ts... ts)
	{
		f(ts...);
	};

// this is not fully specified iterator concept
// it has enough constraints for for_each
template<typename T>
concept bool Iterator
	= CopyConstructible<T> &&
	Comparable<T> &&
	 requires(T it)
	{
		++it;
		*it;
	};

template<Iterator T>
using value_type_t = std::decay_t<decltype(*std::declval<T>())>;


// Reimplement std::min
template<Ordered T>
constexpr 
const T& min (const T& lhs, const T& rhs)
{
	return lhs < rhs ? lhs : rhs;
}

template<typename T, typename C>
	requires CopyConstructible<T> 
			&& Comparator<C, T>
			&& CopyConstructible<C>
constexpr 
T min (std::initializer_list<T> list, C comp)
{
	auto b = list.begin();
	auto e = list.end();
	auto it = b;


	while(b != e)
	{
		if (comp(b, it))
		{
			it = b;
		}
		b++;
	}
	return *it;
}

template<Ordered T>
	requires CopyConstructible<T>
constexpr 
T min (std::initializer_list<T> list)
{
	return min(list, std::less<>());
}

template<Ordered T, typename C>
	requires Comparator<C, T> && CopyConstructible<C>
constexpr 
const T& min (const T& lhs, const T& rhs, C comp)
{
	return comp(lhs,rhs) ? lhs : rhs;
}

// Reimplement std::for_each
template<Iterator T, typename Func>
	requires Callable<Func, value_type_t<T>>
		&& CopyConstructible<Func>
	Func for_each(T b, T e, Func f)
	{
		while(b != e) 
		{
			f(*b);
			++b;
		}
		return f;
	}

