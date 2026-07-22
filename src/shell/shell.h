
typedef struct {
    const char* name;
    void (*func) (char (*)[32]);
} SHELL_CMD;


void parse();
void shell_exec(const char* prompt);
void shell_main();