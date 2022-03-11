typedef enum { formatR, formatI, formatJ } InstrFormat;

typedef enum { instr, lbl } LineKind;

typedef enum { unique, vector, address } VarKind;

typedef enum { $zero,$r1,$r2,$r3,$r4,$r5,$r6,$r7,$r8,$r9,$r10,$r11,$r12,$r13,$r14,$r15,$r16,$r17,$r18,$r19,$r20,$r21,$r22,$r23,$r24,$r25,$r26,$r27,$r28,$r29,$r30,
                $a1,$a2,$a3,$a4,$a5,$a6,$a7,$a8,$a9,$a10,$a11,$a12,$a13,$a14,$a15,$a16,$a17,$a18,$a19,$a20,$a21,$a22,$a23,$a24,$a25,$a26,$a27,$a28,$ra,$gp, 
                $sp, $ret,$fp,none} Reg; //64 Registradores no Banco de Registradores mais a enumeração none para a instrução que não usar todos os registradores

typedef enum { add, sub, mult, divi, and, or, nand, nor, sle, slt, sge, mod, addi, subi, divim, multi, andi, ori,
               nori, slei, slti, beq, bne, blt, bgt, swi, lwi, swr, lwr, move, put, halt, in, out, jump, jal, jr 
              } InstrKind; //Instruções da arquitetura do Processador Jarvis


typedef struct {
    InstrFormat format;
    InstrKind opcode;
    Reg reg1;
    Reg reg2;
    Reg reg3;
    int imed;
    char * label;
} Instruction;

typedef struct AssemblyCodeRec {
    int lineno;
    LineKind kind;
    union {
        Instruction instruction;
        char * label;
     }line;
    struct AssemblyCodeRec * next;
} * AssemblyCode;

typedef struct VarListRec {
    char * id;
    int size;
    int memloc;
    VarKind kind;
    struct VarListRec * next;
} * VarList;

typedef struct FunListRec {
    char * id;
    int size;
    VarList vars;
    struct FunListRec * next;
} * FunList;

void generateAssembly (QuadList head);

AssemblyCode getAssembly ();

void printAssembly ();

int getSize();