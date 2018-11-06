#pragma once
#include <iostream>
#include "list.h"
//---------------------------------------------------
// Constructors
//---------------------------------------------------
template <typename T> void List<T>::init()
{
    header = new ListNode<T>;
    trailer = new ListNode<T>;
    header -> succ = trailer; header -> pred = NULL;
    trailer -> pred = header; trailer -> succ = NULL;
    _size = 0;
}

template <typename T> List<T>::List(const List<T>& L)
{
    copyNodes(L.first(),L._size);
}

template <typename T> List<T>::List(const List<T>& L, Rank r, int n)
{
    /* 原版这里应该有错
       L[r]返回的是对数据T的引用 T&
       而copyNodes接收的第一个参数为ListNode<T>*
    */
    //copyNodes(L[r],n);
}

template <typename T> List<T>::List(Posi(T) p, int n)
{
    copyNodes(p,n);
}

template <typename T> List<T>::~List()
{
    clear();
    delete header;
    delete trailer;
}

template <typename T> Posi(T) List<T>::first() const
{
    return header -> succ;
}

template <typename T> Posi(T) List<T>::last() const
{
    return trailer -> pred;
}

template <typename T> Posi(T) List<T>::find(const T& e) const
{
    return find(e, _size, trailer);
}

template <typename T> Posi(T) List<T>::find(const T& e, int n, Posi(T) p) const
{
    while(0 < n--)
    {
        if(e == (p = p -> pred) -> data)
        {
            return p;
        }
    }
    return NULL;
}

template <typename T> T& List<T>::operator [] (Rank r) const
{
    Posi(T) p = first();
    //if( r >= _size) return -1;
    while( r-- > 0 )
    {
        p = p->succ;
    }
    return p-> data;
}

//在有序列表内节点p(可能是trailer)的n个(真)前驱中，找到不大于e的最后者
template <typename T> Posi(T) List<T>::search(const T& e, int n, Posi(T) p) const
{

    while(n-- >= 0 && p != header)
    {
        if((p = p -> pred) -> data <= e)
        {
            break;
        }
    }
    return p;
}

template <typename T> Posi(T) List<T>::selectMax(Posi(T) p, int n) const
{
    Posi(T) max = p;
    Posi(T) cur = p;
    while(n-- > 1)
    {
        if(max->data < (cur = cur -> succ) -> data)
        {
            max = cur;
        }
    }
    return max;
}

// 可写访问接口
template <typename T> void List<T>::copyNodes(Posi(T) p, int n)
{
    init();
    while(n--)
    {
        insertAsLast(p -> data);
        p = p -> succ;
    }
}

template <typename T> int List<T>::clear()
{
    int oldSize = _size;
    while(_size > 0)
    {
        remove(header -> succ);
    }
    return oldSize;
}

template <typename T> Posi(T) List<T>::insertAsFirst(const T& e)
{
    _size ++;
    return header -> insertAsSucc(e);
}

template <typename T> Posi(T) List<T>::insertAsLast(const T& e)
{
    _size++;
    return trailer -> insertAsPred(e);
}

template <typename T> Posi(T) List<T>::insertBefore(Posi(T) p, const T& e)
{
    return p -> insertAsPred(e);
}

template <typename T> Posi(T) List<T>::insertAfter(Posi(T) p, const T& e)
{
    return p -> insertAsSucc(e);
}

template <typename T> T List<T>::remove(Posi(T) p)
{
    T e = p -> data;
    p -> pred -> succ = p -> succ;
    p -> succ -> pred = p -> pred;
    delete p; _size --;
    return e;
}

template <typename T> int List<T>::deduplicate()
{
    //删除无序列表中的重复节点
    if(_size < 2) return 0;
    int oldSize = _size;
    Posi(T) p = header -> succ;
    Rank r = 0;
    while(p != trailer)
    {
        Posi(T) target = find(p->data, r, p);
        if( target != NULL)
        {
            remove(target);
        }
        else
        {
            r++;
        }
        p = p -> succ;
    }
    return oldSize - _size;
}

template <typename T> void List<T>::insertionSort(Posi(T) p, int n)
{
    for(int index = 0; index < n; index++)
    {
        insertAfter(search(p->data, index, p), p -> data);
        p = p -> succ;
        remove(p->pred);
    }
}

template <typename T> void List<T>::selectionSort(Posi(T) p, int n)
{
    Posi(T) head = p -> pred; Posi(T) tail = p;
    for(int i = 0; i < n; i++) tail = tail -> succ;
    while(n > 1)
    {
        Posi(T) tempmax = selectMax(head->succ,n);
        insertBefore(tail,remove(tempmax));
        n--;
        tail = tail -> pred;
    }
}
/*
template <typename T> void List<T>::traverse(void (*VISIT)(T& ))
{
    for(Posi(T) p = header; p != trailer; p = p -> succ)
    {
        VISIT(p->data);
    }
}
*/
template <typename T> void List<T>::traverse(void (*VISIT)(Posi(T) p))
{
    Posi(T) p = header;
    while(trailer != (p = p -> succ))
    {
        VISIT(p);
    }
}

template <typename T> template <typename VST> void List<T>::traverse(VST& VISIT)
{
    for(Posi(T) p = header; p != trailer; p = p -> succ)
    {
        VISIT(p->data);
    }
}

template <typename T> int List<T>::uniquify()
{
    if(_size < 2) return 0;
    int oldSize = _size;
    Posi(T) p = header;
    while(last() != p && p != trailer)
    {
        if(p -> succ -> data == p -> data)
        {
            remove(p -> succ);
        }
        p = p -> succ;
    }
    return oldSize - _size;
}

//---------------------------------------------------
// Utils
//---------------------------------------------------

template <typename T> void listprint(Posi(T) p)
{
    std::cout << "header -> ";
    while(p != NULL && p -> succ != NULL)
    {
        std::cout << p -> data << " -> ";
        p = p -> succ;
    }
    if(p != NULL && p -> succ == NULL)
    {
        std::cout << "trailer" << std::endl;
    }
}

template <typename T> void listprint(List<T>& l)
{
    listprint(l.first());
}

template <typename T> void listnodeprint(Posi(T) p)
{
    std::cout << p->data << std::endl;
}

/*
template <typename T> void Increase(T& data )
{
    data++;
}
*/
template <typename T> void Increase(Posi(T) p)
{
    (p->data)++;
}
