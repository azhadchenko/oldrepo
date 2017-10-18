#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define POISON -666

struct p_stack_t
{
    unsigned char** data;
    int n;
    int max_size;
};

p_stack_t stack_construct_p(int size_of_stack);
void stack_destruct_p(p_stack_t* const object);
void push_p(p_stack_t* const object, unsigned char* item);
unsigned char* pop_p(p_stack_t* const object);
bool stack_p_OK(const p_stack_t* const object);
bool stack_p_full(const p_stack_t* const object);
bool stack_p_empty(const p_stack_t* const object);
void stack_p_dump(const p_stack_t* const object, bool OK);

p_stack_t stack_construct_p(int size_of_stack)
{
    p_stack_t object = {(unsigned char**)calloc(size_of_stack, sizeof(unsigned char*)), 0, size_of_stack};
    return object;
};

void stack_destruct_p(p_stack_t* const object)
{
    assert(object);

    free(object -> data);
    object -> data = NULL;
    object -> max_size = POISON;
    object -> n = POISON;
};

void push_p(p_stack_t* const object, unsigned char* item)
{
    assert(stack_p_OK(object));

    assert(object->n < object->max_size);
    object -> data[object -> n] = item;
    object -> n++;

    assert(stack_p_OK(object));
};

unsigned char* pop_p(p_stack_t* const object)
{
    assert(stack_p_OK(object));

    unsigned char* item = object->data[--object -> n];

    assert(stack_p_OK(object));
    return  item;
};

bool stack_p_OK(const p_stack_t* const object)
{
    bool OK = true;

    if(object)
    {
        if(object -> n < 0)                     OK = false;
        if(object -> max_size < 0)              OK = false;
        if(!object -> data)                     OK = false;
        if(object -> n > object -> max_size)    OK = false;
    } else                                      OK = false;

    if(!OK) stack_p_dump(object, OK);


    return OK;
};

bool stack_p_full(const p_stack_t* const object)
{
    assert(stack_p_OK(object));

    return (object->max_size == object->n);
};

bool stack_p_empty(const p_stack_t* const object)
{
    assert(stack_p_OK(object));

    return !(object->n);
};

void stack_p_dump(const p_stack_t* const object, bool OK)
{
    if(object)
    {
        printf("STACK");
        printf("[%08x] \n", object);

        if(OK)                                  printf("Stack is OK \n");
        else                                    printf("!!!WARNING!!! STACK IS CORRUPTED \n");

        printf("Max size of stack = %d \n", object -> max_size);
        printf("Current size = %d \n\n", object -> n);

        if(object -> data == 0) printf("DATA DESTROYED\n");
        else
        {
            for(int i = 0; i < object -> max_size; i++)
            {
                printf("data[%d] = %lf", i, object -> data[i]);
                if(i < object -> n) printf("\n");
                else printf("   DUMP\n");
            }
        }
    }
    else printf("!!!WARNING!!!!\nSTACK POINTER DOES NOT EXIST");

    printf("\n\n");
};
