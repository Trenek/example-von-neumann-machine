#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "pmcDef.h"

struct setup {
    const char *fileName;
    int base;
    bool uns;
    bool debugMode;
    bool stop;
    bool clean;
};

struct setup setupInit(int argc, char **argv, struct setup *setup) {
    int i = 1;
    
    while (i < argc) {
        if (0 == strcmp(argv[i], "-f")) {
            setup->fileName = argv[i + 1];
            
            i += 1;
        }
        else if (0 == strcmp(argv[i], "-base")) {
            setup->base = atoi(argv[i + 1]);
        }
        else if (0 == strcmp(argv[i], "-signed")) {
            setup->uns = false;
        }
        else if (0 == strcmp(argv[i], "-unsigned")) {
            setup->uns = true;
        }
        else if (0 == strcmp(argv[i], "-debugMode")) {
            setup->debugMode = true;
            if (i + 1 < argc) {
                if (0 == strcmp(argv[i + 1], "cs")) {
                    setup->stop = true;
                    setup->clean = true;
                    i += 1;
                }
                else if (0 == strcmp(argv[i + 1], "c")) {
                    setup->clean = true;
                    i += 1;
                }
                else if (0 == strcmp(argv[i + 1], "s")) {
                    setup->stop = true;
                    i += 1;
                }
            }            
        }
        
        i += 1;
    }
}

int main(int argc, char **argv) {
    union memCell MEM[512];
    int16_t AC;
    int16_t OR;
    uint16_t PC = 0;
    union memCell IR = { 0 };
    
    struct setup setup = {
        .fileName = "main.vnm",
        .base = 10,
        .uns = false,
        .debugMode = false,
        .stop = false,
        .clean = false
    };
    setupInit(argc, argv, &setup);
    
    bool isCode[512];
    uint16_t maxMem = 0;
    
    if (loadPMC(setup.fileName, MEM, isCode, &PC, &maxMem)) {
        IR = MEM[PC];
        while (IR.code != STOP) {
            if (setup.clean) system("clear");
            if (setup.debugMode) printMem(maxMem, MEM, isCode, setup.base, setup.uns, PC, AC, OR);
            PC += 1;

            switch(IR.mode) {
                case Current:  OR = IR.arg; break;
                case Direct:   OR = MEM[IR.arg].val; break;
                case Indirect: OR = MEM[MEM[IR.arg].val].val; break;
                case Numbered: OR = MEM[AC + IR.arg].val; break;
            }
            
            switch(IR.code) {
                case LOAD:  AC = OR; break;
                case STORE: MEM[OR].val = AC; break;
                case JUMP:  PC = OR; break;
                case JNEG:  if (AC < 0) PC = OR; break;
                case JZERO: if (AC == 0) PC = OR; break;
                case ADD:   AC += OR; break;
                case SUB:   AC -= OR; break;
                case MULT:  AC *= OR; break;
                case DIV:   AC /= OR; break;
                case AND:   AC &= OR; break;
                case OOR:   AC |= OR; break;
                case NOT:   AC = ~OR; break;
                case CMP:   AC = AC == OR ? -1 : 0; break;
                case SHZ:   AC = (OR < 0) ? AC >> -OR : AC << OR; break;
                case SHC:   AC = (OR < 0) ? (AC << (16 - -OR)) | (AC >> -OR) :
                                            (AC >> (16 - OR)) | (AC << OR); break;
            }
                    
            IR = MEM[PC];
            if (setup.stop) getchar();
        }

        printMem(maxMem, MEM, isCode, setup.base, setup.uns, PC, AC, OR);
    }
    
    
    return 0;
}