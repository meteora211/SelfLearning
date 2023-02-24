char *str = "Hello, World\n";

void print(){
  asm("movq $1, %%rax \n"   // syscall table 1 -> write
      "movq $1, %%rdi \n"   // int filedesc
      "movq %0, %%rsi \n"   // char* buffer
      "movq $13, %%rdx \n"  // int size
      "syscall \n"
      :                     // no output
      : "r"(str)            // input str as %0
      : "rax", "rdi", "rsi", "rdx");
}

void exit() {
  asm("movq $60, %rax \n"
      "movq $42, %rdi \n"
      "syscall \n");
}

int nomain() {
  print();
  exit();
}
