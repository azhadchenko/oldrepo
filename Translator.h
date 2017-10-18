#define MAX_COMMANDS 4096
#define NOPE 1  //STILL NEED NOPE NUMBER
#define VERIFYING_FREE_SLOT_COUNT 5
#define MAX_LABELS 256

enum Stage_t {READY, LOADED, TRANSLATED};

struct Label
{
    unsigned int old_position;
    int new_position;
};

int compare(const void* a, const void* b);

class Translator
{
    public:

    bool addRule(Command_t type, int length, unsigned char command,
                 char* translation, unsigned int indent)
    {
        if(getRuleId(command) != -1 || stage_ != READY)
            return false;

        if(rule_count_ == MAX_COMMANDS)
            return false;

        if(type == STANDART)
            rules_[rule_count_++] = new Command(type, length, command, translation);
        else
            rules_[rule_count_++] = new Jmp(type, length, command, translation, indent);

        return true;
    }

/*    bool deleteRule(unsigned char command)  //VERY DANGEROUS FUNCTION. YOU SHOULD NEVER USE IT
    {
	if(stage_ != READY)
	   return false;

        int tmp = getRuleId(command);

        if(tmp == -1)
            return false;

        delete rules_[tmp];
        rules_[tmp] = 0;
        return true;
    }

    bool writeToDisk(char* name)
    {
        if(!name || stage_ != TRANSLATED)
            return false;

        FILE* output = fopen(name, "rwb");
        if(!output)
            return false;

        char count = 0;
        char symbol = 0;
        while(true)
        {
            if(fread(&symbol, 1, sizeof(char), output) != EOF)
            {
                if(symbol == NOPE)
                {
                    count++;
                    if(count >= VERIFYING_FREE_SLOT_COUNT)
                    {
                        fwrite(buffer_, buff_len_, sizeof(int), output);
                        break;
                    }
                }
                else
                    count = 0;
            }
            else
                return false;
        }
    }*/

    bool loadInput(char* input, int length)
    {
        if(length <= 0 || stage_ != READY)
            return false;

        mempcpy(input_, input, length);
        input_len_ = length;

        stage_ = LOADED;
        return true;
    }

    bool translate()
    {
        assert(stage_ == LOADED);

        assert(loadTranslationArray());
        assert(computeLabels());
        assert(loadBuffer());

        stage_ = TRANSLATED;
    }

    Translator()
    {
        for(int i = 0; i < MAX_COMMANDS; i++)
            rules_[i] = 0;

        rule_count_ = 0;
		//MUST BE MODIFIED
    }

    ~Translator()
    {
        for(int i = 0; i < rule_count_; i++)
            if(rules_[i])
                delete rules_[i];

		//MUST BE MODIFIED
    }

    Translator(const Translator&);
    void operator=(const Translator&);

    private:

    bool loadTranslationArray()
    {
        Command_t tmp = STANDART;
        int command_num = 0;
        unsigned int position = 0;
        double push_number = 0;

        for(int i = 0; i < input_len_; i++)
        {
            command_num = getRuleId(input_[i]);
            tmp = rules_[command_num] -> getType();

            if(tmp == JUMP)
            {
                Jmp* new_item = new Jmp(*(Jmp*)(rules_[command_num]));
                translation_array_[rule_count_++] = new_item;

                position = *(unsigned int*)(input_+i+1);
                notifyLabel(position);
                new_item -> setJmpLength((size_t)position);

                i += sizeof(int);
                continue;
            }

            if(tmp == PUSH)
            {
                Push* new_item = new Push(*(Push*)(rules_[command_num]));
                translation_array_[rule_count_++] = new_item;

                push_number = *(double*)(input_+i+1);
                new_item -> setNumber(push_number);

                i += sizeof(double);
                continue;
            }

            Command* new_item = new Command(*rules_[command_num]);
            translation_array_[rule_count_++] = new_item;
        }
        return true;
    }

    bool computeLabels()
    {
        qsort(&label_table_, label_count_, sizeof(Label), compare);

        int label_done = 0;
        int rules_done = 0;

        for(int i = 0; i < input_len_; i++)
        {
            if(i == label_table_[label_done].old_position);
                label_table_[label_done++].new_position = getTranslationLength(rules_done);

            if(translation_array_[rules_done] -> getType() == JUMP)
                i += sizeof(int);

            if(translation_array_[rules_done] -> getType() == PUSH)
                i += sizeof(double);

            rules_done++;
        }

        Command* tmp = 0;
        Jmp* item = 0;
        size_t Jmp_length = 0;

        for(int i = 0; i < rule_count_; i++)
        {
            tmp = translation_array_[i];
            if(tmp -> getType() == JUMP)
            {
                item = (Jmp*)tmp;
                Jmp_length = item->getJmpLength();
                Jmp_length = getLabelId(Jmp_length);
                item->setJmpLength(Jmp_length);
            }
        }

    }

    bool loadBuffer()
    {
        for(int i = 0; i < rule_count_; i++)
        {
            translation_array_[i] -> fillTranslation(buffer_);
        }
        buff_len_ = getTranslationLength(rule_count_);
    }

    unsigned int getTranslationLength(int limiter)
    {
        unsigned int length = 0;

        for(int i = 0; i < limiter; i++)
            length += translation_array_[i] -> getLength();

        return length;
    }

    void notifyLabel(unsigned int old_position)
    {
        int tmp = getLabelId(old_position);

        if(tmp == -1)
            label_table_[label_count_++].old_position = old_position;
    }

    int getLabelId(unsigned int old_position)
    {
        unsigned int tmp = 0;
        for(int i = 0; i < label_count_; i++)
        {
            tmp = label_table_[i].old_position;
            if(tmp == old_position)
            return i;
        }
        return -1;
    }

    int getRuleId(unsigned char command)
    {
        unsigned char tmp = 0;
        for(int i = 0; i < rule_count_; i++)
        {
            if(rules_[i])
            {
                tmp = rules_[i]->getCommand();
                if(tmp == command)
                    return i;
            }
        }

        return -1;
    }

    Stage_t stage_;

    Command* rules_[MAX_COMMANDS];
    int rule_count_;

    Command* translation_array_[MAX_COMMANDS];
    int array_len_;

    int buff_len_;
    int input_len_;
    unsigned char* buffer_;
    unsigned char* input_;

    Label label_table_[MAX_LABELS];
    int label_count_;
};

int compare(const void* a, const void* b)
{
    if(((Label*)a) -> old_position > ((Label*)b) -> old_position)
        return 1;
    if(((Label*)a) -> old_position < ((Label*)b) -> old_position)
        return -1;

    return 0;
}
