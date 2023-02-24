gcc -o nopic.o -c pic.c -fno-PIC
gcc -o pic.o -c pic.c -fPIC

objdump -s -d ./nopic.o > no_pic_asm
# objdump -R ./nopic.o > no_pic_rel
objdump -s -d ./pic.o > pic_asm
# objdump -R ./pic.o > pic_rel
