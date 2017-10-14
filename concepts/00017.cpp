// Using concepts syntax of your choice:
#include<algorithm>
#include<functional>
#include<vector>

#define Forward(x) std::forward<decltype(x)>(x)

template<class T,class Compare>
concept bool OrderedBy = requires(const T& t,const Compare& c)
  {
  {c(t,t)}->bool;
  };

template<class T>
concept bool Ordered = OrderedBy<T,std::less<T>>;

template<class T>
using pointed_type = decltype(*std::declval<T>());

template<class T>
concept bool Iterator = requires(T&& v)
  {
  {*Forward(v)}->const auto&;//not void
  {++v}-> T&;
  {v!=v}-> bool;
  };

template<class T,class...Args>
concept bool Callable = requires(T&& v,Args&&...args)
  {
  Forward(v)(Forward(args)...);
  };

// Reimplement std::min
Ordered{T}
constexpr auto 
min(const T& t,const T& u)
  {
  return t<u;
  }

OrderedBy{T,C}
constexpr auto 
min(const T& t,const T& u,C c)
  {
  return c(t,u);
  }

constexpr auto
min(std::initializer_list<Ordered> l)
  {
  return *std::min_element(l.begin(),l.end());
  }

//template<class C> //I expected it to work
//constexpr auto
//min(std::initializer_list<OrderedBy<C>> l,C c)
OrderedBy{T,C}
constexpr auto
min(std::initializer_list<T> l,C c)
  {
  return *std::min_element(l.begin(),l.end(),c);
  }


// Reimplement std::for_each
Iterator{T}
auto
constexpr for_each(T beg, T end, auto f)
  requires Callable<decltype((f)),pointed_type<T>>
  {
  for(;beg!=end;++beg)
    f(*beg);
  return f;
  }

    



