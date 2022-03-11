flex scanner.l 
bison -d parser.y
gcc -c *.c -fno-builtin-exp -Wno-implicit-function-declaration
gcc *.o -lfl -o compilar -fno-builtin-exp