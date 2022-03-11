# Compiler for C Minus Language

This project is a complete compiler with a lexical, syntactic and semantic analyzer, and a generator of intermediate, assembly and binary code for the "processor in verilog" project.

The project uses the Flex and YACC Bison tools as facilitators in the construction of the lexical and syntactic analyzer, respectively.

The Flex and YACC Bison can be installed in a Linux System using the following commands:

```
sudo apt-get update 
sudo apt-get upgrade 
sudo apt-get install flex bison
```

To run the compiler in a specific c-minus algorithm follow the instructions:

First, grant permission to the .sh files:

```
chmod +x compile.sh
chmod +x clear.sh
```

In the main.c file, change the input_code variable to the desired code to be compiled. Save, and run the following command:

```
./compile.sh
./compilar
```

If you want to delete all the mounted files, run the clear.sh directive.

```
./clear.sh
```