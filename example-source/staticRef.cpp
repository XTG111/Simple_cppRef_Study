#include "variable_traits.h"
#include "function_traits.h"
#include <string>
#include <iostream>

struct Person final
{
    std::string familyName;
    float height;
    bool isFemale;

    void IntroduceMyself() const{
        std::cout << "introduce myself" << std::endl;
    }
    bool IsFemale() const {
        std::cout << "Is Female" << std::endl;
        return false;
    }
    bool GetMarried(Person& other)
    {
        bool success = other.isFemale != isFemale;
        if(isFemale)
        {
            familyName = "Mrs."+other.familyName;
        }
        else
        {
            familyName = "Mr." + other.familyName;
        }
        return success;
    }
};

//自定义decltype
//定义多个自动推导函数指针的类型
template <typename Ret, typename... Args>
auto function_pointer_type(Ret(*)(Args...)) -> Ret(*)(Args...);
//类成员函数
template <typename Ret, typename Class, typename... Args>
auto function_pointer_type(Ret(Class::*)(Args...)) -> Ret(Class::*)(Args...);
//类静态成员函数
template <typename Ret, typename Class, typename... Args>
auto function_pointer_type(Ret(Class::*)(Args...) const) -> Ret(Class::*)(Args...) const;
//利用模板，传入函数指针，调用decltype获取其类型
template <auto F>
using function_pointer_type_t = decltype(function_pointer_type(F));
// template <auto F>
// using function_traits_t = function_traits<function_pointer_type_t<F>>;

//判断是否为函数
//is_function_v 判断的是函数类型，而将要传入的T是一个函数指针，使用remove_pointer去除指针后判断
namespace detail
{
    template <typename T>
    struct is_function{
        static constexpr bool value = std::is_function_v<std::remove_pointer_t<T>> || std::is_member_function_pointer_v<T>;
    };
}

template <typename T>
constexpr bool is_function_v = detail::is_function<T>::value;

//获取类中成员变量和成员函数的类型基类
//需要添加这个基类实现对函数和变量的区分
template <typename T, bool isFunc>
struct basic_field_traits
{
    /* data */
};

//如果是函数，则继承对函数的萃取类
template <typename T>
struct basic_field_traits<T, true>: public function_traits<T>
{
    /* data */
    using traits = function_traits<T>;
    //添加函数，方便获取变量值
    constexpr bool is_member() const
    {
        return traits::is_member;
    }
    constexpr bool is_const() const
    {
        return traits::is_const;
    }
    constexpr bool is_func() const
    {
        return true;
    }
    constexpr bool is_variable() const
    {
        return false;
    }
    //统计函数参数的个数
    constexpr size_t param_count() const
    {
        return std::tuple_size_v<typename traits::args>;
    }
};

//如果是变量，则继承对变量的萃取
template <typename T>
struct basic_field_traits<T, false>: public variable_traits<T>
{
    using traits = variable_traits<T>;
        //添加函数，方便获取变量值
    constexpr bool is_member() const
    {
        return traits::is_member;
    }
    constexpr bool is_const() const
    {
        return traits::is_const;
    }
    constexpr bool is_func() const
    {
        return false;
    }
    constexpr bool is_variable() const
    {
        return true;
    }
};

//对类型的萃取,不管传入变量还是函数的指针都可以获取到类型
template <typename T>
struct field_traits: public basic_field_traits<T, is_function_v<T>>
{
    constexpr field_traits(T&& pointer,std::string_view name): pointer_ins(pointer),name_ins(name) {}
    //变量的指针类型
    T pointer_ins;
    //变量名字
    std::string_view name_ins;
};

//静态反射
template <typename T>
struct TypeInfo{
    /* data */
};
// //对类的静态反射
// template <>
// struct TypeInfo<Person>
// {
//     //获取类中的成员函数和成员变量信息
//     static constexpr auto function = std::make_tuple(
//         field_traits(&Person::IntroduceMyself),
//         field_traits(&Person::IsFemale),
//         field_traits(&Person::GetMarried)
//     );
//     static constexpr auto variables = std::make_tuple(
//         field_traits(&Person::familyName),
//         field_traits(&Person::height),
//         field_traits(&Person::isFemale)
//     );
// };

//使用宏定义 替代上面的模板
#define BEGIN_CLASS(X) template<> struct TypeInfo<X>{

//不定参数个数，##添加保护
#define functions(...) static constexpr auto functions = std::make_tuple(__VA_ARGS__);
//参数 #F获取F的字符串格式
#define func(F) field_traits(F,#F)

//成员变量
#define variables(...) static constexpr auto variables = std::make_tuple(__VA_ARGS__);
#define var(v) field_traits(v,#v)

#define END_CLASS()};

//注册
BEGIN_CLASS(Person)
    functions(
        func(&Person::IntroduceMyself),
        func(&Person::IsFemale),
        func(&Person::GetMarried)
    );
    variables(
        var(&Person::familyName),
        var(&Person::height),
        var(&Person::isFemale)
    )
END_CLASS()

//函数用于获取TypeInfo<T>结构体中的内容
template <typename T>
constexpr auto type_info()
{
    return TypeInfo<T>();
}

//获取类成员函数，然后调用其函数指针
//由于整个函数信息存放在一个tuple里面，所以去遍历tuple
//Idx: tuple中的下标
//Args：tuple中的参数
//Class：类的信息
template <size_t Idx, typename... Args, typename Class>
void VisitTuple(const std::tuple<Args...>& tuple, Class* instance)
{
    using tuple_type = std::tuple<Args...>;
    if constexpr(Idx >= std::tuple_size_v<tuple_type>)
    {
        //遍历完函数tuple
        return ;
    }
    else
    {
        //遍历存储了函数指针的tuple,获取那些无参数的函数然后调用
        if constexpr (auto elem = std::get<Idx>(tuple); elem.param_count() == 0)
        {
            if(instance == nullptr) return ;
            //调用该函数指针指向的函数
            (instance->*elem.pointer_ins)();
        }
        //递归遍历
        VisitTuple<Idx+1>(tuple,instance);
    }
}

int main()
{
    constexpr auto info = type_info<Person>();
    
    //输出第一个成员函数
    // std::cout << std::get<0>(info.functions).name_ins << std::endl;
    // std::cout << std::get<0>(info.variables).name_ins << std::endl;

    Person p;
    VisitTuple<0>(info.functions,&p);

    // //获取到类中的某个函数的信息
    // auto filed1 = field_traits(&Person::GetMarried);
    // std::cout << filed1.is_member() << std::endl;
    // std::cout << filed1.is_const() << std::endl;
    // std::cout << (filed1.pointer_ins == &Person::GetMarried) << std::endl;

    // //获取函数的类型
    // using functype1 = function_pointer_type_t<&Person::IntroduceMyself>;
    // using functype2 = function_pointer_type_t<&Person::GetMarried>;
    // using functype3 = function_pointer_type_t<&Person::IsFemale>;

    // static_assert(std::is_same_v<functype1, void(Person::*)(void) const>);
    // static_assert(std::is_same_v<functype2, bool(Person::*)(Person&)>);
    // static_assert(std::is_same_v<functype3, bool(Person::*)(void) const>);

    return 0;
}