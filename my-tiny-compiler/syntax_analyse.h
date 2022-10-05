//
// Created by Mr.K on 2022-08-07.
//

#ifndef SYNTAX_ANALYSE_H
#define SYNTAX_ANALYSE_H

#include <string>

using namespace std;

typedef struct AstTree Ast;
struct AstTree{
    Ast* lc;
    Ast* rc;
    int node_type;  // 存储节点是语法树的什么部分
    struct Data{  // 存储节点对应的值以及类型
        int data_type;  // 在后面一些地方有用处
        string data_data;
        Data() : data_type(0) {};
    } data;
    AstTree() : lc(nullptr), rc(nullptr), node_type(0), data(Data()) {};
};

typedef struct VarList VL;
struct VarList{  // 第一个节点作为头节点
    string var_name;
    VL* next;
    VarList() : next(nullptr) {};
};

typedef struct VarListStack VLS;
struct VarListStack{
    VL* local_var_list;  // 放置当前大括号内的局部变量
    VLS* next;  // 下一层大括号内的变量
    VarListStack() : local_var_list(nullptr), next(nullptr) {};
};


/*语法树节点类型*/
typedef enum NodeKind{
    // 外部定义序列
    EXT_DEF_LIST = 1,
    // 外部变量定义
    EXT_VAR_DEF,
    // 外部变量类型
    EXT_VAR_TYPE,
    // 外部变量
    EXT_VAR,
    // 外部变量序列
    EXT_VAR_LIST,
    //数组定义
    ARRAY_DEF,
    //数组类型
    ARRAY_TYPE,
    // 数组名
    ARRAY_NAME,
    // 函数定义
    FUNC_DEF,
    // 函数返回类型
    FUNC_RETURN_TYPE,
    // 函数名
    FUNC_NAME,
    // 函数声明
    FUNC_CLAIM,
    // 函数体
    FUNC_BODY,
    // 函数形参列表
    FUNC_PARA_LIST,
    // 函数形参定义
    FUNC_PARA_DEF,
    // 函数形参类型
    FUNC_PARA_TYPE,
    // 函数形参名
    FUNC_PARA_NAME,
    // 局部变量定义序列
    LOCAL_VAR_DEF_LIST,
    // 局部变量定义
    LOCAL_VAR_DEF,
    // 局部变量类型
    LOCAL_VAR_TYPE,
    // 局部变量名序列
    LOCAL_VAR_NAME_LIST,
    // 局部变量名
    LOCAL_VAR_NAME,
    // 语句序列处理
    STATE_LIST,
    // if复合语句
    IF_STATE,
    // if-else复合语句
    IF_ELSE_STATE,
    // if语句块
    IF_PART,
    // if条件
    IF_COND,
    // else部分
    ELSE_PART,
    // while复合语句
    WHILE_STATE,
    // while条件部分
    WHILE_COND,
    // while语句块
    WHILE_BODY,
    // for复合语句
    FOR_STATE,
    // for条件部分
    FOR_COND,
    // for条件1\2\3
    FOR_COND_1,
    FOR_COND_2,
    FOR_COND_3,
    // for语句块
    FOR_BODY,
    // return语句
    RETURN_STATE,
    // break语句
    BREAK_STATE,
    // continue语句
    CONTINUE_STATE,
    // 运算符
    OPERATOR,
    // 操作数
    OPERAND,
    // 表达式
    EXPRESSION,

}node_kind;




/*错误类型*/
typedef enum ErrorKind{
    // 外部变量定义错误
    EXT_DEF_ERR,
    // 变量类型错误
    VAR_TYPE_ERR,
    // 外部定义重复
    VAR_DEF_DUPLICATED,
    //语法错误
    GRAMMA_ERR,
    // 函数定义处错误
    FUNC_DEF_ERR,
    // 函数形参定义出错
    FUNC_PARA_DEF_ERR,
    // 复合语句内错误
    COMP_LIST_ERR,
    // 局部变量定义错误
    LOCAL_VAR_DEF_ERR,
    // 局部变量名重复
    LOCAL_VAR_DUPLICATED,
    // if语句错误
    IF_ERR,
    // while语句错误
    WHILE_ERR,
    // for语句错误
    FOR_ERR,
    // void函数有返回值
    VOID_FUNC_WITH_RETURN,
    // break语句缺少分号
    BREAK_LACK_SEMI,
    // 在非循环中出现break
    BREAK_IN_UNCYCLE,
    // continue缺少分号
    CONTINUE_LACK_SEMI,
    // 在非循环中出现continue
    CONTINUE_IN_UNCYCLE,
    // 表达式出现未知符号
    UNKNOWN_TOKEN,
    // 表达式处错误
    EXPRESSION_ERR,
    // 未定义变量
    UNDEF_VAR,

}error_kind;




// 抛出错误
void raise_error(int line_cnt, int error_type);
// 常用一系列错误处理
void deal_with_error_1(int line_cnt, int error_type, Ast* cur_node);
// 先序遍历
void preorder(Ast* cur_node, int depth);
// 打印节点
void print_node(Ast* cur_node);
// 回收节点
void free_tree(Ast* root);
//获取第一个不是注释和头文件的token
void filter_anno_include();
// 将token返回到文件流
void return_token(FILE* fp);
//向某个域的定义添加变量
int add_var_name(string token_text);
// 查看变量名是否被定义过
int check_name_exist(string token_text);
//最终调用函数
void syntax_analyse();
// 一开始的语法分析，进行一些预处理以及调用extern_def_list
Ast* begin_program();
// 外部定义序列
Ast* extern_def_list();
// 辅助函数extern_def处理普通变量定义、数组定义还是函数定义
Ast* extern_def();
// 外部变量定义节点
Ast* extern_var_def();
// extern_var_list 类型
Ast* extern_var_list();
// 数组定义节点
Ast* arr_def();
// 函数定义节点
Ast* func_def();
// 函数形参列表
Ast* para_list();
// 函数形参节点
Ast* para_def();
// 大括号扩着的复合语句
Ast* comp_state();
// 局部变量定义序列节点
Ast* local_var_def_list();
// 局部变量定义节点，处理一行局部变量定义
Ast* local_var_def();
// 语句序列处理
Ast* state_list();
// 一条语句处理
Ast* single_state();
// 处理if语句块
void deal_with_if(Ast* cur_node);
// 处理while语句块
void deal_with_while(Ast* cur_node);
// 处理for语句块
void deal_with_for(Ast* cur_node);
// break语句
void deal_with_break(Ast* cur_node);
// continue语句
void deal_with_continue(Ast* cur_node);
// 表达式处理
Ast* expression(int end_sym);
// 优先级比较函数
char precede(int c1,int c2);

#endif



