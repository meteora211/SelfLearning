#pragma once
#include "listnode.h"

template <typename T> class List {
private:
	int _size;
	Posi(T) header;
	Posi(T) trailer;
protected:
	void init();									//创建时的初始化
    int clear();                                    //清除所有节点
    void copyNodes(Posi(T) p, int n);               //复制列表中自p起的n项
    
public:
	//构造函数
	List(){ init();}								//默认构造函数
	List(const List<T>& L);							//整体复制列表L
	List(const List<T>& L, Rank r, int n);			//复制列表L中自第r项起的n项
	List(Posi(T) p, int n);							//复制列表中自位置p起的n项
	//析构函数
	~List();										//释放（包含头，尾哨兵在内的）所有节点

	//只读访问接口
	Rank size() const {return _size;}
	bool empty() const {return _size <=0;}
	Posi(T) first() const;
	Posi(T) last() const;
    Posi(T) find(const T& e) const;                    //无序列表查找
	Posi(T) find(const T& e, int n, Posi(T) p) const;  //无序区间查找
    T& operator[](Rank r) const;
    bool valid(Posi(T) p) const                         //判断位置p是否合法
    {return p && p != trailer && p != header;}
    Posi(T) search(const T&, int n, Posi(T) p) const;
    Posi(T) selectMax() const {return selectMax(header->succ,_size);}
    Posi(T) selectMax(Posi(T) p, int n) const;

    //可写访问接口
    Posi(T) insertAsFirst(const T& e);  //作为首节点插入
    Posi(T) insertAsLast(const T& e);   //作为末节点插入
    Posi(T) insertBefore(Posi(T) p, const T& e);    //将e作为p的前驱插入
    Posi(T) insertAfter(Posi(T) p, const T& e);     //将e作为p的后继插入
    T remove(Posi(T) p);                //删除合法位置p处的节点
    int deduplicate();                  //剔除无序列表中的重复节点
    int uniquify();
    void traverse(void (*VISIT)(ListNode<T>* p));
    template <typename VST>void traverse(VST& VISIT);
    void insertionSort(Posi(T) p, int n);
    void selectionSort(Posi(T) p, int n);
   // void traverse(void (*VISIT)(T& data));
};

#include "list.cc"
