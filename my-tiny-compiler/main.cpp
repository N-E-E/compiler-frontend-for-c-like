#include <iostream>
#include <string>
#include "syntax_analyse.h"
#include "get_token.h"
#include "format_operation.h"
using namespace std;

extern int line_cnt;
extern string token_text;
FILE *fp;

int main() {
    system("chcp 65001");
    char filename[20];
    cout << "enter the filename:" << endl;
    cin >> filename;
    int op;
    while (1){
        fp = fopen(filename,"r");
        if (fp == nullptr){
            cout << "文件打开错误" << endl;
            return 0;
        }
        cout << "-------------------" << endl;
        cout << "  输入序号选择功能" << endl;
        cout << "    1. 词法分析" << endl;
        cout << "    2. 语法分析" << endl;
        cout << "    3. 格式化" << endl;
        cout << "    0. 退出" << endl;
        cout << "-------------------" << endl;
        cin >> op;
        switch (op) {
            case 1:
                int token;
                cout << "类型\t\t" << "符号" << endl;
                while ((token = getToken(fp)) != -1){
                    if (token == ERROR_TOKEN){
                        cout << "在第" << line_cnt << "行出现错误！" << endl;
                        cout << "错误信息为：" << token_text << "over!" << endl;
                        break;
                    }
                    else{
                        cout << token_kind_arr[token] << "\t\t" << token_text << endl;
                    }
                }
                cout << "词法分析结束，按任意键继续" << endl;
                break;
            case 2:
                line_cnt = 1;
                syntax_analyse();
                cout << "语法分析结束，按任意键继续" << endl;
                break;
            case 3:
                //fp = fopen("../test.txt","r");
                format();
                cout << "格式化结束，请按任意键继续" << endl;
                //fclose(fp);
                break;
            case 0:
                //system("clear");
                cout << "感谢使用" << endl;
                return 0;
            default:
                cout << "输入有误，按任意键重新输入" << endl;
        }
        fclose(fp);
    }
    return 0;
}
