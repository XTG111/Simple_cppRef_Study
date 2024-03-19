# 反射

让程序看到自己的数据，并且能够对数据进行操作

# 类型萃取

对类型做萃取，有一组混合类型，将特定类型获取出来
核心思路：使用模板来匹配查找

## 例子：指针类型萃取

解除一层指针，三级变二级，二级变一级

```c++
template <typename T>
struct remove_pointer {};

template <typename T>
struct remove_pointer<T*> {
  using type = T;
};

int main()
{
  using type = remove_pointer<int*>::type; //type 类型为int
  return 0;
}
```

## 固定流程

1. 声明一个标准的模板用来声功能

```c++
template <typename T>
struct remove_const{};
```

2. 声明定义需要萃取的类，比如获取const修饰的类型

```c++
template <typename T>
//匹配参数，需要使用T来拼凑
struct remove_const<const T>{
  using type = T;
};
```

3. 使用时需要使用相应的传入参数，然后调用remove_const就可以获得T

```c++
int main()
{
  const int a;
  using type = remove_const<a>::type;
}
```

## 成员函数和普通函数的模型

对于一个普通函数int func(double) 其函数指针可以表示为int*(double)
而对于一个成员函数 int ClassA::func(double) 其函数指针为int (ClassA::*)(double)
对于一个成员变量 int ClassA::c 其指针为 int (ClassA::*) **可以直接使用cout输出为1，导致如果是两个不同的类如果使用两个指针来接受成员变量，然后做判断会出现true**
