//
// Created by Mr.K on 2022-08-28.
//

#ifndef MY_TINY_COMPILER_FORMAT_OPERATION_H
#define MY_TINY_COMPILER_FORMAT_OPERATION_H

#include <vector>
#include <string>



// 读取每个部分
int read_part(std::vector<std::string> &sep_part);

void print_part(std::vector<std::string> sep_part);

// 格式化主函数
void format();



#endif //MY_TINY_COMPILER_FORMAT_OPERATION_H
