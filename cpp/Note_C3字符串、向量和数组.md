# 3.1 命名空间using声明

在程序开始阶段用using进行声明，可以一行声明多个

```
using std::cin; using std::cout;
```

**头文件中不应包含using** ,否则调用其的文件中也会有using声明，也许会出现无法意料的错误

# 3.2 标准库类型string

```
可变长字符序列
string s1; //默认初始化为空
string s2 = s1;
string s3 = "hiya"; // 是字符串字面值的副本，除了最后那个空字符
string s4(10,'c') ; //内容是10个c组成的字符串

```

- =  初始化，实际上执行的 是拷贝初始化，编译器把右边的初始值直接拷贝到新创建的对象中。
- 其他则是执行 直接初始化
- 如果初始值只有一个，则用拷贝初始化和直接初始化都可以，如果是多个则得用直接初始化
- eg：多个： string s5(10,'c');

### string对象上的操作	

- getline(is , s)， 从is流中读一行给s，不忽略空白，遇到换行符为止，开始为换行符则得到一个空字符串
- s.empty() , 是否为空
- s.size() , 返回s中的字符个数，不含末尾的空字符
  - size（）返回的是string:: size_type类型，是一个无符号类型，有size（）函数则不要用int
- s[n], 返回第n个字符的引用，n=0,1,2,..
- s1=s2, 用s2替代s1中的字符
- ==、！=、<、..., 大小写敏感, 字典顺序比较
- string对象的读写： **string对象会自动忽略开头的空白**，到下一个空白停止
- getline和cin都会返回流参数，读取成共true
- endl,换行符写入输出流并刷新缓冲区内容到设备
- **s1 + s2得到一个新string对象**
- 标准库允许把字符字面值和字符串字面值转换成string对象
  - 在字面值混合使用时确保每个+两侧对象至少有一个是string
  - 不能把字面值直接相加处理string中的字符

###　处理string对象中的字符

cctype

- isalnum(c)
- isalpha(c)
- isdigit(c)
- islower\isupper
- tolower\toupper
- isspace
- ispunct(c) ,是标点符号时

遍历方式：

- 范围for

  - ```
    for (auto c : str){
    	
    }
    ```

- 用范围for**改变字符串中的字符**

  - 此时需要将循环变量定义成引用类型

    - ```
      for ( auto &c : s){
      	c=toupper(c);
      }
      ```

  - 只处理部分字符

    - 用下标运算符[]

# 3.3 标准库类型vector

容器，vector 是一个类模板，模板本身不是类或函数，是编译器生成类或函数的一份说明书。

编译器根据模板创建类或函数的过程称为**实例化**，使用模板时需要指出编译器应把函数或类实例化为哪种类型。

- vector< int > ivec;
-  vector<Sales_item> sales_vec
- vector<vector<string>> file;
- vector能容纳绝大多数对象作为元素，除了引用（不是对象）
- 早期版本c++,需要在外层vector对象的右尖括号和其他元素类型之间添加一个空格。vector<vector<string> >

#### vector的初始化

- v1; //默认初始化vector,创建一个空vector
- v2(v1);
- v2=v1;
- v3(n,val);
- v4(n); //n个重复执行了值初始化的对象
- v5{a ,b ,c ...}; //
- v5={a,b,c...}
- () 和{} ，（）表示个数，{}表示值

#### 向vector对象中添加元素

- **v2.push_back(i)** ;//放到v2末尾
- **如果循环体内包含有指向vector对象*添加元素*的语句，则不能使用范围for**
- 范围for语句体内不应该改变其遍历序列的大小

#### vector的其他操作

- v.empty()
- v.size()
- v[n]
- v1=v2;
- v1={a,b,c...}
- ==、！=、<、<=、...字典顺序比较,其中的对象需要可以比较才行
- 在获取size_type时要加上包含的类型，vector<int >::size_type
- 不能使用下标形式添加元素，只能对已存在的元素执行下标操作

# 3.4 迭代器介绍

所有标准库容器都可以使用迭代器；

迭代器 类似于指针，提供了对对象的间接访问，其对象是容器中的元素或者string对象中的字符。

但其中少数几个才同时支持下标操作；

迭代器的获取

- v.begin() ;//返回指向第一个元素的迭代器
- v.end()； //返回指向尾元素的下一个元素的位置的迭代器，尾后迭代器（若v为空，则begin和end都返回是同一个迭代器）

迭代器支持的运算：

- ==、！= ;//指向的是否是同一个元素

- *iter ；//返回迭代器所指元素的索引，不能对尾迭代器解引用

- iter->mem; //解引用iter并获取该元素的名为mem的成员，等价**于（*iter).mem**

- ++iter;指向容器中下一个元素

- --iter

  - ```
    while(s.begin()!=s.end()){
    
    }
    ```

- vector和string迭代器支持的运算（除了==和!=外）：
  - iter+n; 移动n个元素后的位置
  - iter-n;
  - iter +=n;
  - iter -=n;
  - iter1 -  iter2; 它们之间的距离
  - **iter1< iter2 ; iter1在iter2之前**

迭代器类型：

- iterator: 可读写
- const_iterator:仅可读
- 与vector是否是常量有关

cbegin() ;cend()；返回const_iterator;

不能在范围for中向vector添加元素

不能在使用迭代器时 ，改变vector的容量

# 3.5 数组

#### 基本定义：

- 复合类型，维度必须是一个常量表达式
- 默认会初始化数组元素
- 定义时 必需指定数组类型，不可以用auto推断
- 数组的元素应为对象，不可以是引用

#### 初始化数组的方式：

const unsigned sz= 3;

- int ial[sz] ={0,1,2} ；//指定初始值个数，并给初始值 ，初始值个数必须小于等于指定个数
- int a2[ ] = { 1, 2 , 3} ; // 不指定维度，直接给初始值
- 字符数组的初始化：
  - char a1[] = { 'a' ,'b'  };
  - char a2 [] = “C++” ;  //实际还会拷贝 一个空字符 ’\0‘
  - const char a3[6 ] = "Daniel" ; //出错，还有一个空字符

#### 不允许拷贝和赋值

int a[]={1,2,3};

- 不能将数组的内容拷贝给其他数组作为其初始值: int a2[] = a;
- 也不能用数组为其他数组赋值  : a2 = a;

#### 复杂数组的声明：

int  *ptr[10] ; 

int &ptr2[10] ; 错误

int (*p) [10] =&arr ; 指向数组的指针

int (&p) [10] = arr ;指向数组的引用

#### 访问数组元素下标：size_t类型

数组、vector、string的下标是否越界编译器不检查

#### 数组和指针

&num[0]  ;获得指向数组中元素的指针；

&num ；获得指向数组的指针

数组类型的对象其实是指向该数组首元素的指针（当数组名作为右值时）auto  p = num ; 得到一个指针

但decltype(num)  返回的是一个10个整数构成的数组

数组名不能作为左值

#### 指针也是迭代器

确定首尾迭代器；int *e =&arr[10];

```
for (int *b=arr ;b! =e;b++){
	cout<< *b<<endl;
}

得到尾后指针：begin、end函数
int *beg =begin(arr);
int *last=end(arr);
```

#### 指针运算

指针加减数字后 得到的还是指针

两个指针相减的类型是：ptrdiff_t

#### 数组与标准库类型的string \vector 的区别

标准库类型的下标必须是无符号类型

内置数组的下标无此要求，即下标可以是负数

#### C标准库String 函数：C风格字符串：放在字符数组并以空字符结尾

下列函数不对参数进行验证，传入此类函数的指针必须以'\0'结尾

- strlen (p ) ,返回p的长度，不包含’\0‘
- strcmp(p1, p2) , 比较p1,p2 大小；p1>p2 返回正值 ，p1==p2 返回0
- strcat(p1,p2) ,将p2附加到p1后，返回p1
- strcpy(p1,p2 ), 将p2拷贝给p1，返回p1

想要比较C风格的字符串必须要调用strcmp，因为这里用的是指针，两个指针比较不能用普通的关系运算符

#### 与旧代码接口的兼容

- 混用string对象和C风格字符串
  - 允许用以空字符结束的字符数组来初始化string对象或为string对象赋值
  - 在string对象的加法运算中允许**以空字符结束的字符数组**作为其中一个运算对象（不能两个都是）
  - 在string对象的复合赋值运算中允许以空字符结束的字符数组作为右侧的运算对象
  - 反之不成立，char *str =s ;错误
  - const char *str = s.c_str() ;可以，但是如果s变化，str也会变化，故若想要s.c_str()返回值一直有效，则应该重新拷贝一份
- 使用数组初始化vector对象
  - 不允许使用一个数组为另一个内置类型的数组赋初值
  - 不允许使用vector对象初始化数组
  - 可以用数组来初始化vector对象，指明拷贝区域的首尾元素地址即可
    - vector<int> ivec(begin(int_arr) ,end(int_arr)) ;
    - vector<int> subVec(int_arr + 1, int_arr + 4) ；// int_arr[1] ，int_arr[2] , int_arr[3] 
- 现代C++程序应当尽量使用vector和迭代器，避免使用内置数组和指针
- 应当尽量使用string， 避免使用C风格的基于数组的字符串

# 3.6 多维数组

#### 数组的初始化

可以只初始化部分元素，剩下的默认初始化

#### 多维数组的下标引用

```
ia[3][4];
int (&row)[4]= ia[1]; 将row绑定到ia的第二个4元素数组上
```

#### 嵌套循环处理多维数组的元素

```
constexpr size_t rowCnt=3,colCnt=4;  //常量表达式
int ia[rowCnt][colCnt]; 12个未初始化元素
for(size_t i =0; i!=rowCnt ; ++i){
	for(size_t j=0;j!=colCnt ; ++j){
		ia[i][j] = i* colCnt +j;
	}
}

使用范围for
size_t cnt = 0;
for(auto &row :ia){   //因为要对元素的值进行改变，所以这里需要用到引用类型，不然就是字面值，无法更改
	for(auto &col : row){
		col = cnt;
		++ cnt;
	}
}

//不对元素进行修改也要使用引用，因为auto 会将数组推断为指针类型,用引用则可以避免
for(const auto &row :ia ){
	for(auto col : row){
		cout<<col<< endl;
	}
}
```

#### 指针和多维数组

当使用多维数组时，多维数组名转换得来的指针实际上是指向第一个内层数组的指针

使用普通循环时：

```
int ia[3][4] ;
for(auto p = ia ;p != ia + 3; ++p){   //p是指向一维数组的指针
	for(auto q= *p; q!= *p+4 ; ++q) {  //*p是一个包含4个元素的数组，数组名会被自动转换为数组首元素的地址
		cout<< *q <<endl; //这里q也是指向数组元素的指针
	}
	cout<<endl;
}

使用begin、end
for(auto p=begin(ia) ; p!=end(ia);p++){
	for(auto q=begin(*p) ;q!=end(*p);q++){
		cout<<*q<<endl;
	}
}
```

#### 类型别名简化多维数组的指针

using int_array =int [4];  //新标准下类型别名的声明

typedef int int_array[4] ;  //等价的typedef声明 ,将 int_array => int [4]

```
for(int_array *p =ia ;p!= ia + 3; ++p){
	for(int *q =*p;q!=*p+4 ;++q){
		cout<<*q<<endl;
	}
}
```





