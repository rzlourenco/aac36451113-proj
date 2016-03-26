/**
   Standard I/O adresses
*/

void outbyte(char c);
char inbyte(void);

void printc(char c);
void prints(char const *s);
void printi(int i);

void my_printf(char const *fmt, ...);

void outbyte(char c)
{
    volatile char *COUT = (char *) 0xFFFFFFC0;
    *COUT = c;
}

char inbyte(void)
{
    volatile char *CIN = (char *) 0xFFFFFFC0;
    return (char) *CIN;
}

void printc(char c) {
    outbyte(c);
}

void prints(char const *s) {
    while (*s)
        outbyte(*s++);
}

void printi(int i) {
    prints("hah!");

}

void my_printf(char const *fmt, ...) {

}
