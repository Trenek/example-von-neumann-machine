union memCell {
    struct {
        int16_t arg : 9;
        uint16_t mode : 2;
        uint16_t code : 4;
        uint16_t : 1;
    };
    int16_t val;
};

enum code {
    STOP,
    LOAD,
    STORE,
    JUMP,
    JNEG,
    JZERO,
    ADD,
    SUB,
    MULT,
    DIV,
    AND,
    OOR,
    NOT,
    CMP,
    SHZ,
    SHC
};

static const char *const codeName[] = {
    [STOP] =  "stop",
    [LOAD] =  "load",
    [STORE] = "store",
    [JUMP] =  "jump",
    [JNEG] =  "jneg",
    [JZERO] = "jzero",
    [ADD] =   "add",
    [SUB] =   "sub",
    [MULT] =  "mult",
    [DIV] =   "div",
    [AND] =   "and",
    [OOR] =   "or",
    [NOT] =   "not",
    [CMP] =   "cmp",
    [SHZ] =   "shz",
    [SHC] =   "shc"
};

enum mode {
    Current,
    Direct,
    Indirect,
    Numbered
};

static const char *const modeName[] = {
    [Current] = "C",
    [Direct] = "D",
    [Indirect] = "I",
    [Numbered] = "N"
};

bool loadPMC(const char * fileName, union memCell MEM[512], bool isCode[512], uint16_t *PC, uint16_t *maxMem);
void printMem(uint16_t maxMem, union memCell MEM[static maxMem], bool isCode[static maxMem], int base, bool uns, uint16_t PC, uint16_t AC, uint16_t OR);