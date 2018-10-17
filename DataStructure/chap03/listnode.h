typedef int Rank;						//秩
#define Posi(T) ListNode<T>*

template <typename T> struct ListNode {
	T data;								//数值
	ListNode<T>* pred;					//前驱
	ListNode<T>* succ;					//后驱

	ListNode() {};
	ListNode(T e, ListNode<T>* p, ListNode<T>* s): data(e), pred(p), succ(s) {};

	ListNode<T>* insertAsPred(T const& e);
	ListNode<T>* insertAsSucc(T const& e);
}
