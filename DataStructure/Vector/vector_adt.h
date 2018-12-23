#ifndef VECTOR_HEADER
#define VECTOR_HEADER
#include <iostream>

typedef int Rank; //秩
#define DEFAULT_CAPACITY 3 //默认的初始容量

template <typename T> class Vector{ //向量模板类
	protected:
		Rank _size; int _capacity; T* _elem;
		void copyFrom(T const* A, Rank lo, Rank hi);
		void expand();
		void shrink();
		bool bubble(Rank lo, Rank hi);  //扫描交换
		void bubbleSort(Rank lo, Rank hi); //冒泡排序算法
		//max(Rank lo, Rank hi); //选取最大元素
		//selectionSort(Rank lo, Rank hi); //选择排序算法
		//merge(Rank lo, Rank mi, Rank hi); //归并算法
		//mergeSort(Rank lo, Rank hi); //归并排序算法
		//partition(Rank lo, Rank hi); //轴点构造算法
		//quickSort(Rank lo, Rank hi); //快速排序算法
		//heapSort(Rank lo, Rank hi); //堆排序
	public:
		//构造函数
		Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = 0 )		//容量为c,规模为s,所有元素初始为v
		{
			_elem = new T[_capacity = c];							//_capacity = c; new T[capacity]
			for (_size = 0; _size < s; _elem[_size ++] = v);
		}
		Vector(T const* A, Rank lo, Rank hi) {copyFrom(A,lo,hi);}	//数组区间复制
		Vector(T const* A, Rank n) {copyFrom(A,0,n);}				//数组整体复制
		Vector(Vector<T> const &V, Rank lo, Rank hi) {copyFrom(V._elem, lo, hi);} //向量整体复制
		Vector(Vector<T> const &V) {copyFrom(V._elem, 0, V._size);}
		//析构函数
		~Vector() {delete [] _elem;}
		//custom function
		void Print()
		{
			for(int i = 0; i < _size; std::cout << _elem[i++] << ' ');
			std::cout << std::endl;
		}
		int capacity() const;										//const成员函数:可以访问但不允许修改数据成员。
		T get(Rank r) const;
		//只读访问接口: 注意成员函数均为const成员函数
		Rank size() const;											//规模
		bool empty() const;											//判空
		//disordered() //判断向量是否已排序
		Rank find(const T& e) const {return find(e, 0, _size); }	//无序向量整体查找
		Rank find(const T& e, Rank lo, Rank hi) const;				//无序向量区间查找
		int remove(Rank lo, Rank hi);
		T remove(Rank r);
		//Rank search() const;										//有序向量整体查找
		Rank search(T const &e, Rank lo, Rank hi) const;				//有序向量区间查找
		//可写访问接口
		T& operator [] (Rank r) const;
		Rank insert( Rank r, const T& e);
		Rank insert( const T& e) { insert(_size, e);}
		//sort() //排序
		void unsort(Rank lo, Rank hi);								//置乱
		void unsort(){ unsort(0,_size); }
		int deduplicate();											//无序去重
		int uniquify();												//有序去重
        void traverse(void (*vist)(T& e) );
		template <typename VST> void traverse(VST& visit);
}; // Vector

#include "vector_implementation.h"
#include "vector_adt.cc"
#endif
