#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// =========================================================
// 1. 中間碼與詞法分析
// =========================================================
typedef struct { char op[16]; char arg1[32]; char arg2[32]; char result[32]; } Quad;
Quad quads[1000];
int quad_count = 0;

void emit(const char* op, const char* a1, const char* a2, const char* res) {
    strcpy(quads[quad_count].op, op); strcpy(quads[quad_count].arg1, a1);
    strcpy(quads[quad_count].arg2, a2); strcpy(quads[quad_count].result, res);
    printf("%03d: %-10s %-10s %-10s %-10s\n", quad_count, op, a1, a2, res);
    quad_count++;
}

typedef enum {
    TK_FUNC, TK_RETURN, TK_IF, TK_WHILE, TK_ID, TK_NUM, 
    TK_LPAREN, TK_RPAREN, TK_LBRACE, TK_RBRACE, TK_COMMA, TK_SEMICOLON,
    TK_ASSIGN, TK_PLUS, TK_MINUS, TK_MUL, TK_DIV, 
    TK_EQ, TK_LT, TK_GT, TK_EOF
} TokenType;

typedef struct { TokenType type; char text[32]; } Token;
Token cur_token;
const char *src;

void next_token() {
    while (1) {
        while (isspace(*src)) src++;
        if (*src == '\0') { cur_token.type = TK_EOF; return; }
        if (*src == '/') {
            if (*(src+1) == '/') { src += 2; while (*src != '\n' && *src != '\0') src++; continue; }
            if (*(src+1) == '*') { src += 2; while(!(*src=='*' && *(src+1)=='/') && *src!='\0') src++; if(*src) src+=2; continue; }
        }
        break;
    }
    if (isdigit(*src)) {
        int i=0; while(isdigit(*src)) cur_token.text[i++] = *src++;
        cur_token.text[i] = '\0'; cur_token.type = TK_NUM;
    } else if (isalpha(*src) || *src == '_') {
        int i=0; while(isalnum(*src) || *src == '_') cur_token.text[i++] = *src++;
        cur_token.text[i] = '\0';
        if (strcmp(cur_token.text, "func") == 0) cur_token.type = TK_FUNC;
        else if (strcmp(cur_token.text, "return") == 0) cur_token.type = TK_RETURN;
        else if (strcmp(cur_token.text, "if") == 0) cur_token.type = TK_IF;
        else if (strcmp(cur_token.text, "while") == 0) cur_token.type = TK_WHILE;
        else cur_token.type = TK_ID;
    } else {
        cur_token.text[0] = *src; cur_token.text[1] = '\0';
        switch (*src++) {
            case '(': cur_token.type = TK_LPAREN; break;
            case ')': cur_token.type = TK_RPAREN; break;
            case '{': cur_token.type = TK_LBRACE; break;
            case '}': cur_token.type = TK_RBRACE; break;
            case '+': cur_token.type = TK_PLUS; break;
            case '-': cur_token.type = TK_MINUS; break;
            case '*': cur_token.type = TK_MUL; break;
            case '/': cur_token.type = TK_DIV; break;
            case ',': cur_token.type = TK_COMMA; break;
            case ';': cur_token.type = TK_SEMICOLON; break;
            case '<': cur_token.type = TK_LT; break;
            case '>': cur_token.type = TK_GT; break;
            case '=': if(*src=='='){src++; cur_token.type=TK_EQ; strcpy(cur_token.text,"==");} else cur_token.type=TK_ASSIGN; break;
        }
    }
}

// =========================================================
// 2. 語法解析 (Parser)
// =========================================================
int t_idx = 0;
void new_t(char *s) { sprintf(s, "t%d", ++t_idx); }
void expression(char *res);
void statement();

void factor(char *res) {
    if (cur_token.type == TK_NUM) { new_t(res); emit("IMM", cur_token.text, "-", res); next_token(); }
    else if (cur_token.type == TK_ID) {
        char name[32]; strcpy(name, cur_token.text); next_token();
        if (cur_token.type == TK_LPAREN) {
            next_token(); int cnt=0;
            while(cur_token.type != TK_RPAREN) {
                char arg[32]; expression(arg); emit("PARAM", arg, "-", "-"); cnt++;
                if(cur_token.type == TK_COMMA) next_token();
            }
            next_token(); new_t(res); char c_str[10]; sprintf(c_str, "%d", cnt);
            emit("CALL", name, c_str, res);
        } else strcpy(res, name);
    } else if (cur_token.type == TK_LPAREN) { next_token(); expression(res); next_token(); }
}

void term(char *res) {
    char l[32], r[32], t[32]; factor(l);
    while (cur_token.type == TK_MUL || cur_token.type == TK_DIV) {
        char op[10]; strcpy(op, cur_token.type == TK_MUL ? "MUL" : "DIV");
        next_token(); factor(r); new_t(t); emit(op, l, r, t); strcpy(l, t);
    }
    strcpy(res, l);
}

void arith_expr(char *res) {
    char l[32], r[32], t[32]; term(l);
    while (cur_token.type == TK_PLUS || cur_token.type == TK_MINUS) {
        char op[10]; strcpy(op, cur_token.type == TK_PLUS ? "ADD" : "SUB");
        next_token(); term(r); new_t(t); emit(op, l, r, t); strcpy(l, t);
    }
    strcpy(res, l);
}

void expression(char *res) {
