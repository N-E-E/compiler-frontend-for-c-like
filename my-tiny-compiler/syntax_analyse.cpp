//
// Created by Mr.K on 2022-08-07.
//

#include "syntax_analyse.h"
#include "get_token.h"
#include <cstdio>
#include <iostream>
#include <stack>

extern string token_text;
extern int line_cnt;
extern FILE *fp;
int mistake = 0;
int isVoid = 0, isInCyc = 0, hasReturn = 0;
int word_type, word_type_cp;
string token_text_cp;
VLS* var_list_stack;

string node_kind_arr[] = {"a", "外部定义序列", "外部变量定义", "外部变量类型", "外部变量",\
    "外部变量序列", "数组定义", "数组类型", "数组名", "函数定义", "函数返回类型",\
    "函数名", "函数声明", "函数体", "函数形参列表", "函数形参定义", "函数形参类型",\
    "函数形参名", "局部变量定义序列", "局部变量定义", "局部变量类型", "局部变量名序列",\
    "局部变量名", "语句序列处理", "if复合语句", "if-else复合语句", "if语句块", \
    "if条件", "else部分", "while复合语句", "while条件部分", "while语句块", \
    "for复合语句", "for条件部分", "for条件1", "for条件2", "for条件3", "for语句块",\
    "return语句","break语句", "continue语句", "运算符", "操作数", "表达式"};

string error_kind_arr[] = {"外部定义处错误", "变量类型错误","变量重复定义","语法错误",\
    "函数定义处错误", "函数形参定义出错", "复合语句内错误", "局部变量定义错误", "局部变量名重复",\
    "if语句错误", "while语句错误", "for语句错误", "void函数有返回值","break语句缺少分号", "在非循环中出现break",\
    "continue缺少分号", "在非循环中出现continue", "表达式出现未知符号", "表达式处错误",\
    "未定义变量"};


// 抛出错误
void raise_error(int line_cnt, int error_type){
    cout << "在第" << line_cnt << "行出现错误" << endl;
    cout << "错误类型：" << error_kind_arr[error_type] << endl;
    exit(0);
}

// 常用一系列错误处理
void deal_with_error_1(int line_cnt, int error_type, Ast* cur_node){
    free_tree(cur_node);
    mistake = 1;
    raise_error(line_cnt, error_type);
}

// 先序遍历
void preorder(Ast* cur_node, int depth){
    if (cur_node == nullptr) return;
    else{
        for (int i = 0; i < depth; i++){
            cout << "\t";
        }
        print_node(cur_node);
    }
    preorder(cur_node->lc, depth+1);
    preorder(cur_node->rc, depth);
}

// 打印节点
void print_node(Ast* cur_node){
    cout << node_kind_arr[cur_node->node_type] << ":";
    if (cur_node->data.data_data.empty()){
        cout << endl;
    }
    else{
        cout << cur_node->data.data_data << endl;
    }
}

// 回收节点
void free_tree(Ast* root){
    if (root){
        free_tree(root->lc);
        free_tree(root->rc);
        delete root;
    }
}

//获取第一个不是注释和头文件的token
void filter_anno_include(){
    word_type = getToken(fp);
    while (word_type == ANNO || word_type == INCLUDE || word_type == MACRO){
        word_type = getToken(fp);
    }
}

// 将token返回到文件流
void return_token(FILE* fp){
    ungetc(' ', fp);  // 去掉会出现bug！
    for (int i = token_text.size()-1; i >= 0; i--){
        ungetc(token_text[i], fp);
    }
}

//向某个域的定义添加变量
int add_var_name(string token_text){
    int flag = 0;
    VLS* p = var_list_stack;
    while (p->next != nullptr){
        p = p->next;  // 一直找到当前语句块所在范围
    }
    VL* cur_var_list = p->local_var_list;
    VL *q = cur_var_list;
    while (q->next != nullptr){
        q = q->next;
        if (q->var_name == token_text){
            flag = 1;
            break;
        }
    }
    if (flag == 1){
        mistake = 1;
        raise_error(line_cnt, VAR_DEF_DUPLICATED);
        return flag;
    }
    else{
        q->next = new VarList;
        q->next->var_name = token_text;
        return flag;
    }
}

// 查看变量名是否被定义过
int check_name_exist(string token_text){  // 返回1表示定义过
    if (mistake == 1) return 0;
    int flag = 0;
    VLS* p = var_list_stack;
    while (p->next != nullptr){
        p = p->next;
    }
    VL* q = p->local_var_list;
    while (q->next != nullptr){
        q = q->next;
        if (q->var_name == token_text){
            flag = 1;
            return 1;
        }
    }
    q = var_list_stack->local_var_list;  // 全局域
    while (q->next != nullptr){
        q = q->next;
        if (q->var_name == token_text){
            flag = 1;
            return 1;
        }
    }
    if (flag == 0){
        raise_error(line_cnt, UNDEF_VAR);
        mistake = 1;
    }
    return flag;
}

//最终调用函数
void syntax_analyse(){
    Ast* root = begin_program();
    if (root == nullptr || mistake == 1){
        cout << "出现语法错误，语法分析结束" << endl;
        return;
    }
    else{
        cout << "语法正确，语法树先序遍历如下：" << endl;
        preorder(root, 0);
    }
}

// 一开始的语法分析，进行一些预处理以及调用extern_def_list
Ast* begin_program(){
    filter_anno_include();
    // 建立最外层变量表，即全局变量
    var_list_stack = new VarListStack;
    var_list_stack->local_var_list = new VarList;
    Ast* cur_node = extern_def_list();
    if (cur_node == nullptr){  // 某个地方语法有错
        mistake = 1;
        return nullptr;
    }
    else{
        cur_node->node_type = EXT_DEF_LIST;
        return cur_node;
    }
}

// 外部定义序列
Ast* extern_def_list(){
    if (mistake == 1 || word_type == -1) return nullptr;
    Ast* cur_node = new AstTree;
    cur_node->node_type = EXT_DEF_LIST;
    cur_node->lc = extern_def();
    filter_anno_include();
    cur_node->rc = extern_def_list();
    return cur_node;
}

// 辅助函数extern_def处理普通变量定义、数组定义还是函数定义
Ast* extern_def(){  // 进入时存储着类型判断token
    if (!(word_type >= 8 && word_type <= 14)){
        raise_error(line_cnt, EXT_DEF_ERR);
    }
    word_type_cp = word_type;  // 保存类型
    word_type = getToken(fp);
    if (word_type != IDENT && word_type != ARRAY){
        raise_error(line_cnt, EXT_DEF_ERR);
    }
    token_text_cp = token_text;  // 保存变量名
    int next_word_type = getToken(fp);  // 看看下一个是啥来判断是否是函数定义
    Ast* p = new AstTree;
    if (next_word_type == LP){
        word_type = next_word_type;  // 现在w_t存储的是最新读入的token的类型,就是‘（’
        p = func_def();
    }
    else if (word_type == ARRAY){  // 简化处理：数组不会连续定义
        if (next_word_type != SEMI && next_word_type != COMMA){
            deal_with_error_1(line_cnt-1, EXT_DEF_ERR, p);
        }
        word_type = next_word_type;
        p = arr_def();
    }
    else{
        if (next_word_type != SEMI && next_word_type != COMMA){
            deal_with_error_1(line_cnt-1, EXT_DEF_ERR, p);
        }
        word_type = next_word_type;  // 现在w_t存储的是最新读入的token的类型
        p = extern_var_def();
    }
    return p;
}

// 外部变量定义节点
Ast* extern_var_def(){  // 进入时word_type_cp存储着类型，token_text_cp存储变量名，w_t、token_text存储后一个token
    if (mistake == 1) return nullptr;
    if (word_type_cp == VOID){
        raise_error(line_cnt, VAR_TYPE_ERR);
    }
    Ast* cur_node = new AstTree;
    cur_node->node_type = EXT_VAR_DEF;
    Ast* p = new AstTree;
    p->node_type = EXT_VAR_TYPE;
    p->data.data_type = word_type_cp;
    p->data.data_data = token_kind_arr[word_type_cp];
    cur_node->lc = p;
    // p的右子树为extern_var_list类型
    p->rc = extern_var_list();
    return cur_node;
}

// extern_var_list 类型
Ast* extern_var_list(){  // w_t_cp存储类型, t_t_cp存储变量名，w_t存储分隔符类型
    if (mistake == 1) return nullptr;
    // 当前读入的变量没问题，可以生成
    int state = add_var_name(token_text_cp);
    if (state == 1){
        raise_error(line_cnt, VAR_DEF_DUPLICATED);
        return nullptr;
    }
    Ast* cur_node = new AstTree;
    cur_node->node_type = EXT_VAR_LIST;
    cur_node->lc = new AstTree;
    Ast* p = cur_node->lc;
    p->node_type = EXT_VAR;
    p->data.data_data = token_text_cp;
    // 开始判断
    if (word_type != COMMA && word_type != SEMI){
        raise_error(line_cnt, GRAMMA_ERR);
    }

    if (word_type == SEMI){  // 这一行的变量定义结束
        return cur_node;
    }
    // 连续的同种变量定义, 注意保持进入函数时的一致性
    else if (word_type == COMMA){
        word_type = getToken(fp);
        if (word_type != IDENT){
            raise_error(line_cnt, EXT_DEF_ERR);
        }
        word_type_cp = word_type;  // 保持一致性
        token_text_cp = token_text;
        word_type = getToken(fp);
        cur_node->rc = extern_var_list();
        return cur_node;
    }
    else{
        deal_with_error_1(line_cnt, UNKNOWN_TOKEN, cur_node);
        return nullptr;
    }
    // 退出时w_t保持在最后一个分隔符
}

// 数组定义节点
Ast* arr_def(){  // 进入时word_type_cp存储着类型，token_text_cp存储数组名，w_t、token_text存储后一个token,也就是分号
    if (mistake == 1) return nullptr;
    if (word_type_cp == VOID){
        raise_error(line_cnt, VAR_TYPE_ERR);
    }
    Ast* cur_node = new AstTree;
    cur_node->node_type = ARRAY_DEF;
    Ast* p = new AstTree;
    p->node_type = ARRAY_TYPE;
    p->data.data_data = token_kind_arr[word_type_cp];
    cur_node->lc = p;
    p->rc = new AstTree;  // 数组名节点
    p = p->rc;
    p->node_type = ARRAY_NAME;
    p->data.data_data = token_text_cp;
    // 可以再加一个数组大小的节点. to do.
    return cur_node;
    // 退出时w_t,t_t保持在最后一个分隔符
}

// 函数定义节点
Ast* func_def(){  // 进入时w_t_cp存储着类型，t_t_cp存储数组名，w_t、t_t存储后一个token,也就是左括号
    if (mistake == 1) return nullptr;
    Ast* cur_node = new AstTree;
    cur_node->node_type = FUNC_DEF;  // 先设为函数定义
    Ast* p = new AstTree;
    p->node_type = FUNC_RETURN_TYPE;
    p->data.data_data = token_kind_arr[word_type_cp];
    if (word_type_cp == VOID) isVoid = 1;  // 后面有用
    else isVoid = 0;
    cur_node->lc = p;
    p->rc = new AstTree;  // 函数名节点
    p = p->rc;
    p->node_type = FUNC_NAME;
    p->data.data_data = token_text_cp;
    // 进入函数域，变量栈加一层
    VLS * last = var_list_stack;
    while (last->next != nullptr){
        last = last->next;
    }
    last->next = new VarListStack;
    last->next->local_var_list = new VarList;
    p->rc = para_list();  // 结束时读在右括号
    // 判断是定义还是声明
    filter_anno_include();
    if (word_type == SEMI){
        cur_node->node_type = FUNC_CLAIM;
        // 没有函数体节点
    }
    else if (word_type == LB){
        p->rc->rc = comp_state();
        p->rc->rc->node_type = FUNC_BODY;
    }
    else{
        raise_error(line_cnt, FUNC_DEF_ERR);
    }
    return cur_node;
}

// 函数形参列表
Ast* para_list(){
    if (mistake == 1) return nullptr;
    filter_anno_include();
    Ast* cur_node = new AstTree;
    cur_node->node_type = FUNC_PARA_LIST;
    if (word_type == RP){
        return cur_node;
    }
    cur_node->lc = para_def();  // 结束时读在右括号
    return cur_node;
}

// 函数形参节点
Ast* para_def(){  // 初次进入时w_t存的是形参类型
    if (mistake == 1) return nullptr;
    if (word_type == RP){  // 遇到右括号结束
        return nullptr;
    }
    else if (word_type == COMMA){  // 碰到逗号继续读，这里有个bug就是连续逗号不会报错
        filter_anno_include();
        return para_def();
    }
    else if (word_type >= 8 && word_type <= 13){  // wt是形参类型
        word_type_cp = word_type;  // wtc保存形参类型，这里也有bug就是后面又是类型也不会报错
        filter_anno_include();
        if (word_type != IDENT){
            raise_error(line_cnt, FUNC_PARA_DEF_ERR);
        }
        Ast* cur_node = new AstTree;  // 函数形参定义节点
        cur_node->node_type = FUNC_PARA_DEF;
        Ast* p = new AstTree;  // 函数形参类型
        p->node_type = FUNC_PARA_TYPE;
        p->data.data_data = token_kind_arr[word_type_cp];
        cur_node->lc = p;
        p->rc = new AstTree;
        p = p->rc;  // 函数形参名
        p->node_type = FUNC_PARA_NAME;
        int state = add_var_name(token_text);
        if (state == 1){
            mistake = 1;
            raise_error(line_cnt, VAR_DEF_DUPLICATED);
            return nullptr;
        }
        else{
            p->data.data_data = token_text;
        }
        filter_anno_include();  // 进入递归前还要读一个
        cur_node->rc = para_def();
        return cur_node;
    }
    else{
        mistake = 1;
        raise_error(line_cnt, FUNC_PARA_DEF_ERR);
        return nullptr;
    }
    // 结束时读在右括号
}

// 大括号扩着的复合语句
Ast* comp_state(){  // 进入时读了个'{'
    if (mistake == 1) return nullptr;
    Ast* cur_node = new AstTree;  // 语句块节点
    //node_type先不用写，不知道是哪种块
    filter_anno_include();
    if (word_type >= 8 && word_type <= 13){
        cur_node->lc = local_var_def_list();
    }
//    if (cur_node->lc == nullptr){
//        cur_node->lc = new AstTree;
//        cur_node->lc->node_type = LOCAL_VAR_DEF_LIST;
//    }
    Ast* p = cur_node->lc;
    p->rc = state_list();  // 语句序列处理,出来时会多读一个
    if (word_type == RB){
        return cur_node;
    }
    else{
        mistake = 1;
        raise_error(line_cnt, COMP_LIST_ERR);
        return nullptr;
    }
}

// 局部变量定义序列节点
Ast* local_var_def_list(){
    if (mistake == 1) return nullptr;
    Ast* cur_node = new AstTree;
    cur_node->node_type = LOCAL_VAR_DEF_LIST;
    cur_node->lc = local_var_def();  // 按行为节点单位处理局部变量定义
    return cur_node;
}

// 局部变量定义节点，处理一行局部变量定义
Ast* local_var_def(){
    if (mistake == 1) return nullptr;
    Ast* cur_node = new AstTree;
    cur_node->node_type = LOCAL_VAR_DEF;
    cur_node->lc = new AstTree;  // 局部变量类型节点
    Ast* p = cur_node->lc;
    p->node_type = LOCAL_VAR_TYPE;
    p->data.data_data = token_kind_arr[word_type];
    p->rc = new AstTree;  // 局部变量名列表
    p = p->rc;
    p->node_type = LOCAL_VAR_NAME_LIST;
    filter_anno_include();  // 读取一个变量
    if (word_type != IDENT){
        deal_with_error_1(line_cnt, LOCAL_VAR_DEF_ERR, cur_node);
        return nullptr;
    }
    p->lc = new AstTree;  // 局部变量名
    p = p->lc;
    p->node_type = LOCAL_VAR_NAME;
    p->data.data_data = token_text;
    // 把变量加入到变量表
    int state = add_var_name(token_text);
    if (state == 1){
        deal_with_error_1(line_cnt, LOCAL_VAR_DUPLICATED, cur_node);
        return nullptr;
    }
    // 开始循环读取变量
    while (1){
        filter_anno_include();
        if (word_type == SEMI){  // 定义结束
            // 多读一个保持一致性
            filter_anno_include();
            break;
        }
        else if (word_type == COMMA){
            filter_anno_include();
            if (word_type != IDENT){
                deal_with_error_1(line_cnt, LOCAL_VAR_DEF_ERR, cur_node);
                return nullptr;
            }
            p->rc = new AstTree;  // 变量名
            p = p->rc;
            p->node_type = LOCAL_VAR_NAME;
            p->data.data_data = token_text;
            state = add_var_name(token_text);
            if (state == 1){
                deal_with_error_1(line_cnt, LOCAL_VAR_DUPLICATED, cur_node);
                return nullptr;
            }
        }
        else{
            deal_with_error_1(line_cnt, LOCAL_VAR_DEF_ERR, cur_node);
            return nullptr;
        }
    }
    if (word_type >= 8 && word_type <= 13){
        cur_node->rc = local_var_def();  // 正常情况下从这里退出，多读一个
    }
    return cur_node;
}

// 语句序列处理
Ast* state_list(){  // 进入时应该多读一个
    if (mistake == 1) return nullptr;
    Ast* cur_node = new AstTree;
    cur_node->node_type = STATE_LIST;  // 语句序列节点
    cur_node->lc = single_state();  // 处理一条语句
    if (cur_node->lc == nullptr){
        return cur_node;  // 语句序列已经结束
    }
    else{
        filter_anno_include();  // 多读一个准备递归
        if (word_type != RB){
            cur_node->rc = state_list();
        }
        return cur_node;
    }
}

// 一条语句处理
Ast* single_state(){  // 进入时多读一个
    if (mistake == 1) return nullptr;
    Ast* cur_node = new AstTree;
    switch (word_type) {
        case IF: {
            deal_with_if(cur_node);
            return cur_node;
        }
        case WHILE: {
            deal_with_while(cur_node);
            return cur_node;
        }
        case FOR: {
            deal_with_for(cur_node);
            return cur_node;
        }
        case RETURN:{
            hasReturn = 1;
            if(isVoid == 1){
                deal_with_error_1(line_cnt, VOID_FUNC_WITH_RETURN, cur_node);
            }
            cur_node->node_type = RETURN_STATE;
            filter_anno_include();
            cur_node->lc = expression(SEMI);
            return cur_node;
        }
        case BREAK: {
            deal_with_break(cur_node);
            return cur_node;
        }
        case CONTINUE: {
            deal_with_continue(cur_node);
            return cur_node;
        }
        case INT_CONST:
        case FLOAT_CONST:
        case CHAR_CONST:
        case IDENT:
        case ARRAY:
            return expression(SEMI);
    }
    return cur_node;
}

// 处理if语句块
void deal_with_if(Ast* cur_node){
    filter_anno_include();
    if (word_type != LP){
        deal_with_error_1(line_cnt, IF_ERR, cur_node);
    }
    filter_anno_include();
    cur_node->lc = new AstTree;  // if-part节点
    Ast* p = cur_node->lc;
    p->node_type = IF_PART;
    p->lc = new AstTree;  // condition节点
    Ast* q = p->lc;
    q->node_type = IF_COND;
    q->lc = expression(RP);  // condition下挂的是表达式
    if (q->lc == nullptr) {
        deal_with_error_1(line_cnt, IF_ERR, cur_node);
    }
    filter_anno_include();
    if (word_type == LB){  // 多语句if
        filter_anno_include();
        q->rc = state_list();
    }
    else if (word_type >= 2 && word_type <= 19){  // 可能的开头token
        q->rc = single_state();
    }
    else{
        deal_with_error_1(line_cnt, IF_ERR, cur_node);
    }
    filter_anno_include();
    if (word_type == ELSE){
        cur_node->node_type = IF_ELSE_STATE;
        p->rc = new AstTree;
        q = p->rc;
        q->node_type = ELSE_PART;
        filter_anno_include();
        if (word_type == LB){  // 多语句else
            filter_anno_include();
            q->lc = state_list();
        }
        else if (word_type >= 2 && word_type <= 19){  // 可能的开头token
            q->lc = single_state();
        }
        else{
            deal_with_error_1(line_cnt, IF_ERR, cur_node);
        }
    }
    else{
        cur_node->node_type = IF_STATE;
        return_token(fp);
    }
}

// 处理while语句块
void deal_with_while(Ast* cur_node){
    isInCyc = 1;
    filter_anno_include();
    if (word_type != LP){
        deal_with_error_1(line_cnt, WHILE_ERR, cur_node);
        return;
    }
    filter_anno_include();
    cur_node->lc = new AstTree();
    Ast* p = cur_node->lc;
    p->node_type = WHILE_COND;
    p->lc = expression(RP);
    if (p->lc == nullptr){
        deal_with_error_1(line_cnt, WHILE_ERR, cur_node);
        return;
    }
    Ast* q = new AstTree;  // while_body
    q->node_type = WHILE_BODY;
    filter_anno_include();
    if (word_type == LB){  // 语句块
        filter_anno_include();  // 读入一个，一致性
        q->lc = state_list();
    }
    else if (word_type >= 2 && word_type <= 19){  // 此时读入的是单句的第一个token
        q->lc = single_state();
    }
    else{
        deal_with_error_1(line_cnt, WHILE_ERR, cur_node);
        return;
    }
    p->rc = q;
    cur_node->node_type = WHILE_STATE;
    isInCyc = 0;
}

// 处理for语句块
void deal_with_for(Ast* cur_node){
    isInCyc = 1;
    filter_anno_include();
    if (word_type != LP){
        deal_with_error_1(line_cnt, FOR_ERR, cur_node);
        return;
    }
    Ast* p = new AstTree;
    cur_node->lc = p;
    p->node_type = FOR_COND;
    // 开始处理条件部分
    Ast* q = new AstTree;
    q->node_type = FOR_COND_1;
    p->lc = q;
    filter_anno_include();  // 一致性
    q->lc = expression(SEMI);
    if (q->lc == nullptr){
        q->data.data_data = "无";
    }
    q->rc = new AstTree;
    q = q->rc;
    q->node_type = FOR_COND_2;
    filter_anno_include();
    q->lc = expression(SEMI);
    if (q->lc == nullptr){
        q->data.data_data = "无";
    }
    q->rc = new AstTree;
    q = q->rc;
    q->node_type = FOR_COND_3;
    filter_anno_include();
    q->lc = expression(RP);
    if (q->lc == nullptr){
        q->data.data_data = "无";
    }
    // 处理for_body
    q = new AstTree;
    q->node_type = FOR_BODY;
    filter_anno_include();
    if (word_type == LB) {
        filter_anno_include();  // 一致性
        q->lc = state_list();
    }
    else if (word_type >= 2 && word_type <= 19){
        q->lc = single_state();
    }
    else{
        deal_with_error_1(line_cnt, FOR_ERR, cur_node);
        return;
    }
    cur_node->node_type = FOR_STATE;
    isInCyc = 0;
}

// break语句
void deal_with_break(Ast* cur_node){
    filter_anno_include();
    if (word_type != SEMI){
        deal_with_error_1(line_cnt,BREAK_LACK_SEMI, cur_node);
        return;
    }
    if (isInCyc == 0){
        deal_with_error_1(line_cnt, BREAK_IN_UNCYCLE, cur_node);
        return;
    }
    cur_node->node_type = BREAK_STATE;
}

// continue语句
void deal_with_continue(Ast* cur_node){
    filter_anno_include();
    if (word_type != SEMI){
        deal_with_error_1(line_cnt,CONTINUE_LACK_SEMI, cur_node);
        return;
    }
    if (isInCyc == 0){
        deal_with_error_1(line_cnt, CONTINUE_IN_UNCYCLE,cur_node);
        return;
    }
    cur_node->node_type = CONTINUE_STATE;
}

// 表达式处理
Ast* expression(int end_sym){  // 进入时已经读入一个单词
    if (mistake == 1) return nullptr;
    if (word_type == end_sym) return nullptr;  // 空语句
    stack<Ast*> op;  // 存放运算符
    stack<Ast*> num;  // 存放操作数
    Ast* op_node;
    Ast* num_node;
    while (word_type != end_sym){
        if (word_type == IDENT && check_name_exist(token_text) != 1){
            raise_error(line_cnt, UNDEF_VAR);
        }
        if (word_type == IDENT || word_type == INT_CONST || word_type == FLOAT_CONST || word_type == CHAR_CONST || word_type == ARRAY || word_type == STRING_CONST){
            num_node = new AstTree();
            num_node->node_type = OPERAND;
            num_node->data.data_data = token_text;
            num.push(num_node);
            filter_anno_include();  // 为下一轮做准备
        }
        else{
            if (word_type == RP){
                if (num.size() < 2 || op.size() < 2){
                    raise_error(line_cnt, EXPRESSION_ERR);  // to do. 清理栈内存
                    return nullptr;
                }
                while (op.top()->data.data_type != LP){
                    op_node = op.top();  // 获取一个运算符
                    op.pop();
                    op_node->node_type = OPERATOR;
                    if (num.size() < 2){
                        raise_error(line_cnt, EXPRESSION_ERR);
                        return nullptr;
                    }
                    else{
                        op_node->rc = num.top();  // 获取两个操作数
                        num.pop();
                        op_node->lc = num.top();
                        num.pop();
                        num.push(op_node);
                    }
                }
                op.pop();  // 弹出左括号
            }
            else if (word_type == LP){
                op_node = new AstTree();
                op_node->node_type = OPERATOR;
                op_node->data.data_data = token_text;
                op_node->data.data_type = LP;  // data_type的作用只是优先级比较
                op.push(op_node);
            }
            else if (word_type >= EQ && word_type <= AND){
                if (op.empty()){  // 栈里面还没有运算符
                    op_node = new AstTree();
                    op_node->node_type = OPERATOR;
                    op_node->data.data_data = token_text;
                    op_node->data.data_type = word_type;
                    op.push(op_node);
                }
                else{
                    switch (precede(op.top()->data.data_type, word_type)) {
                        case '>':  // 要先把上一个表达式计算出来
                            if (num.size() < 2 || op.size() < 1){
                                raise_error(line_cnt, EXPRESSION_ERR);  // to do. 清理栈内存
                                return nullptr;
                            }
                            op_node = op.top();
                            op.pop();
                            op_node->node_type = OPERATOR;
                            op_node->rc = num.top();
                            num.pop();
                            op_node->lc = num.top();
                            num.pop();
                            num.push(op_node);
                            op_node = new AstTree();  // 然后把这一轮的运算符压入栈中
                            op_node->node_type = OPERATOR;
                            op_node->data.data_data = token_text;
                            op_node->data.data_type = word_type;
                            op.push(op_node);
                            break;
                        case '<':  // 直接压入栈中
                            op_node = new AstTree();
                            op_node->node_type = OPERATOR;
                            op_node->data.data_data = token_text;
                            op_node->data.data_type = word_type;
                            op.push(op_node);
                            break;
                        case '\0':
                            raise_error(line_cnt, EXPRESSION_ERR);  // to do. 清理栈内存
                            return nullptr;
                        default:
                            break;
                    }
                }
            }
            else{
                raise_error(line_cnt, EXPRESSION_ERR);  // to do. 清理栈内存
                return nullptr;
            }
            filter_anno_include();  // 为下一轮做准备
        }
    }
    if (op.empty() && num.empty()){  // 空语句
        return nullptr;
    }
    else if (op.empty() && num.size() == 1){
        Ast* cur_node = new AstTree();
        cur_node->node_type = EXPRESSION;
        cur_node->lc = num.top();
        num.pop();
        return cur_node;
    }
    else if (op.size() >= 1 && num.size() >= 2){
        while (!op.empty() && num.size() >= 2){
            op_node = op.top();
            op.pop();
            op_node->node_type = OPERATOR;
            op_node->rc = num.top();
            num.pop();
            op_node->lc = num.top();
            num.pop();
            num.push(op_node);
        }
        if (op.empty() && num.size() == 1){
            Ast* cur_node = new AstTree();
            cur_node->node_type = EXPRESSION;
            cur_node->lc = op_node;
            return cur_node;
        }
        else{
            raise_error(line_cnt, EXPRESSION_ERR);  // to do. 清理栈内存
            return nullptr;
        }
    }
    else{
        raise_error(line_cnt, EXPRESSION_ERR);  // to do. 清理栈内存
        return nullptr;
    }
}

// 优先级比较函数
char precede(int c1,int c2){
    if(c1==PLUS||c1==MINUS){
        switch (c2){
            case PLUS:
            case MINUS:
            //case RP:
            //case POUND:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            case ASSIGN:
                return '>';
            case TIMES:
            case DIVIDE:
            case LP:
            case AND:
            case OR:
                return '<';
            default:
                return '\0';
                break;

        }
    }
    else if(c1==TIMES||c1==DIVIDE){
        switch (c2){
            case PLUS:
            case MINUS:
            //case LP:
            //case POUND:
            case TIMES:
            case DIVIDE:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            case ASSIGN:
                return '>';
            case LP:
            case AND:
            case OR:
                return '<';
            default:
                return '\0';
        }
    }
    else if (c1==LP){
        switch (c2){
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case AND:
            case OR:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
                return '<';
            default:
                return '\0';
        }
    }
//    else if(c1==RP){
//        switch (c2){
//            case PLUS:
//            case MINUS:
//            case TIMES:
//            case DIVIDE:
//            case LP:
//            case MORE:
//            case LESS:
//            case MOREEQUAL:
//            case LESSEQUAL:
//            case EQ:
//            case NEQ:
//            case PLUSPLUS:
//            case MINUSMINUS:
//            case POUND:
//                return '>';
//            default:
//                return '\0';
//        }
//    }
    else if(c1==ASSIGN){
        switch (c2){
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            case AND:
            case OR:
                return '<';
//            case POUND:
//                return '>';
            default:
                return '\0';
        }
    }
    else if(c1==MORE||c1==LESS||c1==MOREEQUAL||c1==LESSEQUAL){
        switch (c2){
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case AND:
            case OR:
                return '<';
            //case RP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            //case POUND:
                return '>';
            default:
                return '\0';
        }
    }
    else if(c1==EQ||c1==NEQ){
        switch (c2){
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case AND:
            case OR:
                return '<';
            //case RP:
            case EQ:
            case NEQ:
            //case POUND:
                return '>';
            default:
                return '\0';
        }
    }
//    else if(c1==POUND){
//        switch (c2){
//            case PLUS:
//            case MINUS:
//            case TIMES:
//            case DIVIDE:
//            case LP:
//            case MORE:
//            case LESS:
//            case MOREEQUAL:
//            case LESSEQUAL:
//            case RP:
//            case EQ:
//            case NEQ:
//            case ASSIGN:
//            case PLUSPLUS:
//            case MINUSMINUS:
//                return '<';
//            case POUND:
//                return '=';
//            default:
//                return '\0';
//        }
//    }
    else if(c1 == AND||c1 == OR){
        switch (c2){
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            //case LP:
            case ASSIGN:
            //case POUND:
                return '>';
            case LP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
                return '<';
            default:
                return '\0';
        }
    }
    return '\0';  // 不知道会不会出现这种情况
}




