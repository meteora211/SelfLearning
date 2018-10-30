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

//---------------------------------------------------
// Utils
//---------------------------------------------------

template <typename T> void listprint(Posi(T) p)
{
    while(p != NULL && p -> pred != NULL)
    {
        std::cout << p -> data << " -> ";
        p = p -> pred;
    }
    if(p != NULL)
    {
        std::cout << p-> data << std::endl;
    }
}

template <typename T> void listprint(List<T>& l)
{
    listprint(l.first());
}
