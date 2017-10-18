#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define POISON -666

struct stack_t
{
    int* data;
    int n;
    int max_size;
};

stack_t stack_construct(int size_of_stack);
void stack_destruct(stack_t* const object);
void push(stack_t* const object, int item);
int pop(stack_t* const object);
bool stack_OK(const stack_t* const object);
bool stack_full(const stack_t* const object);
bool stack_empty(const stack_t* const object);
void stack_dump(const stack_t* const object, bool OK);

stack_t stack_construct(int size_of_stack)
{
    stack_t object = {(int*)calloc(size_of_stack, sizeof(int)), 0, size_of_stack};
    return object;
};

void stack_destruct(stack_t* const object)
{
    assert(object);

    free(object -> data);
    object -> data = NULL;
    object -> max_size = POISON;
    object -> n = POISON;
};

void push(stack_t* const object, int item)
{
    assert(stack_OK(object));

    assert(object->n < object->max_size);
    object -> data[object -> n] = item;
    object -> n ++;

    assert(stack_OK(object));
};

int pop(stack_t* const object)
{
    assert(stack_OK(object));

    int item = object->data[object -> n];
    object -> n--;

    assert(stack_OK(object));
    return  item;
};

bool stack_OK(const stack_t* const object)
{
    bool OK = true;

    if(object)
    {
        if(object -> n < 0)                     OK = false;
        if(object -> max_size < 0)              OK = false;
        if(!object -> data)                     OK = false;
        if(object -> n > object -> max_size)    OK = false;
    } else                                      OK = false;

    if(!OK) stack_dump(object, OK);


    return OK;
};

bool stack_full(const stack_t* const object)
{
    assert(stack_OK(object));

    return (object->max_size == object->n);
};

bool stack_empty(const stack_t* const object)
{
    assert(stack_OK(object));

    return !(object->n);
};

void stack_dump(const stack_t* const object, bool OK)
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
                printf("data[%d] = %d", i, object -> data[i]);
                if(i < object -> n) printf("\n");
                else printf("   DUMP\n");
            }
        }
    }
    else printf("!!!WARNING!!!!\nSTACK POINTER DOES NOT EXIST");

    printf("\n\n");
};
