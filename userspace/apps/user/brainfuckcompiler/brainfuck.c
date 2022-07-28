#include <print.h>

void brainfuck(char *Data, char *Input)
{
    int BracketFlag;
    char Command;
    char CodeData[1001] = {0};
    char *TempBuffer;
    TempBuffer = &CodeData[500];

    while (Command = *Data++)
        switch (Command)
        {
        case '>':
        {
            TempBuffer++;
            break;
        }
        case '<':
        {
            TempBuffer--;
            break;
        }
        case '+':
        {
            (*TempBuffer)++;
            break;
        }
        case '-':
        {
            (*TempBuffer)--;
            break;
        }
        case '.':
        {
            printf("%c", *TempBuffer);
            break;
        }
        case ',':
        {
            *TempBuffer = *Input++;
            break;
        }
        case '[':
        {
            if (!*TempBuffer)
            {
                for (BracketFlag = 1; BracketFlag; Data++)
                    if (*Data == '[')
                        BracketFlag++;
                    else if (*Data == ']')
                        BracketFlag--;
            }
            break;
        }
        case ']':
        {
            if (*TempBuffer)
            {
                Data -= 2;
                for (BracketFlag = 1; BracketFlag; Data--)
                    if (*Data == ']')
                        BracketFlag++;
                    else if (*Data == '[')
                        BracketFlag--;
                Data++;
            }
            break;
        }
        }
    printf("\n");
}

int main(int argc, char *argv[])
{
    char *Data;
    char *Input;
    if (argc != 3)
    {
        printf("Usage: %s <Code> <Input>\n", argv[0]);
        return 1;
    }
    Data = argv[1];
    Input = argv[2];
    brainfuck(Data, Input);
    return 0;
}
