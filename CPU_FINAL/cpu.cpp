#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "stack_struct.h"
#include "pointer_stack.h"

struct CPU_t
{
    unsigned char* commands;
    double ax;
    double bx;
    double cx;
    double dx;
    double bp;
    double sp;
    unsigned char* si;
    stack_t stack;
    p_stack_t pointers;
};

CPU_t CPU_init(int stack_size);
void CPU_load(CPU_t* object, char* source);
void read_filename(char* target, int SYMBOLS_EXP);
FILE* try_open(char* source);
void CPU_Run(CPU_t* object);
double read_next(FILE* input);

int main()
{
    const int stack_size = 100;
    CPU_t i_386 = CPU_init(stack_size);

    const int SYMBOLS_EXP = 257;
    char buffer[SYMBOLS_EXP] = {0};

    read_filename(buffer, SYMBOLS_EXP);

    CPU_load(&i_386, buffer);

    CPU_Run(&i_386);

    stack_destruct(&i_386.stack);
    free(i_386.commands);


    return 0;
}

CPU_t CPU_init(int stack_size)
{
    CPU_t tmp = {0,0,0,0,0,0,0,0,stack_construct(stack_size),stack_construct_p(stack_size)};
    return tmp;
}

void read_filename(char* target, int SYMBOLS_EXP)
{
    assert(target);

    printf("Please, select file to run. Non .cpu format not supported. 256 symbols max \n");

    for(;;)
    {
        fgets(target, SYMBOLS_EXP, stdin);
        int size = strlen(target);

        if(target[size-1]=='\n')
        {
            target[size-1]='\0';
            break;
        }
        else
        {
            scanf("%*[^\n]");
            scanf("%*c");
            printf("Too long way. Select another");
        }
    }
}

FILE* try_open(char* source)
{
    assert(source);

    FILE* input = fopen(source, "rb");

    if(input)
        return input;
    else
    {
        printf("File does not exist. Programm finishing...");
        exit(0);
    }
}

void CPU_load(CPU_t* object, char* source)
{
    assert(object);
    assert(source);

    FILE* input = try_open(source);

    int n = 0;

    fread(&n,sizeof(int),1,input);
    assert(n>0);

    object->commands = (unsigned char*)calloc(n,sizeof(char));
    assert(object->commands);

    fread(object->commands, sizeof(char), n, input);
    object->si=object->commands;

    fclose(input);
}


void CPU_Run(CPU_t* object)
{
    assert(object);
    assert(object->commands);

    double tmp1 = 0;
    double tmp2 = 0;
    bool check = true;
    unsigned char* comm_ptr = object->commands;

    for(;check;)
    {
        switch (*(comm_ptr))
        {

            #include "H:\MIPT_CS_PROJECTS\CPU\COMM_ENUM.h"
            #define SET_COMMAND_NO_ARG(name, code)  case CPU_##name:code break;
            #define SET_JUMPS(name, code)           case CPU_##name:code break;
            #define SET_COMMAND_PUSH(name, code)    case CPU_PUSH_##name:code break;
            #define SET_COMMAND_POP(name, code)     case CPU_POP_##name:code break;
            #include "H:\MIPT_CS_PROJECTS\CPU\commands.h"
            #include "H:\MIPT_CS_PROJECTS\CPU\pop_c.h"
            #include "H:\MIPT_CS_PROJECTS\CPU\push_c.h"
            case CPU_PUSH    :  comm_ptr++;
                                    push(&object->stack, *((double*)comm_ptr)); comm_ptr+=sizeof(double);
                                    break;
            default          :  printf("Unknown command : %d", *comm_ptr);
                                    exit(0);
        }
    }

}

