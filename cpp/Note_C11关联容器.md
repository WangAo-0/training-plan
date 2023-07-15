# 容器类型

1. 按关键字有序保存元素 
   - map : 保存键值对，关键字不可以重复 （映射）
   - set ：保存关键字，关键字不可重复
   - multimap ：关键字可重复的map
   - multiset : 关键字可重复的set
2. 无序集合
   - unordered_map : 用哈希函数组织的map
   - unordered_set :用哈希函数组织的set
   - unordered_multimap : 关键字可以重复的unordered_map
   - unordered_multiset ：关键字可以重复的unordered_set

map、multimap定义在map头文件下

set、multiset 定义在set头文件下

无序容器 定义在 unordered_map 、unordered_set 下

# 与顺序容器的区别

不支持push_back、push_front操作

关联容器的迭代器是双向的：可读写、可多变扫描、可递增递减

支持类型别名，支持很多其他操作

- ```
  关联容器的类型别名
  key_type //容器的关键字类型
  mapped_type //map类关键字的关联类型
  value_type //对于set,与key_type相同，对应map为pair<const key_type ,mapped_type>,关键字不可以修改，故为const
  
  可以用作用域运算符提取一个类型的成员：
  map<string , int >::key_type v1 ;//v1是string 
  map<string , int >::mapped_type v2 ;//v2是int
  map<string, int >::value_type v3 ; //v3是pair<const string ,int>
  
  ```

无序容器还提供一些调整哈希性能的操作



# 容器初始化

定义map时，需要指明关键字和值的类型；定义set时需要指定关键字的类型

```
1、map<string ,size_t> word_count ; 默认构造，空容器
2、set<string > exclude = {"A","B"}; 列表初始化
   map<string, string > authors= {
   {"A","AA"},
   {"B","BB"}
  }
3、set<string> set1(exclude) ;也可以用另一个同类型的关联容器初始化；

4、vector<int> ivec;  
for(vector<int>::size_type i=0; i!=10 ;++i){
	ivec.push_back(i);
}
set<int> iset(ivec.cbegin(),ivec.cend()); 迭代器区间初始化 
```

# 关键字类型的要求

### 对于有序容器，关键字必须定义元素的比较方法；

- 默认**标准库下**用关键字类型的**< 运算符**来比较两个关键字

- 自定义操作替代关键字上的< 运算符，必须满足严格弱序

  - 两个关键字不能同时 小于等于 对方

  - k1小于等于k2 ,且 k2小于等于k3 ,则 k1必须小于等于 k3

  - 若两个关键字互相都不小于等于另一个，则这两个关键字等价，等价也有传递性

  - 自定义操作的类型必须在尖括号中紧跟着元素类型给出 ，即<自定义类型，比较操作类型（一种函数指针类型）>

    - 尖括号中出现的类型仅仅是一个类型，在创建容器时，还需要以构造函数参数的方式提供真正的比较操作。

    - ```
      //定义Sales_data类型的比较器
      bool compareIsbn(const Sales_data &lhs ,const Sales_data &rhs){
      	return lhs.isbn() < rhs.isbn() ;
      }
      multiset<Sales_data ,decltype(compareIsbn) *>  bookstore(compareIsbn) ; //函数名会自动转换为函数指针
      //每当添加元素时会调用这compareIsbn进行排序，即按isbn值的大小排序
      ```

    - 

# pair类型

在头文件 utility中

pair <key ,value> anon ;

pair <string ,vector<int> > line;  

- pair默认构造函数对数据成员进行**值初始化**
- 也可以提供初始化器 pair<string ,string > author { "James" , " Jack" } ;
- pair的数据成员是**public** ; author .first ,author .second; 可直接用普通成员访问符访问

#### pair的操作

- pair<T1,T2> p ;
- pair<T1 ,T2 > p( v1 ,v2)
- pair<T1 ,T2 > P= {V1,V2}
- **make_pair(v1,v2 ) ;** 
- p1 relop p2; 关系运算符按字典顺序比较
- p1 == p2 ; 
- 新标准下，返回pair对象可以直接 **return { "A" ,"B" } ;** 
- 或者隐式构造返回值 ，return pair<string ,string >() ;

# 关联容器迭代器

获取迭代器的方法：

```
//解引用关联容器的迭代器，得到一个value_type的值的引用
auto map_it = word_count.begin(); // *map_it 是一个指向pair<const string ,int >对象的引用
cout<< *map_it.first ;
map_it->first = "ak" ;//错误
++map_it->second; 
```

set的迭代器是const 的， 只能读

# 遍历关联容器的方法

```
//获取首尾迭代器
auto map_it = word_count.cbegin() ;
while(map_it != word_count.cend()){
	cout<<map_it->first<<endl;
	++map_it;
}
当有序关联容器输出时，按关键字升序
```

# 关联容器只能用于只读算法

find()函数，给定关键字，直接获取元素

# 添加元素

```
set<int > set2;
set2.insert(ivec.cbegin(),ivec.cend()); //一对迭代器
set2.insert({1,2,3,4});  //一个初始化器列表
word_count.insert({word,1});  //返回void
c.insert(p,v) ;p是迭代器提示位置，v是value_type类型的对象，返回一个迭代器
对于不重复的，insert返回一个pair，first是一个迭代器指向给定关键字元素，second是bool，表示插入是否成功。
对于multiset,insert后返回一个指向新元素的迭代器
```

# 删除元素

erase() ;

- 按关键字删除 ，erase(word ... ) ；返回删除的个数
- 删除范围内的，erase(b,e)  ; 迭代器范围，返回e
- 按迭代器指向删除 ，erase( p) ; p不能是c.end() ,返回指向p后面的元素迭代器

# map的下标操作

- 不能对multimap\unodered_multimap操作，单边不确定

- 下标运算符[ ] ：map[key ] 

  - **如果不存在，则新建一个并插入**，关联值进行值初始化；

  - 通常解引用一个迭代器所返回的类型与下标运算符所返回的类型一样

    但是map，下标运算获得是mapped_type对象；解引用map迭代器获得的是value_type对象

- at函数 ：map.at(key) ，如果不存在，抛出out_of_range异常

# 访问元素

- c.find（k）;返回迭代器，指向第一个关键字为k的元素，若不在则返回尾后

  - 不能用下标运算符判断一个元素是否在容器里，可以用find

- c.count( k) ;返回关键字等于k的个数

- c.lower_bound(k) ;返回迭代器，指向第一个关键字不小于k的元素

- c.upper_bound(k) ;返回迭代器，指向第一个关键字大于k的元素

  - **当没有k元素时**，**lower和upper返回相等的迭代器**，不影响排序的关键字插入位置
  - 当查找的是最大的元素，则upper返回尾后迭代器
  - 关键字不存在时且大于容器中的最大关键字时，lower返回尾后迭代器

- c.equal_range(k) ; 返回迭代器pair, 表示等于k的关键字的范围，k不存在则返回pair<c.end(),c.end()>

- 在multiset、multimap中查找元素，相同的元素会相邻的存储

  - ```
    1、获取边界
    2、迭代器遍历
    法1 ：find()获取首边界，count()获取个数
    法2：c.lower_bound()获取首边界，c.upper_bound()获取尾边界
    法3：equal_range() 获取首尾边界
    ```

# 无序容器

使用哈希函数和关键字类型的==运算符来组织元素

- 允许有重复关键字
- 无序容器在存储上组织为桶，每个桶保存0个或多个元素，使用哈希函数将元素映射到桶里，相同参数，哈希函数必须产生相同结果
- 访问元素时，先计算哈希值，再定位在哪个桶
- 性能依赖于哈希函数的质量和桶数量以及大小

#### 无序容器对关键字类型的要求

默认用关键字类型的==运算符比较，用hash<key_type>类型的对象为每个元素生成一个哈希值。标准库的内置类型可以直接用hash模板

自定义类型的需要提供自己的hash模板。

```
重载函数
eg；为了让Sale_data用作关键字
size_t hasher(const Sale_data &sd){
	return hash<string>()(sd.isbn());
}
bool eqOp(const Sales_data &lhs,const Sales_data &rhs){
	return lhs.isbn() == rhs.isbn();
}
```

