#include <type_traits>
#pragma 1

//封装实际的萃取方法
namespace detail
{
    template <typename T>
    struct variable_type{};

    template <typename Class, typename T>
    struct variable_type<T Class::*>
    {
        using type = T;
    };
}

template <typename T>
using variable_type_t = typename detail::variable_type<T>::type;

namespace internal
{
    template <typename T>
    struct basic_variable_traits{
        using type = variable_type_t<T>;
        static constexpr bool is_member = std::is_member_pointer_v<T>;
    };
}

//对外可使用的萃取结构
template <typename T> struct variable_traits;

//: 表示当前结构体继承上一个结构体
//但不是is-a关系，更像是has-a关系，表示子类拥有父类的全部信息
template <typename T> 
struct variable_traits<T *>:internal::basic_variable_traits<T>
{
    using pointer_type = T *;
    static constexpr bool is_const = false;
};

template <typename Class, typename T>
struct variable_traits<T Class::*>:internal::basic_variable_traits<T Class::*>
{
    using pointer_type = T Class::*;
    using class_type = Class;
    static constexpr bool is_const = false;
};


