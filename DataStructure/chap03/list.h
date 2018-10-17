#include "listnode.h"

template <typename T> class List {
private:
	int _size;
	Posi(T) header;
	Posi(T) trailer;
protected:
	void init();									//创建时的初始化

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
	Rank find(const T& e) const;

	//可写访问接口
}
