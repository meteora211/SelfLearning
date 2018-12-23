## Vector

### 局部变量的引用
在继承类Stack中使用remove方法发现会导致程序segment fault，debug之后发现问题出现在以下代码
```cpp
template <typename T> T& Vector<T>::remove(Rank r)
{
	T e = _elem[r];
	remove(r,r+1);
	return e;
}
```
注意到 该函数返回值e是一个局部变量，返回的类型是该局部变量的引用。在程序执行完之后，局部变量自动销毁，自然这个引用就是无效的。
