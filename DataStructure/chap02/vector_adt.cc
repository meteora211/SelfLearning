#include "vector_adt.h"
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
template <typename T> void Vector<T>::copyFrom(T const* A, Rank lo, Rank hi)
{
	_elem = new T[_capacity = 2*(hi - lo)];
	_size = 0;
	while(lo < hi)
	{
		_elem[_size++] = A[lo++];
	}
}

template <typename T> void Vector<T>::expand()
{
	if( _size< _capacity) return;
	_capacity = std::max(_capacity, DEFAULT_CAPACITY);
	T* oldElem = _elem;
	_elem = new T[_capacity <<= 1];
	for(int i = 0; i <_size; i ++)
	{
		_elem[i] = oldElem[i];
	}
	//比较for-loop与以下代码的区别:
	//memcpy(_elem, oldElem, _size * sizeof(T));
	//当T为非基本类型且有对应的赋值运算符以执行深复制时，前一段代码会调用复制运算符进行深复制，而后一段只能进行浅复制
	delete [] oldElem;
}

template <typename T> void Vector<T>::shrink()
{
	if(_capacity < DEFAULT_CAPACITY << 1) return;
	if(_size<<2 > _capacity) return;
	T* oldElem = _elem;
	_elem = new T[_capacity >>= 1];
	for(int i = 0; i < _size; i ++)
	{
		_elem[i] = oldElem[i];
	}
	delete [] oldElem;//new - delete 不要忘记
}

template <typename T> Rank Vector<T>::size() const
{
	return _size;
}


template <typename T> int Vector<T>::capacity() const
{
	return _capacity;
}

template <typename T> T Vector<T>::get(Rank r) const
{
	return r < _size && r >= 0 ? _elem[r] : NULL;
}

template <typename T> bool Vector<T>::empty() const
{
	return !_size;
}

template <typename T> T& Vector<T>::operator [] (Rank r) const
{
	return _elem[r]; //assert: 0<=r<_size
}

template <typename T> void Vector<T>::unsort(Rank lo, Rank hi)
{
	//随机数产生方法:
	//rand() % a + b will generate a random number in [b,a+b)
	while(lo < hi)
	{
		std::swap(_elem[lo++],_elem[rand() % (hi - lo) + lo]);
	}
}

template <typename T> Rank Vector<T>::find(const T& e, Rank lo, Rank hi) const
{
	while((lo < hi--) && (_elem[hi] != e));
	return hi;
}

template <typename T> Rank Vector<T>::insert(Rank r, const T& e)
{
	_size ++;
	expand();
	for(Rank i = _size; i > r; i--)
	{
		_elem[i] = _elem[i-1];
	}
	_elem[r] = e;
	return r;
}

template <typename T> int Vector<T>::remove(Rank lo, Rank hi)
{
	if(lo == hi) return 0; //出于效率考虑，单独处理退化情况，比如remove(0,0)
	for(int i = 0; i < _size - hi; i ++)
	{
		_elem[lo + i] = _elem[hi + i];
	}
	_size = _size - hi + lo;
	shrink();
	return hi - lo;
}

template <typename T> T& Vector<T>::remove(Rank r)
{
	T e = _elem[r];
	remove(r,r+1);
	return e;
}

template <typename T> int Vector<T>::deduplicate()
{
	int _oldSize = _size;
	Rank i = 1;
	while(i < _size)
	{
		(find(_elem[i],0,i) > 0) ? i++ : remove(i);
	}
	shrink();
	return _size - _oldSize;
}

template <typename T> void Vector<T>::traverse(void (*visit)(T& e))
{
	puts("function pointer method");
	for(int i = 0; i < _size; i ++)
	{
		visit(_elem[i]);
	}
}

template <typename T> template <typename VST> void Vector<T>::traverse(VST& visit)
{
	puts("function object method");
	for(int i = 0; i < _size; i ++)
	{
		visit(_elem[i]);
	}
}

template <typename T> int Vector<T>::uniquify()
{
	int oldSize = _size;
	/*
	int i = 0;
	while(i < _size - 1)
	{
		_elem[i] == _elem[i+1] ? remove(i,i+1) : i++;
	}
	*/
	Rank lo = 0, hi = 0;
	while(++hi < _size)
	{
		if(_elem[lo] != _elem[hi])
		{
			_elem[++lo] = _elem[hi];
		}
	}
	_size = ++lo;
	shrink();
	return oldSize - _size;
}

template <typename T> Rank Vector<T>::search(T const &e, Rank lo, Rank hi) const
{
	return binSearch(_elem, e, lo, hi);
}

//test fuctions
template <typename T> struct Increase //函数对象：递增一个T类对象
{
	virtual void operator() ( T& e ) { e++; }  
}; //假设T可直接递增或已重载++

template <typename T> void increase ( Vector<T> & V ) //统一递增向量中的各元素
{
	//This works as expected
	Increase<T> IncreaseObject;
	V.traverse ( IncreaseObject );
	//This cause an error: invalid initialization of non-const reference of type ‘Increase<int>&’ from an rvalue of type ‘Increase<int>’
	//V.traverse( Increase<T>() );
} //以Increase<T>()为基本操作进行遍历

