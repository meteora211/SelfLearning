#include "../Vector/vector_adt.h"

template <typename T> 
class Stack : public Vector<T> {
public:
    using Vector<T>::size;
    using Vector<T>::insert;
    void push(const T & e)
    {
        insert(size(),e);
    }
    T pop()
    {
        return remove(size() - 1);
    }
    T& top()
    {
        return (*this)[size() - 1];
    }
};
