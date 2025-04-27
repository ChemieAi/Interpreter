
//05180000027_Barýþ_Levent_Kara_05180000063_Burak_Kýzýlay_05170000123_Onur_Can_Baþ
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define IDSIZE 30

// Flag to hold the token if extra has been retrieved
// initially set to 0
int flag = 0;

//File pointers for read and write
FILE* readcode;     //File for writing our textjedi code in and reading it
FILE* symboltable;  //Symbol table to keep track of variables


// Defining the data types, keywords, and tokens of TextJedi
typedef enum {
    // Explicitly assigning enum ID
    TEXT = 1,
    INT = 2,
    NEW = 3,
    SEMICOLON = 4,
    COMMENT = 5,
    ASSIGNMENT = 6,
    PLUS = 7,
    MINUS = 8,
    IDENTIFIER = 9,
    CONSTANT = 10
} TokenType;

// Making ADT for a token of TextJedi
typedef struct {
    int value;
    TokenType type;
    char TokenName[IDSIZE];
    TokenType idtype;
} Token;


//Global return variable to be used by gettoken and ungettoken function
Token ret = { 0, TEXT, "ExampleToken" };




void openreadfiles()
{
    char filename[] = "myProg.tj";
    readcode = fopen(filename, "r");
    char filename1[] = "symboltable.txt";
    symboltable = fopen(filename1, "w");
}


// Lexical analyzer for TextJedi
// Produces individual Lexemes
Token Lexical() {
    Token token = { 0, TEXT, "ExampleToken" };

    char key = NULL;


        //Consume character from file
        key = fgetc(readcode);

        //Skip space and newline characters if the exist before a comment
        while (isspace(key) || key == '\n' && key != EOF)
        {
            key = fgetc(readcode);
        }

        if (key == '/')
        {
            key = fgetc(readcode);
            if (key == '*')
            {
                key = fgetc(readcode);
                while (key != '*')
                {
                    key = fgetc(readcode);
                }

                if (key == '*')
                {
                    key = fgetc(readcode);
                    if (key != '/')
                    {
                        printf("COMMENT ERROR");
                        exit(1);
                    }
                    else key = fgetc(readcode);
                }
            }
            else
            {
                printf("COMMENT ERROR");
                exit(1);
            }
        }

        //Skip spaces or newlines after a comment
        while (isspace(key) || key == '\n' && key != EOF)
        {
            key = fgetc(readcode);
        }



        if (key == '+')
        {
            token.value = -1;
            token.type = PLUS;
            strcpy(token.TokenName, "+");
            return token;
        }

        else if (key == '-')
        {
            token.value = -1;
            token.type = MINUS;
            strcpy(token.TokenName, "-");
            return token;
        }

        else if (key == ';')
        {
            token.value = -1;
            token.type = SEMICOLON;
            strcpy(token.TokenName, ";");
            return token;
        }

        else if (key == ':')
        {
            key = fgetc(readcode);
            if (key == '=')
            {
                token.value = -1;
                token.type = ASSIGNMENT;
                strcpy(token.TokenName, ":=");
                return token;
            }
            else
            {
                printf("Syntax Error");
                exit(1);
            }
        }

        else if (isdigit(key))
        {
            char val[100] = "";
            int i = 0;
            while (isdigit(key))
            {
                val[i] = key;
                key = fgetc(readcode);
                ++i;
            }
            ungetc(key, readcode);
            token.value = atoi(val);
            strcpy(token.TokenName, val);
            token.type = CONSTANT;
            //Error checking for int as int can only be a positive integer
            if (token.value < 0)
            {
                printf("\nInvalid integer value");
                exit(1);
            }

            return token;

        }

        else
        {
            char toCheck[100] = "";
            int i = 0;
            while (isalpha(key)||isdigit(key) && key!=EOF && key != ' ')
            {
                toCheck[i] = key;
                key = fgetc(readcode);
                ++i;
            }

            ungetc(key, readcode);

            if (strcmp(toCheck,"new")==0)
            {
                token.value = -1;
                token.type = NEW;
                strcpy(token.TokenName, "new");
                return token;
            }

            else if (strcmp(toCheck, "text") == 0)
            {
                token.value = -1;
                token.type = TEXT;
                strcpy(token.TokenName, "text");
                return token;
            }

            else if (strcmp(toCheck, "int") == 0)
            {
                token.value = -1;
                token.type = INT;
                strcpy(token.TokenName, "int");
                return token;
            }

            else
            {
                token.value = -1;
                token.type = IDENTIFIER;
                strcpy(token.TokenName, toCheck);
                fprintf(symboltable,toCheck);
                fprintf(symboltable, "\n");
                return token;
            }

        }
}


// Helper function to get tokens from lexical
Token gettoken() {
    if (!flag) {
        ret = Lexical();
        return ret;
    }
    else {
        // Reset the flag
        flag = 0;
        return ret;
    }
}

// Raise flag to avoid getting a new token
void ungettoken()
{
    flag = 1;
}



int expr2() {
    Token t;
    t = gettoken();
    if (t.type == CONSTANT || t.type == IDENTIFIER) {
        return 1;
    }
    else return 0;
}


int expr1() {
    Token t;
    do
    {
        if (!expr2()) {
            return 0;
        }

        t = gettoken();
    } while (t.type == PLUS || t.type == MINUS);
    ungettoken();
    return 1;
}


//Function to check if variable has been declared before
int declarevar()
{
    Token var;
    var = gettoken();
    if (var.type == NEW)
    {
        var = gettoken();
        if (var.type == INT || var.type == TEXT)
        {
            //Temporary token to store the type of id in it's structure
            TokenType assigntypetovar=var.type;
            var = gettoken();
            if (var.type == IDENTIFIER)
            {
                var.idtype = assigntypetovar;
                var = gettoken();
                if (var.type == SEMICOLON)
                {
                    return 1;
                }
                else return 0;
            }
            else return 0;
        }
        else return 0;
    }
    else return 0;

}


int AssignVariable()
{
    Token assign;
    assign = gettoken();

    if (assign.type == IDENTIFIER)
    {
        assign = gettoken();
        if (assign.type == ASSIGNMENT)
        {
                if (expr1())
                {
                    assign = gettoken();
                    if (assign.type == SEMICOLON)
                    {
                        return 1;
                    }
                    else return 0;
                }
                else return 0;
        }
        else return 0;
    }
    else return 0;
}

int size(char* myText) {
    return strlen(myText);
}

char* subs(char* myText, int begin, int end) {
    int length = end - begin + 1;
    char* substring = malloc(length + 1);
    strncpy(substring, myText + begin, length);
    substring[length] = '\0';
    return substring;
}

int locate(char* bigText, char* smallText, int start) {
    char* position = strstr(bigText + start, smallText);
    if (position != NULL) {
        return position - bigText + 1;
    }
    return 0;
}

char* insert(char* myText, int location, char* insertText) {
    int textSize = strlen(myText);
    int insertSize = strlen(insertText);
    int newSize = textSize + insertSize;
    char* newText = malloc(newSize + 1);

    strncpy(newText, myText, location);
    strncpy(newText + location, insertText, insertSize);
    strncpy(newText + location + insertSize, myText + location, textSize - location);
    newText[newSize] = '\0';

    return newText;
}

void override(char* myText, int location, char* ovrText) {
    int textSize = strlen(myText);
    int ovrSize = strlen(ovrText);

    for (int i = 0; i < ovrSize; i++) {
        if (i + location >= textSize) {
            break;
        }
        myText[i + location] = ovrText[i];
    }
}

char* asString(int myInt) {
    char* myString = malloc(12);
    sprintf(myString, "%d", myInt);
    return myString;
}

int asInt(char* myString) {
    return atoi(myString);
}

int main() {

    //open all the read files for textjedi
    openreadfiles();

    if (declarevar())
    {
        if (AssignVariable())
        {
            printf("Syntax correct");
        }
        else printf("Incorrect Syntax");
    }
    else printf("Syntax incorrect");
    return 0;
}
