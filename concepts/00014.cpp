#include <type_traits>
#include <utility>
#include <initializer_list>
#include <iterator>

// Using concepts syntax of your choice:

// The tricky thing about concepts is splitting them into
// smaller pieces when that smaller piece might be useful
// at some point. Because it will be annoying if the compiler
// can't figure out that CopyConstructible subsumes Integral.
// Wrote these looking at cppreference.com, but without
// peeking at the Ranges TS, which I believe defines many of
// these or similar things also...

template <typename T, typename... List>
struct type_in_list : public std::false_type {};
template <typename T, typename... List>
struct type_in_list<T, T, List...> : public std::true_type {};
template <typename T, typename Head, typename... List>
struct type_in_list<T, Head, List...> :
    public std::bool_constant<
    type_in_list<T, List...>::value> {};

template <typename T>
concept bool Signed = std::is_signed_v<T>;
template <typename T>
concept bool Unsigned = std::is_unsigned_v<T>;
template <typename T>
concept bool NarrowChar =
    type_in_list<std::remove_cv_t<T>,
                 char, signed char, unsigned char>::value;
template <typename T>
concept bool WideChar =
    type_in_list<std::remove_cv_t<T>,
                 char16_t, char32_t, wchar_t>::value;
template <typename T>
concept bool CharType = NarrowChar<T> || WideChar<T>;
template <typename T>
concept bool Integral =
    Signed<T> || Unsigned<T> || CharType<T>;
template <typename T>
concept bool Floating = std::is_floating_point_v<T>;
template <typename T>
concept bool Arithmetic = Integral<T> || Floating<T>;

template <typename T>
concept bool IsVoid = std::is_void_v<T>;
template <typename T>
concept bool IsNullPtr = std::is_null_pointer_v<T>;

// g++ 7.2 complains if std::underlying_type_t is used
// invalidly anywhere inside a concept.
template <typename T, bool IsEnum = std::is_enum_v<T>>
struct enum_helper {};
template <typename T>
struct enum_helper<T, true>
{ using type = std::underlying_type_t<T>; };
template <typename T>
using enum_helper_t = typename enum_helper<T>::type;

// An unscoped enum implictly converts to its
// underlying type.
template <typename T>
concept bool UnscopedEnum =
    requires (T e, void (*f)(enum_helper_t<T>))
    { (*f)(e); };
template <typename T>
concept bool EnumClass =
    std::is_enum_v<T> && !UnscopedEnum<T>;
template <typename T>
concept bool Enum = UnscopedEnum<T> || EnumClass<T>;

template <typename T>
concept bool Array = std::is_array_v<T>;
template <typename T>
concept bool Union = std::is_union_v<T>;
template <typename T>
concept bool Class = std::is_class_v<T>;
template <typename T>
concept bool ClassLike = Union<T> || Class<T>;
template <typename T>
concept bool Function = std::is_function_v<T>;

template <typename T>
concept bool LValReference = std::is_lvalue_reference_v<T>;
template <typename T>
concept bool RValReference = std::is_rvalue_reference_v<T>;
template <typename T>
concept bool Reference =
    LValReference<T> || RValReference<T>;

template <typename T>
concept bool FuncPointer =
    std::is_pointer_v<T> && Function<std::remove_pointer_t<T>>;
template <typename T>
concept bool VoidPointer =
    std::is_pointer_v<T> && IsVoid<std::remove_pointer_t<T>>;
template <typename T>
concept bool ObjPointer =
    std::is_pointer_v<T> && !FuncPointer<T> && !VoidPointer<T>;
template <typename T>
concept bool Pointer =
    FuncPointer<T> || VoidPointer<T> || ObjPointer<T>;
template <typename T>
concept bool MemObjPointer =
    std::is_member_object_pointer_v<T>;
template <typename T>
concept bool MemFuncPointer =
    std::is_member_function_pointer_v<T>;
template <typename T>
concept bool MemPointer =
    MemObjPointer<T> || MemFuncPointer<T>;
template <typename T>
concept bool AnyPointer = Pointer<T> || MemPointer<T>;

template <typename T>
concept bool Scalar =
    Arithmetic<T> || Enum<T> || AnyPointer<T> || IsNullPtr<T>;

template <typename T>
concept bool Object = Scalar<T> || Array<T> || ClassLike<T>;

template <typename T>
concept bool Overloadable = ClassLike<T> || Enum<T>;

////////////

template <typename T1, typename T2>
concept bool SameType = std::is_same_v<T1, T2>;

template <typename T>
concept bool DestructibleClass = ClassLike<T> &&
    requires (T obj) { obj.~T(); };

template <typename T>
concept bool CanCallDestructor =
    Scalar<T> || DestructibleClass<T>;

template <typename T>
concept bool DestructibleArray =
    Array<T> && CanCallDestructor<std::remove_all_extents_t<T>>;

template <typename T>
concept bool Disposable =
    CanCallDestructor<T> || DestructibleArray<T> ||
    Reference<T> || IsVoid<T>;

// ( T{} works for arrays; T() does not. )
template <typename T>
concept bool ValueInitializable =
    Scalar<T> ||
    ( ( DestructibleClass<T> || DestructibleArray<T> )
      && requires { T{}; } );

// In the following concepts, Source may be a reference type.
template <typename T, typename Source>
concept bool CopyInitializable =
    ( CanCallDestructor<T> || Reference<T> ) &&
    requires (Source s) {
        { std::forward<Source>(s) } -> T;
    };

template <typename T, typename Source>
concept bool ScalarConversion =
    Scalar<T> && CopyInitializable<T, Source>;

template <typename T, typename Source>
concept bool BindableRef =
    Reference<T> && CopyInitializable<T, Source>;

template <typename... Ts>
struct first_type { using type = void; };
template <typename T0, typename... Rest>
struct first_type<T0, Rest...> { using type = T0; };
template <typename... Ts>
using first_type_t = typename first_type<Ts...>::type;

// Avoid using T(std::forward<Args>(args)...) when sizeof...
// is 1 for pointers or references, since the syntax then
// becomes the too-powerful C-style cast!
// Use first_type_t because g++ 7.2 tries to substitute
// arguments into ScalarConversion and BindableRef even
// if sizeof... is not 1.
template <typename T, typename... Args>
concept bool DirectInitializable =
    ( sizeof...(Args)==0 && 
        ( IsVoid<T> || Scalar<T> ||
            ( DestructibleClass<T> && ValueInitializable<T> ) ) )
 || ( sizeof...(Args)==1 &&
        ( IsVoid<T> ||
          ScalarConversion<T,first_type_t<Args...>> ||
          BindableRef<T,first_type_t<Args...>> ) )
 || ( sizeof...(Args)>=1 &&
      DestructibleClass<T> &&
      requires (Args... args)
      { T(std::forward<Args>(args)...); } );

template <typename T>
concept bool MoveConstructible =
    Scalar<T> ||
    ( DestructibleClass<T> &&
      CopyInitializable<T, T&&> &&
      DirectInitializable<T, T&&> );

template <typename T>
concept bool CopyConstructible =
    Scalar<T> ||
    ( MoveConstructible<T> &&
      CopyInitializable<T, const T&> &&
      DirectInitializable<T, const T&> );

template <typename T>
concept bool MoveAssignable =
    Scalar<T> ||
    ( ClassLike<T> &&
      requires( T lhs, T&& rhs )
      { { lhs = std::move(rhs) } -> T&; } );

template <typename T>
concept bool CopyAssignable =
    Scalar<T> ||
    ( MoveAssignable<T> &&
      requires( T lhs, const T& rhs )
      { { lhs = rhs } -> T&; } );

template <typename F>
concept bool MaybeCallable =
    Function<F> || FuncPointer<F> || ClassLike<F>;

template <typename F, typename Ret, typename... Args>
concept bool Callable =
    ( Function<F> || FuncPointer<F> || ClassLike<F> ) &&
    ( ( IsVoid<Ret> &&
        requires (F& f, Args... args)
        { f(std::forward<Args>(args)...); } ) ||
      requires (F& f, Args... args)
      { { f(std::forward<Args>(args)...) } -> Ret; } );

// Swappable doesn't subsume anything else, because you
// can always have an ambiguous overload.
namespace SwappableDetail {
    using std::swap;
    template <typename T>
    concept bool Swappable = requires(T& a) { swap(a, a); };
}
// using SwappableDetail::Swappable; // g++7.2 doesn't like
template <typename T>
concept bool Swappable = SwappableDetail::Swappable<T>;

// Reimplement std::min
template <typename T>
concept bool LessComparable =
    Arithmetic<T> ||
    ( Overloadable<T> &&
      requires (const T x) { { x<x } -> bool; } );

template <LessComparable T>
constexpr const T& min(const T& a, const T& b)
{ return b<a ? b : a; }

template <typename T,
          Callable<bool, const T&, const T&> Compare>
constexpr const T& min(const T& a, const T& b, Compare comp)
{ return comp(b, a) ? b : a; }

template <LessComparable T> requires CopyConstructible<T>
constexpr T min(std::initializer_list<T> il)
{
    auto iter = il.begin();
    const T* best = std::addressof( *iter );
    for ( ++iter; iter != il.end(); ++iter ) {
        if ( *iter < *best ) best = std::addressof( *iter );
    }
    return *best;
}

template <CopyConstructible T,
          Callable<bool, const T&, const T&> Compare>
constexpr T min(std::initializer_list<T> il, Compare comp)
{
    auto iter = il.begin();
    const T* best = std::addressof( *iter );
    for ( ++iter; iter != il.end(); ++iter ) {
        if ( comp( *iter, *best ) )
            best = std::addressof( *iter );
    }
    return *best;
}

// Reimplement std::for_each

template <typename T>
concept bool Iterator =
    Swappable<T> &&
    ( ObjPointer<T> ||
      ( Overloadable<T> &&
        CopyConstructible<T> &&
        CopyAssignable<T> &&
        requires (T iter, const T citer) {
            typename std::iterator_traits<T>::iterator_category;
            typename std::iterator_traits<T>::value_type;
            typename std::iterator_traits<T>::pointer;
            typename std::iterator_traits<T>::reference;
            typename std::iterator_traits<T>::difference_type;
            *citer;
            requires SameType<decltype(++iter), T&>;
        } ) );

template <typename T>
concept bool EqualComparable =
    Arithmetic<T> || AnyPointer<T> || IsNullPtr<T> ||
    ( Overloadable<T> &&
      requires (const T a) { { a==a } -> bool; } );

template <typename T>
concept bool EqualUnequalComparable =
    Arithmetic<T> || AnyPointer<T> || IsNullPtr<T> ||
    ( EqualComparable<T> &&
      requires (const T a) {
          a!=a ? 0 : 0;
          // EqualComparable checked that a==a can be
          // implicitly converted to bool, but if a==a
          // has class type, !(a==a) might do something
          // different.
          !(a==a) ? 0 : 0;
      } );

template <typename T>
concept bool HasOpArrow =
    ClassLike<T> &&
    requires (T iter) { iter.operator->(); };

class op_arrow_detail {
private:
    struct invalid;

    template <HasOpArrow T>
    using op_arrow_type =
        std::remove_reference_t<
            decltype(std::declval<T>().operator->()) >;

public: // g++ 7.2 doesn't like if these are private.
    template <typename T, typename... Examined>
    struct resolve_arrow
    {
        using type = invalid;
    };
    template <typename T, typename... Examined>
        requires type_in_list<T, Examined...>::value
    struct resolve_arrow<T, Examined...>
    {
        using type = invalid;
    };
    template <ObjPointer T, typename... Examined>
    struct resolve_arrow<T, Examined...>
    {
        using type = T;
    };
    template <HasOpArrow T, typename... Examined>
        requires (!type_in_list<T, Examined...>::value)
    struct resolve_arrow<T, Examined...>
    {
        using type = typename resolve_arrow<
            op_arrow_type<T>, T, Examined...>::type;
    };

public:
    template <typename T>
    using type = typename resolve_arrow<T>::type;
};

template <typename T>
concept bool InputIterator =
    ( ObjPointer<T> && Swappable<T> ) ||
    ( Iterator<T> &&
      std::is_base_of_v<
          std::input_iterator_tag,
          typename std::iterator_traits<T>::iterator_category> &&
      EqualUnequalComparable<T> &&
      requires (T iter, const T citer) {
          requires SameType<
              decltype(*citer),
              typename std::iterator_traits<T>::reference >;
          { *citer } ->
              typename std::iterator_traits<T>::value_type;
          (void)iter++;
          { *iter++ } ->
              typename std::iterator_traits<T>::value_type;
      } &&
      ( Scalar<typename std::iterator_traits<T>::value_type>
        || SameType<typename op_arrow_detail::type<const T>,
             typename std::iterator_traits<T>::value_type*>
    ) );

template <InputIterator Iter,
          Callable<void,
              typename std::iterator_traits<Iter>::reference>
          Func>
void for_each( Iter first, Iter last, Func f )
{
    for (; first != last; ++first )
        f( *first );
}

///////////////

struct MyData { int n; double z; };
bool operator<(const MyData&, const MyData&);
struct MyIter {
    using iterator_category = std::input_iterator_tag;
    using value_type = const MyData;
    using reference = value_type&;
    using pointer = value_type*;
    using difference_type = std::ptrdiff_t;
    reference operator*() const;
    pointer operator->() const;
    MyIter& operator++();
    MyIter operator++(int);
};
bool operator==(const MyIter&, const MyIter&);
bool operator!=(const MyIter&, const MyIter&);
void swap(MyIter&, MyIter&);

int main() {
    min(2, 5);
    MyData d1{ 3, 1.5 };
    const MyData d2{ 10, 0 };
    const MyData& d3 = min(d1, d2);
    auto greater_int = [](int x, int y) { return x>y; };
    auto my_greater =
      [](const MyData& x, const MyData& y) { return y<x; };
    min(2, 5, greater_int);
    min(d1, d2, my_greater);
    min( { 1, 5, -2, 10 } );
    min( { d1, d2, d3 } );
    min( { 1, 5, -2, 10 }, greater_int );
    min( { d1, d3, d2 }, my_greater );

    int iarr[] = { 1, 5, -2, 10 };
    auto do_int = [](int& n) { ++n; };
    for_each(std::begin(iarr), std::end(iarr), do_int);
    const MyData darr[] = { d1, d2, { -5, 3.0 } };
    auto do_data = [](const MyData&) {};
    for_each(std::begin(darr), std::end(darr), do_data);
    for_each(MyIter{}, MyIter{}, do_data);
}


