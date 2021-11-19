#include <iostream>

// set_new_handler is a function pointer and is called when malloc failed.
// following define is not required since std provide default function.
// namespace std {
//   typedef void (*new_handler) ();
//   new_handler set_new_handler(new_handler p) throw();
// }

void outOfMem() {
  std::cerr << "Unable to satisfy request for memory.\n";
  std::abort();
}

class NewHandlerHolder {
public:
  explicit NewHandlerHolder(std::new_handler h): h_(h) {
    std::cout << "NewHandlerHolder Constructor" << std::endl;;
  };
  ~NewHandlerHolder() {
    std::cout << "NewHandlerHolder Deconstructor" << std::endl;;
    std::set_new_handler(h_);
  }
private:
  std::new_handler h_;
  NewHandlerHolder& operator=(const NewHandlerHolder&) { return *this; };
  NewHandlerHolder(const NewHandlerHolder&) {};
};

template<typename T>
class NewHandlerSupport {
public:
  NewHandlerSupport() {std::cout << "NewHandlerSupport Constructor" << std::endl;;};
  ~NewHandlerSupport() {std::cout << "NewHandlerSupport Deconstructor" << std::endl;;};
  static std::new_handler set_new_handler(std::new_handler p) throw();
  static void* operator new(std::size_t size) throw(std::bad_alloc);
private:
  static std::new_handler currentHandler;
};

template<typename T>
std::new_handler NewHandlerSupport<T>::currentHandler = 0;

template<typename T>
std::new_handler NewHandlerSupport<T>::set_new_handler(std::new_handler p) throw() {
  std::cout << "set handler" << std::endl;
  std::new_handler oldHandler = currentHandler;
  currentHandler = p;
  return oldHandler;
}

template<typename T>
void* NewHandlerSupport<T>::operator new(std::size_t size) throw(std::bad_alloc) {
  std::cout << "operator new std::set_new_handler" << std::endl;
  // XXX: Following code still use default new handler since it deconstruct NewHandlerHolder before new is called.
  // NewHandlerHolder(std::set_new_handler(currentHandler)); */
  NewHandlerHolder h(std::set_new_handler(currentHandler));
  std::cout << "size: " << size << std::endl;
  void* p = ::operator new(size);
  std::cout << "operator new return" << std::endl;
  return p;
}

class Widget : public NewHandlerSupport<Widget> {
public:
  Widget() {
    std::cout << "Widget Construct" << std::endl;
  }
  ~Widget() {
    std::cout << "Widget Deconstruct" << std::endl;
  }
private:
  int* p_[10000000000L];
};

// Non-template implementation
/* class Widget { */
/* public: */
/*   Widget() { */
/*     std::cout << "Construct" << std::endl; */
/*     p_ = new int[100000000000L]; */
/*   } */
/*   ~Widget() { */
/*     std::cout << "Deconstruct" << std::endl; */
/*     delete [] p_; */
/*   } */
/*   static std::new_handler set_new_handler(std::new_handler p) throw(); */
/*   void* operator new(std::size_t size) throw(std::bad_alloc); */
/* private: */
/*   static std::new_handler currentHandler; */
/*   int* p_; */
/* }; */

/* std::new_handler Widget::set_new_handler(std::new_handler p) throw() { */
/*   std::new_handler oldHandler = currentHandler; */
/*   currentHandler = p; */
/*   return oldHandler; */
/* } */

/* void* Widget::operator new(std::size_t size) throw(std::bad_alloc) { */
/*   return ::operator new(size); */
/* } */

/* std::new_handler Widget::currentHandler = 0; */

int main() {

  // setting defualt new hanlder
  /* std::set_new_handler(outOfMem); */
  /* int* pBigData = new int[100000000000L]; */
  /* std::cout << pBigData << std::endl; */
  /* delete [] pBigData; */

  // setting Widget new hanlder with non-template implementation
  /* Widget::set_new_handler(outOfMem); */
  /* Widget* A = new Widget; */
  /* delete A; */

  // setting Widget new hanlder with template implementation
  Widget::set_new_handler(outOfMem);
  Widget* A = new Widget;
  delete A;
}
