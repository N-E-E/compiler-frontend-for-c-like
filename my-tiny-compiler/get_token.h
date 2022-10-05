//
// Created by Mr.K on 2022-08-02.
//

#ifndef GET_TOKEN_H
#define GET_TOKEN_H

#include <string>

using namespace std;

/*词法分析编码列表*/
typedef enum token_kind{
    ERROR_TOKEN = 1,
    IDENT,  //标识符
    INT_CONST,  //整形常量
    FLOAT_CONST,
    CHAR_CONST,
    STRING_CONST,
    KEYWORD,
    INT,  // 8
    FLOAT,
    CHAR,
    LONG,
    SHORT,
    DOUBLE,
    VOID,  // 14
    ELSE,
    DO,
    WHILE,
    FOR,
    IF,
    BREAK,
    SWITCH,
    CASE,
    TYPEDEF,
    RETURN,
    CONTINUE,
    STRUCT,  //26
    LB,  //左大括号
    RB,  //右大括号
    LM,  //左中括号
    RM,  //右中括号
    SEMI,  //分号31
    COMMA,  //逗号
    /*EQ到MINUSMINUS为运算符，必须连在一起*/
    EQ,  //'=='
    NEQ,  //‘!=’
    ASSIGN,  //'='35
    LP,  //左括号
    RP,  //右括号
    TIMES,  //乘法
    DIVIDE,  //除法
    PLUS,  //加法40
    OR, //或运算
    POUND,  //井号42
    MORE,  //大于号
    LESS,  //小于号
    MOREEQUAL,  //大于等于
    LESSEQUAL,  //小于等于
    MINUS,  //减法
    AND,  //与运算
    ANNO,  //注释
    INCLUDE,  //头文件引用
    MACRO,  //宏定义
    ARRAY,  //数组
}token_kind;

static string token_kind_arr[53] = {"a", "b", "ident", "int_const", "float_const", "char_const", \
    "string_const", "keyword", "int", "float", "char", "long", "short", \
    "double", "void", "else", "do", "while", "for", "if", "break",\
    "switch", "case", "typedef", "return", "continue", "struct", "左大括号",\
    "右大括号", "左中括号", "右中括号", "分号", "逗号", "是否等于", "不等于",\
    "赋值", "左括号","右括号", "乘法", "除法", "加法", "或运算",\
    "井号", "大于号", "小于号", "大于等于", "小于等于", "减法", "与运算", \
    "注释","头文件", "宏", "数组"};


// 判断是否是数字
int isNum(char c);

// 判断是否是十六进制数
int isXnum(char c);

// 判断是否是字母
int isLetter(char c);

// 判断是否是字母或数字
int isLorN(char c);

// 向字符串中添加字符
int add2token(string s, char c);

// 读取输入并返回当前读取token的类型
int getToken(FILE* fp);

#endif
