# use -m32 in 64bit machine
gcc -c -fno-builtin -m32 TinyHelloWorld_32bit.c
ld -static -m elf_i386 -T link.lds -e nomain -o TinyHelloWorld_32bit TinyHelloWorld_32bit.o
./TinyHelloWorld_32bit
