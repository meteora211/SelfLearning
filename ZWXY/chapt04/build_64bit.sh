# gcc -c -fno-builtin -O3 TinyHelloWorld_64bit.c
# ld -static -T link_64bit.lds -s -e nomain -o TinyHelloWorld_64bit TinyHelloWorld_64bit.o

gcc TinyHelloWorld_64bit.c -s -O3 -e nomain -fno-builtin -nostartfiles -nostdlib -T link_64bit.lds -o TinyHelloWorld_64bit
./TinyHelloWorld_64bit
