#include "../Vector/vector_adt.h"

template <typename T> 
class Stack : public Vector<T> {
public:
    // method 1 to look up the derived member function: using name
    using Vector<T>::size;
    using Vector<T>::insert;
    void push(const T & e)
    {
        insert(size(),e);
    }
    T pop()
    {
        // method 2 to look up the derived member function: this pointer
        return this -> remove(this -> size() - 1);
    }
    T& top()
    {
        return (*this)[size() - 1];
    }
};
