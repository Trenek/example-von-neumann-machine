#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "pmcDef.h"

const char *skipWhiteSpaces(const char *line) {
    if (line != NULL) while (*line == ' ' || *line == '\n') line += 1;
    
    return line;
}

const char *skipNums(const char *line) {
    if (line != NULL) while (*line >= '0' && *line <= '9') line += 1;
    
    return line;
}

bool isWhiteSpace(const char a) {
    return a == 0 || a == ' ' || a == '\t' || a == '\n' || a == '\r';
}

bool compStr(const char *a, const char *b) {
    int blockLengthA = strchr(a, ' ') == NULL ? strlen(a) : strchr(a, ' ') - a;
    int blockLengthB = strchr(b, ' ') == NULL ? strlen(b) : strchr(b, ' ') - b;

    return blockLengthA == blockLengthB && 0 == strncmp(a, b, blockLengthB);
}

int isCommand(const char *line) {
    int i = 15;
    
    while (i >= 0 && false == compStr(line, codeName[i])) {
        i -= 1;
    }
    
    return i;
}

int isMode(const char *line) {
    int i = 3;
    
    while (i >= 0 && false == compStr(line, modeName[i])) {
        i -= 1;
    }
    
    return i;
}

bool isNum(char a) {
    return a >= '0' && a <= '9';
}

const char *checkFor(const char *a, const char *b) {
    int blockLengthB = 0;
    
    a = skipWhiteSpaces(a);
    b = skipWhiteSpaces(b);
    
    while (a != NULL && *b != 0 && a != 0) {
        blockLengthB = strchr(b, ' ') == NULL ? strlen(b) : strchr(b, ' ') - b;
        
        if (0 == strncmp(b, "%c", 2) && -1 != isCommand(a)) {
            a += strlen(codeName[isCommand(a)]);
            b += 2;
        }
        else if (0 == strncmp(b, "%m", 2) && -1 != isMode(a)) {
            a += 1;
            b += 2;
        }
        else if (0 == strncmp(b, "%d", 2) && (isNum(*a) || ((*a == '-' || *a == '+') && isNum(a[1])))) {
            a = skipNums(a + 1);
            b += 2;
        }
        else if (0 == strncmp(a, b, blockLengthB)) {
            a += blockLengthB;
            b += blockLengthB;
        }
        else {
            a = NULL;
        }
        
        a = skipWhiteSpaces(a);
        b = skipWhiteSpaces(b);
    }
    
    if (a != NULL && *a == 0 && *b != 0) {
        a = NULL;
    }
    
    return a;
}

bool validateLine(const char *line) {
    const char *tempLine;
    bool good = false;
    int temp = 0;
    
    line = skipWhiteSpaces(line);
    
    if (0 == *line) good = true;
    else if (NULL != (line = checkFor(line, "MEM [ %d ] :"))) {
        if (*line == 0) good = true;
        else if (NULL != (tempLine = checkFor(line, "%d"))) {
            if (*tempLine == 0) good = true;
        }
        else if (NULL != (tempLine = checkFor(line, codeName[STOP]))) {
            if (*tempLine == 0) good = true;
        }
        else if (NULL != (tempLine = checkFor(line, "%c %m %d"))) {
            if (*tempLine == 0) good = true;
        }
    }
    
    return good;
}

char *removeComments(char *line) {
    char *comments = strchr(line, ';');
    
    if (comments != NULL) {
        *comments = 0;
    }
    
    return line;
}

static int getMemPtr(const char *line) {
    int result = -1;
    
    line = strchr(line, '[');
    if (line != NULL) sscanf(line + 1, "%d", &result);
    
    return result;
}

static int getCode(const char *line) {
    int result = 15;
    
    while (result >= 0 && NULL == checkFor(line, codeName[result])) result -= 1;
    
    return result;
}

static int getMode(const char *line) {
    int result = 3;
    
    while (result >= 0 && NULL == checkFor(line, modeName[result])) result -= 1;
    
    return result;
}

bool loadPMC(const char * fileName, union memCell MEM[512], bool isCode[512], uint16_t *PC, uint16_t *maxMemOut) {
    bool isOk = true;
    
    FILE *file = fopen(fileName, "r");
    char buffer[512];
    char temp[512];
    char *line;
    int maxMem = 0;
    int tempInt = 0;
    bool f = true;
    
    printf("\e[0;31m");
    while (EOF != fscanf(file, "%[^\n]", buffer)) {
        fscanf(file, "%[\n]", temp);
        
        removeComments(buffer);
        if (validateLine(buffer)) {
            int memAddress = getMemPtr(buffer);
            if (memAddress != -1) {
                maxMem = maxMem > memAddress ? maxMem : memAddress;
                MEM[memAddress].val = 0;
                line = strchr(buffer, ':');
                *line = ' ';
                if (-1 != (tempInt = getCode(line))) {
                    f = false;
                    isCode[memAddress] = true;
                    MEM[memAddress].code = tempInt;
                    line = strstr(line, codeName[MEM[memAddress].code]) + strlen(codeName[MEM[memAddress].code]);

                    if (-1 != (tempInt = getMode(line))) {
                        MEM[memAddress].mode = tempInt;
                        line = strstr(line, modeName[MEM[memAddress].mode]) + strlen(modeName[MEM[memAddress].mode]);
                    }
                }
                else {
                    isCode[memAddress] = false;
                }
                if (f) *PC += 1;
                if (NULL != checkFor(line, "%d")) {
                    sscanf(line, "%hd", &tempInt);
                    MEM[memAddress].arg = tempInt;
                }
            }
        }
        else {
            printf("Syntax Error '%s'\n", buffer);
            isOk = false;
        }
    }
    printf("\e[0;37m");
    
    fclose(file);
    
    *maxMemOut = maxMem;
    return isOk;
}