#include <type_traits>
#include <tuple>
#pragma 1

namespace detail
{
    template <typename Func>
    struct basic_function_traits;

    template <typename Ret, typename... Args>
    struct basic_function_traits<Ret (Args...)>
    {
        //函数的参数
        using args = std::tuple<Args...>;
        //函数的返回类型
        using return_type = Ret;
    };
}

template <typename Func>
struct function_traits;

//对普通函数的萃取
template <typename Ret, typename... Args>
struct function_traits<Ret (Args...)>: public detail::basic_function_traits<Ret (Args...)>
{
    using type = Ret(Args...); //返回值类型
    using args_with_class = std::tuple<Args...>; // 带有类的参数列表
    using pointer_type = Ret(*)(Args...); //函数指针类型
    static constexpr bool is_member = false;
    static constexpr bool is_const = false;
};

//对类成员函数的萃取
template <typename Ret, typename Class, typename... Args>
struct function_traits<Ret (Class::*)(Args...)>: public detail::basic_function_traits<Ret (Args...)>
{
    using type = Ret(Class::*)(Args...);
    using args_with_class = std::tuple<Class*, Args...>;
    using pointer_type = Ret (Class::*)(Args...);
    static constexpr bool is_member = true;
    static constexpr bool is_const = false;
};

//对const 类成员函数的萃取
template <typename Ret, typename Class, typename... Args>
struct function_traits<Ret (Class::*)(Args...) const>:detail::basic_function_traits<Ret (Args...)>
{
    using type = Ret(Class::*)(Args...) const;
    using args_with_class = std::tuple<Class*, Args...>;
    using pointer_type = Ret (Class::*)(Args...) const;
    static constexpr bool is_member = true;
    static constexpr bool is_const = true;
};