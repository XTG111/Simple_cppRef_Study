#include <type_traits>
#include <tuple>

bool Foo(int,double){return false;}

class Person{
public:
    bool IsFemale() {return false;}
    bool IsFemaleConst() const {return false;}
    bool c;
};

template <typename T>
struct function_traits {};

//对普通函数的萃取
template <typename Ret, typename Param1, typename Param2>
//通过接受函数指针来获取返回类型
struct function_traits<Ret(*)(Param1, Param2)>{
    using ret_type = Ret;
    using param1_type = Param1;
    using param2_type = Param2;
    static constexpr bool is_class = false;
};

//使用不定模板参数接受多形参函数
template <typename Ret, typename... Args>
//通过接受函数指针来获取返回类型
struct function_traits<Ret(*)(Args...)>{
    using ret_type = Ret;
    using param1_type = std::tuple<Args...>;
    static constexpr bool is_class = false;
};

//对成员函数萃取
template <typename Ret, typename Class, typename Param1>
struct function_traits<Ret(Class::*)(Param1)>{
    using ret_type = Ret;
    using class_type = Class;
    using param1_type = Param1;
    static constexpr bool is_class = true;
};

template <typename T>
struct  variable_traits{};

//对成员变量的萃取
template <typename Ret, typename Class>
struct variable_traits<Ret(Class::*)>{
    using ret_type = Ret;
    using class_type = Class;
    static constexpr bool is_class = true;
};

//对普通变量的萃取
template <typename Ret>
struct variable_traits<Ret*>
{
    using ret_type = Ret;
    static constexpr bool is_class =false;
};

int main()
{
    //构造传入参数
    auto p = &Foo;
    using Func_Types = function_traits<decltype(p)>;
    //bool
    using ret = Func_Types::ret_type;
    //int
    using p1_type = Func_Types::param1_type;
    //double
    using p2_type = Func_Types::param2_type;

    // IsFemale-> p type == bool (Person::*)(void)
    bool (Person::* p1)(void) = &Person::IsFemale;
    //IsFemaleConst -> p2 type == bool (Person::*)(void) const
    bool (Person::* p2)(void) const = &Person::IsFemaleConst;
    //c -> p3 type == bool(Person::*)
    bool (Person::* p3) = &Person::c;


    static_assert(std::is_same_v<ret, bool>);
    static_assert(std::is_same_v<p1_type, int>);
    static_assert(std::is_same_v<p2_type, double>);
    return 0;
}