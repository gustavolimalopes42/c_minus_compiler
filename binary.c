#include "globals.h"
#include "symtab.h"
#include "codintermedgen.h"
#include "assembly.h"
#include <string.h>

const char *Prefixos[] = { "add", "sub", "mult", "divi", "and", "or", "nand", "nor", "sle","slt", "sge","mod", "addi", "subi", "divim", "multi", "andi", "ori",
                             "nori", "slei", "slti", "beq", "bne", "blt", "bgt", "swi", "lwi", "swr", "lwr","move","put", "halt", "in", "out", "jump", "jal", "jr"};

const char *opcodeBins[] =   {"00000", "00000", "00000", "00000", "00000", "00000", "00000", "00000", "00000", "00000", "00000","00000", 
                              "00001", "00010", "00011", "00100", "00101", "00110", "00111", "01000", "01001", "01010", "01011",
                              "01100", "01101", "01110", "01111", "10000", "10001", "10010", "10011", "10100", "10101", "10110",
                              "10111", "10110", "11010"};

const char *functBins[] = { "00000", "00001", "00010", "00011", "00100", "00101", "00110", "00111", "01000", "01001", "01010", "01011"};

void binary_generator(AssemblyCode assembly_line){

    Instruction inst;
    
    if(assembly_line->kind == instr){
        inst = assembly_line->line.instruction;
        switch(inst.format){
        case formatR:
            fprintf(binary_code,"ram[%d] = {5'b%s, 6'd%d, 6'd%d, 6'd%d, 4'd0, 5'b%s};",assembly_line->lineno,
                                                             opcodeBins[inst.opcode],
                                                             inst.reg2,
                                                             inst.reg3,
                                                             inst.reg1,
                                                             functBins[inst.opcode]);
            fprintf(binary_code,"    // %s\n",Prefixos[inst.opcode]);
            break;
        case formatJ:
            if(inst.opcode == jr){
                fprintf(binary_code,"ram[%d] = {5'b%s, 27'd%d};",assembly_line->lineno,
                                                           opcodeBins[inst.opcode],inst.reg1);
                fprintf(binary_code,"       // %s\n",Prefixos[inst.opcode]);
            }else{
                fprintf(binary_code,"ram[%d] = {5'b%s, 27'd%d};",assembly_line->lineno,
                                                           opcodeBins[inst.opcode],
                                                           inst.imed);
                fprintf(binary_code,"       // %s\n",Prefixos[inst.opcode]);
            }
            break;
        case formatI:

            if(inst.opcode == halt ){
                fprintf(binary_code,"ram[%d] = {6'b%s, 26'd0};",assembly_line->lineno,
                                                           opcodeBins[inst.opcode]);
                fprintf(binary_code,"       // %s\n",Prefixos[inst.opcode]);
            }else if(inst.opcode == swi || inst.opcode == lwi){

                fprintf(binary_code,"ram[%d] = {5'b%s, 6'd0, 6'd%d, 15'd%d};",assembly_line->lineno,
                                                           opcodeBins[inst.opcode],
                                                           inst.reg1,
                                                           inst.imed);
                fprintf(binary_code,"       // %s\n",Prefixos[inst.opcode]);
            }else if(inst.opcode == bgt || inst.opcode == blt){
                fprintf(binary_code,"ram[%d] = {5'b%s, 6'd%d, 6'd%d, 15'd%d};",assembly_line->lineno,
                                                           opcodeBins[inst.opcode],
                                                           inst.reg1,
                                                           inst.reg2,
                                                           inst.imed);
                fprintf(binary_code,"       // %s\n",Prefixos[inst.opcode]);
            }else if(inst.opcode == move || inst.opcode == put){
                fprintf(binary_code,"ram[%d] = {5'b%s, 6'd%d, 6'd%d, 15'd%d};",assembly_line->lineno,
                                                           opcodeBins[addi],
                                                           inst.reg2,
                                                           inst.reg1,
                                                           inst.imed);
                fprintf(binary_code,"       // %s\n",Prefixos[inst.opcode]);
            }else{
                fprintf(binary_code,"ram[%d] = {5'b%s, 6'd%d, 6'd%d, 15'd%d};",assembly_line->lineno,
                                                           opcodeBins[inst.opcode],
                                                           inst.reg2,
                                                           inst.reg1,
                                                           inst.imed);
                fprintf(binary_code,"       // %s\n",Prefixos[inst.opcode]);
            }
            break;
        
        }
    }else{
        fprintf(binary_code,"// %s\n",assembly_line->line.label);
    }
}

void generateBinary () {
    
    AssemblyCode assembly_line = getAssembly();

    while (assembly_line != NULL && assembly_code != NULL) {
        binary_generator(assembly_line);
        assembly_line = assembly_line->next;
    }
}
