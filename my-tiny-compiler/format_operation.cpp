//
// Created by Mr.K on 2022-08-28.
//

#include "format_operation.h"
#include "get_token.h"
#include <iostream>
#include <fstream>

extern FILE* fp;
extern string token_text;
int word_type0;
int tabs;
ofstream fout;

// 存储每个部分的token
std::vector<std::string> sep_part;

int read_part(std::vector<std::string> &sep_part){
    word_type0 = getToken(fp);
    while (word_type0 == ANNO){
        word_type0 = getToken(fp);
    }
    if (word_type0 == -1){  // 读到结尾
        return 0;
    }
    if (word_type0 == INCLUDE || word_type0 == MACRO){  // 根据头文件和宏结束表达
        sep_part.push_back(token_text);
        return 1;
    }
    if (word_type0 == FOR){
        while (word_type0 != RP){
            sep_part.push_back(token_text);
            word_type0 = getToken(fp);
        }
        sep_part.push_back(token_text);
        return read_part(sep_part);
    }
    if (word_type0 == SEMI){  // 根据分号结束一句表达
        sep_part.push_back(token_text);
        return 1;
    }
    if (word_type0 == LB){  // 根据大括号结束一句表达
        sep_part.push_back(token_text);
        //tabs++;
        return 1;
    }
    if (word_type0 == RB){  // 根据右括号结束一句表达
        sep_part.push_back(token_text);
        //tabs--;
        return 1;
    }
    // 普通token
    sep_part.push_back(token_text);
    return read_part(sep_part);
}

void print_part(std::vector<std::string> sep_part){
    if (sep_part[sep_part.size() - 1] == "}") tabs--;
    for (int i = 0; i < tabs; i++){
        std::cout << '\t';
        fout << '\t';
    }
    for (string part : sep_part){
        if (part == "{") tabs++;
        std::cout << part << " ";
        fout << part << " ";
    }
    std::cout << endl;
    fout << endl;
}

void format(){
    fout.open("../out.c");
    while (read_part(sep_part)){
        print_part(sep_part);
        sep_part.clear();
    }
    fout.close();
}
