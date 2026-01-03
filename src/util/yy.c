/*
 * pre-defined symbols, integration with my lers project
 */

typedef unsigned long IdxType;
#define YYTEXT_MAXLEN 999

char *g_buffer;
char *g_buffer_ptr;
IdxType g_buflen;
IdxType g_bufidx;

char yytext[YYTEXT_MAXLEN];
IdxType yyleng;
