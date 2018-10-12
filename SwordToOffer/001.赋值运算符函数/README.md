## 题目：如下为类型CMyString的声明，请为该类型添加赋值运算符函数
```c++
class CMyString
{
public:
    CMyString(char* pData = nullptr);
    CMyString(const CMyString& str);
    ~CMyString(void);
private:
    char* m_pData;
}
```

### Notes:
* 返回值类型应声明为该类的引用：
  * 连续赋值
  * 效率
* 返回*this
* 形参声明为const&：
  * 如果不是引用而是实例，传入形参时会调用复制构造函数，影响效率
  * const形参可以传入const类型的实例
* 重新分配内存时要先释放已有的内存，否则会有内存泄漏
* 判断传入的形参是否为自身

### 异常安全性原则(Exception Safty):
### 测试用例
