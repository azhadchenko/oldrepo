#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "H:\MIPT_CS_PROJECTS\CPU\COMM_ENUM.h"

struct mark
{
    int number;
    int position;
    char* value;
    mark* next;
};

mark* get_mark_by_value (mark* item, char* value);      //! Search for mark with "value". IF not exist, creates one and return
int get_pos_by_number  (mark* item, int number);        //! Returns position of mark number "number"
void free_marks(mark* item);                            //! Free mark starting from element

bool read_line(char* buff, int sz, FILE* source);         //! Read line from source(stdin, files, etc...). If success, returns true. Buffer is filled anyway
void read_name_of_file(char* buff, int sz);               //! Specialises read_line() function to read name of file
bool read_command(char* command, int sz, FILE* source);   //! Specialises read_line() function to read command

FILE* try_open(char* name);                         //! Tries to open file. If not - terminating programm

int translate_commands(FILE* source, double* comm_arr, mark* mark_head, int array_size);   //! Translating commands from text(from source) to list. Returns number of commands
bool is_command(char** command, char* type_);                             //! Checking if command is push. If succsess, placing push argument (ax, bx, or constant) in command
void j_process(char** command);                                           //! Searching for jump argument in command, then writing it to command

void fill_jmps(double* commands, mark* mark_head, int n);   //! Fills jumps arguments with position
void change_format(char* name);                             //! Changes format to write a file. For example, test became test.cpu, test.txt became test.cpu
void write_to_file(char* name, double* commands, int n);    //! Writes array to binary file. Specifies with number of commands. WARNING - MAY OVERWRITE A FILE

void set_jumps(double* commands, int n);    //! Specifies jumps for byte code
int get_comm_length(int k);                 //! Returns size of command in bytes
int count(double* commands, int n);         //! Return full size of programm in bytes

void run_asm(char* name_of_file);               //! Compiling majority of previous functions in programm. Clearing memory.



int main()
{
    const int SYMBOLS_EXP = 257;
    char buffer[SYMBOLS_EXP] = {0};

    read_name_of_file(buffer, SYMBOLS_EXP);

    run_asm(buffer);

    return 0;
}

void read_name_of_file(char* buff, int sz)
{
    assert(buff);

    printf("Please, select file to assemble. Max 256 symbols. \nNon txt format may cause errors \n");

    while(read_line(buff,sz,stdin)==false)
        printf("Incorrect way. Please insert another: \n");
}

bool read_line(char* buff, int sz, FILE* source)
{
    assert(buff);
    assert(source);


    fgets(buff, sz, source);

    int size = strlen(buff);


    if(buff[size-1]=='\n')
    {
        buff[size-1]='\0';
        return true;
    }
    else
    {
        fscanf(source, "%*[^\n]");
        fscanf(source, "%*c");            //Renew input stream

        return false;
    }
}


void run_asm(char* name_of_file)
{
    assert(name_of_file);

    FILE* source = try_open(name_of_file);

    int num_of_comm = 0;

    mark* mark_head = (mark*)calloc(1,sizeof(mark));
    assert(mark_head);

    int array_size = 128;
    double* comm_arr = (double*)calloc(array_size, sizeof(double));
    assert(comm_arr);

    num_of_comm = translate_commands(source, comm_arr, mark_head, array_size);
    fclose(source);

    fill_jmps(comm_arr, mark_head, num_of_comm);

    write_to_file(name_of_file, comm_arr, num_of_comm);

    free(comm_arr);
    free(name_of_file);
}

FILE* try_open(char* name)
{
    assert(name);

    FILE* object = fopen(name, "r");

    if(object > 0)
        return object;
    else
    {
        printf("\nFile has not been found. Finishing programm\n");
        exit(0);
    }
}


int translate_commands(FILE* source, double* arr_comm, mark* mark_head, int array_size)
{
    assert(source);
    assert(mark_head);

    const int MAX_DIGITS = 65;
    char* curr_comm = (char*)calloc(MAX_DIGITS,sizeof(char));
    assert(curr_comm);
    char* clone = curr_comm;

    double tmp=0;
    int n=0;

    for(;read_command(curr_comm,MAX_DIGITS, source)!=0;)
    {
        if(n>=array_size-2)
        {
            array_size*=2;
            arr_comm = (double*)realloc(arr_comm, array_size*sizeof(double));
        }

        strupr(curr_comm);
        #define SET_COMMAND_NO_ARG(NAME_, void_)  if(strcmp(#NAME_, curr_comm)==0)\
                                                                                                 arr_comm[n++] = CPU_##NAME_;\
                                                                                           else
       
       #define SET_JUMPS(NAME__, void_)                        if(strncmp(#NAME__,curr_comm,strlen(#NAME__))==0) \
                                                                                          {\
                                                                                                   arr_comm[n++] = CPU_##NAME__;\
                                                                                                   j_process(&curr_comm);\
                                                                                                   arr_comm[n++] = get_mark_by_value(mark_head, curr_comm)->number;\
                                                                                                   }\
                                                                                          else
                                                                                          
        #include "H:\MIPT_CS_PROJECTS\CPU\commands.h"
        if(curr_comm[0]==':')
        {

            curr_comm++;
            mark* curr_mark = get_mark_by_value(mark_head, curr_comm);
            curr_mark->position=n;
        }
        else
        if(is_command(&curr_comm, "PUSH"))
        {
            #define SET_COMMAND_PUSH(item_, void_) if(strcmp(#item_,curr_comm)==0)  {arr_comm[n++] = CPU_PUSH_##item_;} else
            #include "H:\MIPT_CS_PROJECTS\CPU\push_c.h"
            if((curr_comm[0]>='0'&&curr_comm[0]<='9')||curr_comm[0]=='-')
                {
                    tmp = atof(curr_comm);

                    arr_comm[n++] = CPU_PUSH;
                    arr_comm[n++] = tmp;
                }
            else
            {
                printf("Error in translating file. Unknown command: push %s \n", curr_comm);
                exit(0);
            }
        }
        else
        if(is_command(&curr_comm, "POP"))
        {
            #define SET_COMMAND_POP(item_, void_) if(strcmp(#item_,curr_comm)==0)   {arr_comm[n++] = CPU_POP_##item_;}  else
            #include "H:\MIPT_CS_PROJECTS\CPU\pop_c.h"
            {
                printf("Error in translating file. Unknown command: pop %s \n", curr_comm);
                exit(0);
            }
        }

        else
        {
            printf("Error in translating file. Unknown command: %s \n", curr_comm);
            exit(0);
        }

        curr_comm=clone;
        *curr_comm=0;
    }


    free(curr_comm);

    return n;
}

bool read_command(char* command, int sz, FILE* source)
{
    assert(command);
    assert(source);

    if(read_line(command, sz, source))
        return true;

    else
    {
        if(command[0]!='\0')
            return true;

        if(fscanf(source, " %*")!=EOF)
        {
            printf("Error while reading. Too long command(which is not even exits) or input file damaged.");
            exit(0);
        }

        return false;
    }
}


void j_process(char** command)
{
    assert(command);
    assert(*command);

    char* fast_acs = *command;
    for(;fast_acs;)
        if(*fast_acs++==':')
            break;

    *command = fast_acs;
}


bool is_command(char** command, char* type_)
{
    assert(command);
    assert(*command);
    assert(type_);

    char* fast_acs = *command;

    if(strncmp(fast_acs,type_,strlen(type_))==0)
    {
        for(;fast_acs;)
            if(*fast_acs++==' ')
                break;

        *command=fast_acs;
        return true;
    }

    return false;
}

void fill_jmps(double* commands, mark* mark_head, int n)
{
    assert(commands);
    assert(mark_head);

    for(int i = 0; i < n; i++)
    {
        if(commands[i]==CPU_PUSH)
            i++;
        else
        {
            if(commands[i]>START_JUMPS&&commands[i]<END_JUMPS)
            {
                i++;
                commands[i]=get_pos_by_number(mark_head, (int)commands[i]);
            }
        }
    }
}


void write_to_file(char* name, double* commands, int n)
{
    assert(name);
    assert(commands);

    change_format(name);
    FILE* output = fopen(name, "wb");

    int size = count(commands, n);
    set_jumps(commands, n);

    fwrite(&size, sizeof(int),1,output);

    char tmp = 0;
    for(int i = 0; i < n; i++)
    {
        tmp = (char)commands[i];
        fwrite(&tmp, sizeof(char), 1, output);

        if((int)commands[i]>START_JUMPS&&(int)commands[i]<END_JUMPS)
        {
            int tmp1 = (int)commands[++i];
            fwrite(&tmp1, sizeof(int), 1, output);
        }
        else
            if((int)commands[i]==CPU_PUSH)
            {
                double tmp1 = commands[++i];
                fwrite(&tmp1, sizeof(double), 1, output);
            }
    }

    printf("Done! Name of created file: %s", name);
    fclose(output);
}

int count(double* commands, int n)
{
    assert(commands);

    int result = 0;
    int tmp =0;

    for(int i = 0; i < n; i++)
    {
        tmp=get_comm_length((int)commands[i]);
        if(tmp>sizeof(char))
            i++;

        result+=tmp;
    }

    return result;
}

int get_comm_length(int k)
{
    int result = 0;

    if(k>START_JUMPS&&k<END_JUMPS)
        result+=sizeof(char)+sizeof(int);
    else
        if(k==CPU_PUSH)
            result+=sizeof(char)+sizeof(double);
        else
            result+=sizeof(char);

    return result;
}

void set_jumps(double* commands, int n)
{
    assert(commands);

    int mem_pos = 0;
    int tmp=0;
    for(int i = 0; i < n; i++)
    {
        if((int)commands[i]>START_JUMPS&&(int)commands[i]<END_JUMPS)
        {
            i++;
            for(int j = 0; j < commands[i]; j++)
            {
                tmp=get_comm_length((int)commands[j]);
                if(tmp>sizeof(char))
                    j++;

                mem_pos+=tmp;
            }
            commands[i]=mem_pos;
            mem_pos=0;
        }
    }
}

void change_format(char* name)
{
    assert(name);

    int k = 0;
    for(;name[++k]!='.'&&name[k];);

    name[k] = '\0';

    strcat(name, ".cpu");
}

mark* get_mark_by_value(mark* item, char* value)
{
    assert(item);
    assert(value);

    int n = 0;
    bool exist = false;

    while(item->value)
    {
        if(strcmp(item->value, value)==0)
        {
            exist = true;
            break;
        }
        n++;
        item=item->next;
    }

    if(!exist)
    {
        item->number=n;
        item->position=-1;
        item->value=(char*)calloc(strlen(value), sizeof(char));
        strcat(item->value,value);

        item->next = (mark*)calloc(1,sizeof(mark));
    }

    return item;
}

int get_pos_by_number (mark* item, int number)
{
    assert(item);

    for(;item->value;)
    {
        if(item->number==number)
            break;

        item = item->next;
    }

    return item->position;
}

void free_marks(mark* item)
{
    assert(item);
    mark* tmp = 0;

    for(;item->next;)
    {
        tmp = item->next;
        free(item->value);
        free(item);
        item = tmp;
    }

    free(item->value);
    free(item);
}
