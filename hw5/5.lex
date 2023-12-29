%{
#define YYSTYPE void*
#include "5.tab.h"
extern "C" int yylex();
%}

%option yylineno
%option noyywrap

%%

[/][/].*\n      ; // comment
[0-9]*          {
                  printf("IntLiteral %s\n", yytext);
                  yylval = strdup(yytext);
                  return IntLiteral;
                }
[f][u][n][c]    { printf("FunctionBegin\n"); return FunctionBegin; }
[e][n][d]       { printf("FunctionEnd\n"); return FunctionEnd; }
[d][e][f]       { printf("DefineToken\n"); return DefineToken; }
[c][a][l][l]    { printf("CallFunction\n"); return CallFunction; }
[i][f]          { printf("IfToken\n"); return IfToken; }
[g][o][t][o]    { printf("GotoToken\n"); return GotoToken; }
[P][U][T]       { printf("PutToken\n"); return PutToken; }
[F][L][U][S][H] { printf("FlushToken\n"); return FlushToken; }
[R][A][N][D]    { printf("RandToken\n"); return RandToken; }
[f][o][r]       { printf("ForToken\n"); return ForToken; }
[i][n]          { printf("InToken\n"); return InToken; }
[i][n][t]       { printf("IntToken\n"); return IntToken; }
[v][o][i][d]    { printf("VoidToken\n"); return VoidToken; }
[e][l][s][e]    { printf("ElseToken\n"); return ElseToken; }
[w][h][i][l][e] { printf("WhileToken\n"); return WhileToken; }
[A-Za-z_]+      { // identifier or array
                  printf("Identifier %s\n", yytext);
                  yylval = strdup(yytext);
                  return Identifier;
                }
[ \t\r\n]      ; // whitespace
.              { return *yytext; }

%%