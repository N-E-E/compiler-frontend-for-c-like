//
// Created by Mr.K on 2022-08-02.
//

#include "get_token.h"

#include <cstdio>


string token_text = "";  // 存放读取的一个词
char c;  // 存放从文件流中读取的字符
int line_cnt = 1;  // 读取的代码行数




int isNum(char c)
{
    if (c >= 48 && c <= 57) {
        return 1;
    }
    return 0;
}

int isXnum(char c)
{
    if (isNum(c) || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F') {
        return 1;
    }
    return 0;
}

int isLetter(char c)
{
    if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') {
        return 1;
    }
    return 0;
}

int isLorN(char c)
{
    if (isNum(c) || isLetter(c)) {
        return 1;
    }
    return 0;
}

int add2token(string s, char c){
    token_text += c;
    return 1;
}

int getToken(FILE* fp){
    token_text = "";
    do{  // 过滤缩进和空行
        c = fgetc(fp);
        if (c == '\n') line_cnt++;
    } while (c == ' ' || c == '\n');
    /*标识符、关键字、数组的识别*/
    if (c == '_' || isLetter(c)){
        add2token(token_text, c);
        while (isLorN((c = getc(fp)))){
            token_text += c;
        }
        /*判断是否是关键字*/
        if (token_text == "int") {ungetc(c, fp); return INT;}
        if (token_text == "double") {ungetc(c, fp);return DOUBLE;}
        if (token_text == "char") {ungetc(c, fp);return CHAR;}
        if (token_text == "short") {ungetc(c, fp);return SHORT;}
        if (token_text == "long") {ungetc(c, fp);return LONG;}
        if (token_text == "float") {ungetc(c, fp);return FLOAT;}
        if (token_text == "if") {ungetc(c, fp);return IF;}
        if (token_text == "else") {ungetc(c, fp);return ELSE;}
        if (token_text == "do") {ungetc(c, fp);return DO;}
        if (token_text == "while") {ungetc(c, fp);return WHILE;}
        if (token_text == "for") {ungetc(c, fp);return FOR;}
        if (token_text == "struct") {ungetc(c, fp);return STRUCT;}
        if (token_text == "break") {ungetc(c, fp);return BREAK;}
        if (token_text == "switch") {ungetc(c, fp);return SWITCH;}
        if (token_text == "case") {ungetc(c, fp);return CASE;}
        if (token_text == "typedef") {ungetc(c, fp);return TYPEDEF;}
        if (token_text == "return") {ungetc(c, fp);return RETURN;}
        if (token_text == "continue") {ungetc(c, fp);return CONTINUE;}
        if (token_text == "void") {ungetc(c, fp);return VOID;}
        /*判断是否是数组*/
        if (c == '['){
            do{
                add2token(token_text, c);
                c = fgetc(fp);
            } while (isNum(c));
            if (c != ']') return ERROR_TOKEN;
            else{
                add2token(token_text, c);
                return ARRAY;
            }
        }
        else{  // 普通的标识符
            ungetc(c, fp);
            return IDENT;
        }
    }
    /*数字的识别*/
    if (isNum(c)){
        do{
            add2token(token_text, c);
        } while (isNum(c = getc(fp)));
        if (c != '.' && c != 'u' && c != 'l') {  // int
            if(c != ' ' && c != ';' && c!=')' && c!='+' && c!='-' && c!='*' && c!='/'){
                return ERROR_TOKEN;
            }
            ungetc(c,fp);
            return INT_CONST;
        }
        else if (c == '.'){  // float
            c = getc(fp);
            if (!isNum(c)) return ERROR_TOKEN;
            else{
                ungetc(c,fp);
                c='.';
                add2token(token_text,c);
                c=fgetc(fp);
                do{
                    add2token(token_text, c);
                } while (isNum(c=fgetc(fp)));
                if(c == 'f'){
                    add2token(token_text, c);
                    return FLOAT_CONST;
                }
                else{
                    ungetc(c,fp);
                }
                return FLOAT_CONST;
            }
        }
        else if(c == 'u'){
            add2token(token_text, c);
            c=fgetc(fp);
            if(c == 'l'){
                add2token(token_text, c);
                c = fgetc(fp);
                if(c == 'l'){
                    add2token(token_text, c);
                    return INT_CONST;
                }
                else{
                    ungetc(c,fp);
                    return INT_CONST;
                }
            }
            else{
                ungetc(c,fp);
                return INT_CONST;
            }
        }
        else if(c=='l'){
            add2token(token_text, c);
            return INT_CONST;
        }
        else{
            return ERROR_TOKEN;
        }
    }
    /*数字的识别：小数点开头的浮点型*/
    if(c == '.'){
        do{
            add2token(token_text, c);
        } while (isNum(c = fgetc(fp)));
        ungetc(c,fp);
        return FLOAT_CONST;
    }
    /*字符型常量*/
    if (c == '\''){
        add2token(token_text, c);
        if ((c = getc(fp)) != '\\'){  // 普通字符
            add2token(token_text, c);
            if ((c = fgetc(fp)) != '\'') return ERROR_TOKEN;
            else{
                add2token(token_text, c);
                return CHAR_CONST;
            }
        }
        else{  // 转义字符
            add2token(token_text, c);
            c = fgetc(fp);
            if (c=='n' || c=='t' || c=='\\' || c=='\'' || c=='\"'){  // 普通转义字符
                add2token(token_text, c);
                if ((c = fgetc(fp)) != '\'') return ERROR_TOKEN;
                else{
                    add2token(token_text, c);
                    return CHAR_CONST;
                }
            }
            else if (c == 'x'){  // 十六进制字符
                add2token(token_text, c);
                c = fgetc(fp);
                if (isXnum(c)){
                    add2token(token_text, c);
                    c = getc(fp);
                    if (isXnum(c)){
                        add2token(token_text, c);
                        if ((c = fgetc(fp)) != '\'') return ERROR_TOKEN;
                        else{
                            add2token(token_text, c);
                            return CHAR_CONST;
                        }
                    }
                    else{
                        if (c != '\'') return ERROR_TOKEN;
                        else{
                            add2token(token_text, c);
                            return CHAR_CONST;
                        }
                    }
                }
                else{
                    return ERROR_TOKEN;
                }
            }
            else if (c >= '0' && c <= '7'){
                add2token(token_text, c);
                c = fgetc(fp);
                if (c >= '0' && c <= '7'){
                    add2token(token_text, c);
                    c = fgetc(fp);
                    if (c >= '0' && c <= '7'){
                        add2token(token_text, c);
                        if ((c = fgetc(fp)) != '\'') return ERROR_TOKEN;
                        else{
                            add2token(token_text, c);
                            return CHAR_CONST;
                        }
                    }
                    else{
                        if (c != '\'') return ERROR_TOKEN;
                        else{
                            add2token(token_text, c);
                            return CHAR_CONST;
                        }
                    }
                }
                else{
                    if (c != '\'') return ERROR_TOKEN;
                    else{
                        add2token(token_text, c);
                        return CHAR_CONST;
                    }
                }
            }
            else return ERROR_TOKEN;
        }
    }
    /*字符串常量*/
    if (c == '"'){
        do{
            if(c!='\\') add2token(token_text,c);
            if(c=='\\'){
                c=fgetc(fp);
                if (c == '\n'){
                    line_cnt++;
                }
                else if (c == '"') return ERROR_TOKEN;
            }
        }while ((c=fgetc(fp)) != '"' && c!='\n');
        if (c == '"'){
            add2token(token_text, c);
            return STRING_CONST;
        }
        if (c == '\n') return ERROR_TOKEN;
    }
    /*除号与注释*/
    if (c == '/'){
        add2token(token_text, c);
        c = fgetc(fp);
        if (c == '/'){
            do{
                add2token(token_text, c);
                c = fgetc(fp);
            } while (c != '\n');
            ungetc(c, fp);
            return ANNO;
        }
        else if (c == '*'){
            while (1){
                add2token(token_text, c);
                c = fgetc(fp);
                if (c == '*'){
                    if ((c = fgetc(fp)) == '/'){
                        add2token(token_text, '*');
                        add2token(token_text, c);
                        return ANNO;
                    }
                }
                if (c == '\n'){
                    line_cnt++;
                }
            }
        }
        else{
            ungetc(c, fp);
            return DIVIDE;
        }
    }
    /*头文件和宏*/
    if(c == '#'){
        add2token(token_text, c);
        if(isLetter(c=fgetc(fp))){
            do{
                add2token(token_text, c);
            } while (isLetter(c = fgetc(fp)));
            if(token_text == "#include"){
                do{
                    add2token(token_text,c);
                } while ((c = fgetc(fp)) != '\n');
                ungetc(c,fp);
                return INCLUDE;
            }
            else if(token_text == "#define"){
                do{
                    add2token(token_text,c);
                } while ((c = fgetc(fp)) != '\n');
                ungetc(c, fp);
                return MACRO;
            }
            else{
                return ERROR_TOKEN;
            }
        }
        else{
            return ERROR_TOKEN;
        }
    }
    /*非法字符*/
    if(c == '@' || c == '?'){
        return ERROR_TOKEN;
    }
    /*其他符号*/
    switch (c){
        case ',':
            add2token(token_text,c);
            return COMMA;
        case ';':
            add2token(token_text,c);
            return SEMI;
        case '=':
            c=fgetc(fp);
            if(c=='='){
                add2token(token_text,c);
                add2token(token_text,c);
                return EQ;
            }
            ungetc(c,fp);
            add2token(token_text,'=');
            return ASSIGN;
        case '!':
            c=fgetc(fp);
            if(c=='='){
                add2token(token_text,'!');
                add2token(token_text,'=');
                return NEQ;
            }
            else{
                return ERROR_TOKEN;
            }
        case '+':
            add2token(token_text,'+');
            return PLUS;
        case '-':
            add2token(token_text,'-');
            return MINUS;
        case '&':
            c = fgetc(fp);
            if (c == '&'){
                add2token(token_text, '&');
                add2token(token_text, '&');
                return AND;
            }
            else{
                return ERROR_TOKEN;
            }
        case '|':
            c = fgetc(fp);
            if (c == '|'){
                add2token(token_text, '|');
                add2token(token_text, '|');
                return OR;
            }
            else{
                return ERROR_TOKEN;
            }
        case '(':
            add2token(token_text,c);
            return LP;
        case ')':
            add2token(token_text,c);
            return RP;
        case '{':
            add2token(token_text,c);
            return LB;
        case '}':
            add2token(token_text,c);
            return RB;
        case '[':
            add2token(token_text,c);
            return LM;
        case ']':
            add2token(token_text,c);
            return RM;
        case '*':
            add2token(token_text,c);
            return TIMES;
        case '>':
            add2token(token_text,c);
            if((c=fgetc(fp))=='='){
                add2token(token_text,c);
                return MOREEQUAL;
            }
            else{
                ungetc(c,fp);
                return MORE;
            }
        case '<':
            add2token(token_text,c);
            if((c=fgetc(fp))=='='){
                add2token(token_text,c);
                return LESSEQUAL;
            }
            else{
                ungetc(c,fp);
                return LESS;
            }
        case '~':  // 终止符
            return -1;
        default:
            return ERROR_TOKEN;
    }
}
