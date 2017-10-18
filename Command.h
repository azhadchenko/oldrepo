enum Command_t {STANDART, JUMP, PUSH};

class Command
{
    public:

    Command_t getType()        {return type_;}
    int  getLength()           {return length_;}
    unsigned char getCommand() {return command_;}

    void fillTranslation(char* pointer)
    {
        memcpy(pointer, translation_, length_);
    }

    Command(Command_t type, int length, unsigned char command, char* translation)
    {
        if(length < 0 || !translation);

        type_ = type;
        length_ = length;
        command_ = command;
        char* translation_ = new char[length_];
        memcpy(translation_, translation, length_);
        copy_ = false;
    }

    ~Command()	{if(!copy_) delete[] translation_;}

    Command* operator=(Command* item) {return item;}

    Command(const Command& item)
    {
        length_  = item.length_;
        type_    = item.type_;
        command_ = item.command_;
        translation_ = item.translation_;
        copy_ = true;
    }

    protected:

    Command_t type_;
    int length_;
    unsigned command_;
    char* translation_;
    bool copy_;
};

class Jmp : public Command
{
    public:

    void setJmpLength(size_t length)
    {
        *(size_t*)(translation_ + indent_) = length;
    }

    int getJmpLength() {return *(size_t*)(translation_ + indent_);}

    Jmp(Command_t type, int length, unsigned char command, char* translation, unsigned int indent)
        :Command(type, length, command, translation)
    {
        if(indent <= 0);

        indent_ = indent;
    }

    Jmp* operator=(Jmp* item) {return item;}

    ~Jmp() {if(!copy_) delete[] translation_;}

    void operator=(const Jmp&);

    Jmp(const Jmp& item) : Command(item)
    {
        indent_ = 0;
    }

    private:

    unsigned int indent_;
};

class Push : public Command
{
    public:
    void setNumber(double number)
    {
        *(double*)(translation_ + 1) = number;
    }

    Push(Command_t type, int length, unsigned char command, char* translation)
        :Command(type, length, command, translation) {}

    Push* operator=(Push* item) {return item;}

    ~Push() {delete[] translation_;}

    void operator=(const Push&);
    Push(const Push& item) : Command(item) {}
};
