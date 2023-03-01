#include <iostream>
#include <memory>

struct VideoFile;
struct TextFile;

struct Visitor {
  Visitor() = default;
  virtual ~Visitor() = default;

  virtual void visit(VideoFile&) = 0;
  virtual void visit(TextFile&) = 0;
};


struct Elem {
  Elem() = default;
  virtual ~Elem() = default;

  virtual void accept(Visitor&) = 0;

  virtual void print() = 0;
};

template <typename Derived>
struct CRTPElem : public Elem, public std::enable_shared_from_this<CRTPElem<Derived>> {
  virtual ~CRTPElem() = default;

  virtual void accept(Visitor& v) {
    v.visit(static_cast<Derived&>(*this));
  }

  virtual void print() {
    std::cout << "CRTPElem print" << std::endl;
  }

protected:
  CRTPElem() = default;
};

struct VideoFile final : public CRTPElem<VideoFile> {
  VideoFile() = default;
  ~VideoFile() = default;

  // WITHOUT CRTP: each derived class would has same member function
  // void accept(Visitor& v) override {
  //   v.visit(*this);
  // }
  void print() override {
    std::cout << "VideoFile print" << std::endl;
  }
};

struct TextFile final : public CRTPElem<TextFile> {
  TextFile() = default;
  ~TextFile() = default;

  // WITHOUT CRTP: each derived class would has same member function
  // void accept(Visitor& v) override {
  //   v.visit(*this);
  // }
  void print() override {
    std::cout << "TextFile print" << std::endl;
  }
};


struct PrintVisitor final : public Visitor {
  PrintVisitor() = default;
  ~PrintVisitor() = default;

  void visit(VideoFile& v) override {
    v.print();
  }
  void visit(TextFile& t) override {
    t.print();
  }
};

int main() {
  auto vf = std::make_shared<VideoFile>();
  auto tf = std::make_shared<TextFile>();

  PrintVisitor pv;
  vf->accept(pv);
  tf->accept(pv);
}
