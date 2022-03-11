#include "globals.h"
#include "symtab.h"
#include "codintermedgen.h"
#include "assembly.h"

const char *regNames[] = { "$zero", "$r1","$r2", "$r3", "$r4", "$r5", "$r6", "$r7", "$r8", "$r9", "$r10", "$r11", "$r12", "$r13", "$r14", "$r15",
                            "$r16","$r17","$r18","$r19","$r20","$r21","$r22","$r23","$r24","$r25","$r26","$r27","$r28","$r29","$r30",
                            "$a1", "$a2", "$a3", "$a4", "$a5", "$a6", "$a7", "$a8", "$a9", "$a10","$a11","$a12","$a13","$a14","$a15","$a16","$a17",
                            "$a18","$a19","$a20","$a21","$a22","$a23","$a24","$a25","$a26","$a27","$a28","$ra","$gp","$sp","$ret", "$fp",""};

const char *InstrNames[] = { "add", "sub", "mult", "i", "and", "or", "nand", "nor", "sle","slt", "sge","mod", "addi", "subi", "divim", "multi", "andi", "ori",
                             "nori", "slei", "slti", "beq", "bne", "blt", "bgt", "swi", "lwi", "swr", "lwr","move","put", "halt", "in", "out", "jump", "jal", "jr"};

// Link Processor Jarvis Datapath -  Arquiteture and Organization: https://app.diagrams.net/#G1EfD2d3DZkyisomJK70gSLGZYakk9vf7T

AssemblyCode codehead = NULL;
FunList funlisthead = NULL;

int line;
int nscopes = 0;
int param_count = 30;

int narg = 0;
int jumpmain = 0;

//Função de inserção de lista,no caso insere uma função na lista de funçõesa partir do identificador da função
void insertFun (char * id) {
    FunList new = (FunList) malloc(sizeof(struct FunListRec));
    new->id = (char *) malloc(strlen(id) * sizeof(char));
    strcpy(new->id, id);
    new->size = 0;
    new->next = NULL;
    if (funlisthead == NULL) {
        funlisthead = new;
    }
    else {
        FunList f = funlisthead;
        while (f->next != NULL) f = f->next;
        f->next = new;
    }
    nscopes ++;
}
//Função de inserção de lista,no caso insere uma variavel na lista de uma determinada função partir do identificador da variavel, do escopo dela e do tipo da variavel

void insertVar (char * scope, char * id, int size, VarKind kind) {

    FunList f = funlisthead;
    if(scope == NULL){
    if(kind == 1 )
    scope= f->id;
    else
    scope= f->next->id;
    }
    while (f != NULL && strcmp(f->id, scope) != 0)  f = f->next;
      if (f == NULL) {
        insertFun(scope);
        f = funlisthead;
        while (f != NULL && strcmp(f->id, scope) != 0 ) 
        f = f->next;
    }
    VarList new = (VarList) malloc(sizeof(struct VarListRec));
    new->id = (char *) malloc(strlen(id) * sizeof(char));
    strcpy(new->id, id);
    new->size = size;
    new->memloc = getMemLoc(id,scope);
    new->kind = kind;
    new->next = NULL;
    if (f->vars == NULL) {
        f->vars = new;
    }
    else {
        VarList v = f->vars;
        while (v->next != NULL) v = v->next;
        v->next = new;
    }
    
    f->size = f->size + size;
}

VarKind checkType (QuadList l) {

    QuadList aux = l;
    Quad q = aux->quad;
    aux = aux->next;
    while (aux != NULL && aux->quad.op != opEND) {
        if (aux->quad.op == opVEC && strcmp(aux->quad.addr2.contents.var.name, q.addr1.contents.var.name) == 0) 
            return address;
        aux = aux->next;
    }
    return unique;
}

//Insere um Label como na lista do Assembly

void insertLabel (char * label) {

    AssemblyCode new = (AssemblyCode) malloc(sizeof(struct AssemblyCodeRec));
    new->lineno = line;
    new->kind = lbl;
    new->line.label = (char *) malloc(strlen(label) * sizeof(char));
    strcpy(new->line.label, label);
    new->next = NULL;
    if (codehead == NULL) {
        codehead = new;
    }
    else {
        AssemblyCode a = codehead;
        while (a->next != NULL) a = a->next;
        a->next = new;
    }
}
//Insere uma instrução na lista do assembly

void insertInstruction (InstrFormat format, InstrKind opcode, Reg reg1, Reg reg2, Reg reg3, int imed, char * label) {
    Instruction i;

    i.format = format;
    i.opcode = opcode;
    i.reg1 = reg1;
    i.reg2 = reg2;
    i.reg3 = reg3;
    i.imed = imed;
    if (label != NULL) {
        i.label = (char *) malloc(strlen(label) * sizeof(char));
        strcpy(i.label, label);
    }
    AssemblyCode new = (AssemblyCode) malloc(sizeof(struct AssemblyCodeRec));
    new->lineno = line;
    new->kind = instr;
    new->line.instruction = i;
    new->next = NULL;
    if (codehead == NULL) {
        codehead = new;
    }
    else {
        AssemblyCode a = codehead;
        while (a->next != NULL) a = a->next;
        a->next = new;
    }
    line ++;
}

void instructionR(InstrKind opcode, Reg rf, Reg r1, Reg r2){
    insertInstruction(formatR, opcode, rf, r1,r2, 0, NULL);
}

void instructionI(InstrKind opcode, Reg rf, Reg r1, int imed, char *label){
    insertInstruction(formatI,opcode,rf,r1,$zero,imed,label);
}

void instructionJ (InstrKind opcode, int im, char * imlbl,Reg rf) {
    insertInstruction(formatJ, opcode,rf, $zero, $zero, im, imlbl);
}

// 31 ao 58
Reg getParamReg () {

    param_count++;
    if(param_count==59){
        param_count=31;
    }
    return (Reg) param_count;
}
//vai se 1 até 30
Reg getReg (char * regName) {

    for (int i = 0; i < nregisters; i ++) {
        if (strcmp(regName, regNames[i]) == 0) return (Reg) i;
    }
    return $zero;
}

int getLabelLine (char * label) {
    AssemblyCode a = codehead;
    while (a->next != NULL) {
        if (a->kind == lbl && strcmp(a->line.label, label) == 0) return a->lineno;
        a = a->next;
    }
    return -1;
}

//Busca a variavel na lista de funções e na lista de variaveis da função até encontrar o seu tipo
VarKind getVarKind (char * id, char * scope) {
    FunList f = funlisthead;
    while (f != NULL && strcmp(f->id, scope) != 0) f = f->next;
    if (f == NULL) {
        return unique;
    }
    VarList v = f->vars;
    while (v != NULL) {
        if (strcmp(v->id, id) == 0) return v->kind;
        v = v->next;
    }
    return unique;
}
//Retorna o tamanho da função
int getFunSize (char * id) {
    FunList f = funlisthead;
    while (f != NULL && strcmp(f->id, id) != 0) f = f->next;
    if (f == NULL) return -1;
    return f->size;
}

//Gera as instruções para cada quadrupla de três endereços

void generateInstruction (QuadList l) {
    Quad q;
    Address a1, a2, a3;
    int aux;
    VarKind v;

    //Roda toda a lista de quadruplas gerando o Assembly Code
    while (l != NULL) {

        q = l->quad;
        a1 = q.addr1;
        a2 = q.addr2;
        a3 = q.addr3;
        FunList g = funlisthead;
        switch (q.op) {
            
            case opMOVE:
                instructionI(move, getReg(a1.contents.var.name), getReg(a2.contents.var.name), a3.contents.val, NULL);
                break;

            case opPUT:
                instructionI(put, getReg(a1.contents.var.name), getReg(a2.contents.var.name), a3.contents.val, NULL);
                break;
            
            case opADD:
                instructionR(add, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opSUB:
                instructionR(sub, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opMULT:
                instructionR(mult, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opDIV:
                instructionR(divi, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opSLE:
                instructionR(sle, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opSGE:
                instructionR(sge, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opAND:
                instructionR(and, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opOR:
                instructionR(or, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opBGT:
                instructionI(bgt, getReg(a1.contents.var.name), getReg(a2.contents.var.name), -1, a3.contents.var.name);
                break;
    
            case opBLT:
                instructionI(blt, getReg(a1.contents.var.name), getReg(a2.contents.var.name), -1, a3.contents.var.name);
                break;
    
            case opBEQ:
                instructionI(beq, getReg(a1.contents.var.name), getReg(a2.contents.var.name), -1, a3.contents.var.name);
                break;
    
            case opBNE:
                instructionI(bne, getReg(a1.contents.var.name), getReg(a2.contents.var.name), -1, a3.contents.var.name);
                break;
            
            case opASSIGN: 
                instructionI(move, getReg(a1.contents.var.name), getReg(a2.contents.var.name),0,NULL);
                break;
            
            //A instrução ALLOC n tem instruções associadas, ela simplesmente insere a variavel na lista de variaveis de sua respectiva função na lista de funções
            case opALLOC:

                if (a2.contents.val == 1){
                 insertVar(a3.contents.var.scope, a1.contents.var.name, a2.contents.val, unique); //Alocação de variavel unica
                }
                else {
                    insertVar(a3.contents.var.scope, a1.contents.var.name, a2.contents.val, vector); //Alocação de vetor 
                }

                break;
            
            case opADDI:
                instructionI(addi, getReg(a1.contents.var.name), getReg(a2.contents.var.name), a3.contents.val, NULL);
                break;
    
            case opLOAD:
                //O segundo endereço é o registrador de destino e o terceiro endereço é a memoria de origem
                aux = getMemLoc(a2.contents.var.name,a2.contents.var.scope);
                if (aux == -1) { // caso a variável for global
                    aux = getMemLoc(a2.contents.var.name, "global");
                    instructionI(lwi, getReg(a1.contents.var.name), none, aux, NULL);
                }else 
                    instructionI(lwr, getReg(a1.contents.var.name), $fp, aux, NULL);
                break;

            case opVEC:
                //Se a varivel for do tipo vetor, realiza o deslocamento pegando sua posição na memória de dados na tabela de simbolos
                if(getVarKind(a2.contents.var.name,a2.contents.var.scope) == address){
                    instructionI(lwr,getReg(a1.contents.var.name),$fp,getMemLoc(a2.contents.var.name,a2.contents.var.scope),NULL);
                    instructionR(add,getReg(a3.contents.var.name),getReg(a3.contents.var.name),getReg(a1.contents.var.name));
                    instructionI(lwr,getReg(a1.contents.var.name),getReg(a3.contents.var.name),0,NULL);
                }else{
                    aux = getMemLoc(a2.contents.var.name,a2.contents.var.scope);
                    if (aux == -1) { // Caso o vetor seja global
                        aux = getMemLoc(a2.contents.var.name, "global");
                        instructionI(lwr,getReg(a1.contents.var.name),getReg(a3.contents.var.name),aux,NULL);    
                    }else{
                        instructionI(lwr,getReg(a1.contents.var.name),getReg(a3.contents.var.name),aux,NULL);
                    }
                }break;

            case opSTORE:
                //O segundo endereço é o registrador de origem e o terceiro endereço é a memoria de destino que o dado sera armazenado
                aux = getMemLoc(a2.contents.var.name, a2.contents.var.scope);
                //Aux recebe a posição de memória que a variavel será armazenada na memória de dados
                if (aux == -1){ // Caso a variavel seja global
                    aux = getMemLoc(a2.contents.var.name, "global");
                
                    if(a3.kind == Empty) //  Caso a variavel não seja um vetor global
                        instructionI(swi, getReg(a1.contents.var.name), none, aux, NULL);
                    else if(a3.kind == IntConst){ //Caso seja um vetor global tipo inteiro
                        aux += a3.contents.val;
                        instructionI(swi, getReg(a1.contents.var.name), none, aux, NULL);

                     }else instructionI(swr,getReg(a1.contents.var.name),getReg(a3.contents.var.name),aux,NULL);
                    
                }else if(a3.kind == Empty) // caso não seja um vetor local
                    instructionI(swr, getReg(a1.contents.var.name), $fp, aux, NULL);
                
                else if(a3.kind == IntConst){ // caso seja um vetor local
                    aux += a3.contents.val-1;
                    instructionI(swr, getReg(a1.contents.var.name), $fp, aux, NULL);
                
                } else{
                    instructionR(add,getReg(a3.contents.var.name),getReg(a3.contents.var.name),$fp);
                    instructionI(swr,getReg(a1.contents.var.name),getReg(a3.contents.var.name),aux,NULL);
                }

            break;
            case opGOTO:
                instructionJ(jump, -1, a1.contents.var.name,$zero);
                break;
            
            case opRET:
                if (a1.kind == String) {
                    instructionI(move, $ret, getReg(a1.contents.var.name), 0, NULL);
                }else {
                    instructionI(move, $ret, $zero, a1.contents.val, NULL);
                }
                //Realiza um jump para o endereço armazenado pelo Register Access
                 if(strcmp(a1.contents.var.scope,"main") != 0){
                    instructionJ(jr, 0, NULL,$ra);

                } else instructionJ(jump, -1, "end",$zero);


                break;
            
            case opFUN: //Terminando as declarações globais, ja na primeira função ele marca o jump da main
                if (jumpmain == 0) {
                    instructionJ(jump, -1, "main",$zero);
                    jumpmain = 1;
                }
                insertLabel(a1.contents.var.name);
                insertFun(a1.contents.var.name);
                param_count = 30;
                break;
            
            case opEND:
                if (strcmp(a1.contents.var.name, "main") == 0) {
                    instructionJ(jump, -1, "end",$zero);
                }
 
                break;
            
            case opPARAM: //Define o parametro a ser enviado pro call
                instructionI(move, getParamReg(), getReg(a1.contents.var.name), 0, NULL);
           
                break;
            
            case opCALL:
                if (strcmp(a2.contents.var.name, "input") == 0) {
                    insertInstruction(formatI,in,$zero,$ret,$zero,0,NULL);
                }
                else if (strcmp(a2.contents.var.name, "output") == 0) {
                    insertInstruction(formatI,out,$zero,$ret,$zero,0,NULL);
                }
                else { //Adiciona o endereço de quem chamou no registrador ra
                    aux = getFunSize(a2.contents.var.scope);
                    instructionI(addi,$ra,$fp,aux,NULL);
                    instructionJ(jal, -1, a2.contents.var.name,$zero);
                    instructionI(subi,$ra,$fp,aux,NULL);
                }
                narg = a3.contents.val;
                param_count = 30;
                break;
            
            case opARG:
                insertVar(a3.contents.var.scope, a1.contents.var.name, 1, checkType(l));
                FunList f = funlisthead;
                instructionI(swr, getParamReg(), $fp, getMemLoc(a1.contents.var.name,a1.contents.var.scope), NULL);
                break;
            
            case opLABEL:
                insertLabel(a1.contents.var.name);
                break;
            
            case opHALT:
                insertLabel("end");
                insertInstruction(formatI,halt,$zero,none,$zero,0,NULL);
                break;
    
            default:
                break;
        }
        l = l->next;
        
    }
}

void createInstructions (QuadList head) {
    
    QuadList l = head;
    generateInstruction(l);
    AssemblyCode a = codehead;

    while (a != NULL) {
        if (a->kind == instr) {
            switch(a->line.instruction.opcode){ // Atualiza o imediato dos Saltos Condicionais e Incondicionais com base na linha dos labels de desvios
            case jump:
                a->line.instruction.imed = getLabelLine(a->line.instruction.label)+1;
                break;
            case bne:
                a->line.instruction.imed = getLabelLine(a->line.instruction.label)+1;
                break;
            case beq:
                a->line.instruction.imed = getLabelLine(a->line.instruction.label)+1;
                break;
            case blt:
                a->line.instruction.imed = getLabelLine(a->line.instruction.label)+1;
                break;
            case bgt:
                a->line.instruction.imed = getLabelLine(a->line.instruction.label)+1;
                break;
            case jal:
                a->line.instruction.imed = getLabelLine(a->line.instruction.label)+1;
                break;
            default:
                break;
            }
        }
        a = a->next;
    }
}

void printAssembly () {

    AssemblyCode a = codehead;
    while (a != NULL) {

        if (a->kind == instr) {
            
            switch(a->line.instruction.format){
            case formatR:
                fprintf(assembly_code,"%s %s %s %s;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg1], regNames[a->line.instruction.reg2], regNames[a->line.instruction.reg3]);
            break;

            case formatI:

                 if(a->line.instruction.opcode == halt) {
                    fprintf(assembly_code,"%s;\n", InstrNames[a->line.instruction.opcode]);
                 }
                 else if (a->line.instruction.opcode == in || a->line.instruction.opcode == out) {
                    fprintf(assembly_code,"%s %s;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg2]);
                }
                else if(a->line.instruction.opcode == swi || a->line.instruction.opcode == lwi){
                    fprintf(assembly_code,"%s %s %d;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg1], a->line.instruction.imed);
                } 
                else if(a->line.instruction.opcode == put){
                    fprintf(assembly_code,"%s %s %d;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg1], a->line.instruction.imed);
                }
                else{
                    fprintf(assembly_code,"%s %s %s %d;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg1], regNames[a->line.instruction.reg2], a->line.instruction.imed);
                }
            break;

            case formatJ: {
                
                //Print do Jump Register, quando tiver retornando para o endereço que chamou
                if(a->line.instruction.opcode == jr) {
                    fprintf(assembly_code,"%s $ra;\n", InstrNames[a->line.instruction.opcode]);
                 }
                 //Print do Jump and Link e do Jump normal
                 else{ 
                    fprintf(assembly_code,"%s %d;\n", InstrNames[a->line.instruction.opcode], a->line.instruction.imed);
                       
                }
                
                
                break;
                
            }

            
            }
        }
        else{
            fprintf(assembly_code,"//%s\n", a->line.label);
        }
        a = a->next;
    }
}

/* Função inicial da geração de código assembly */
void generateAssembly (QuadList head) {
    line = 0;
    instructionI(addi,$fp,$zero,initVarLocalAddress,NULL);
    insertFun("global");
    createInstructions(head);
    printAssembly();
   }

AssemblyCode getAssembly() {
    return codehead;
}

int getSize() {
    return line - 1;
}