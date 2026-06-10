#include <iostream>
#include <fstream>
#include <string>
int main() {
    // 写入文本文件
    std::ofstream outFile("example.txt");
    if (outFile.is_open()) {
        outFile << "这是一个文本文件示例。\n";
        outFile << "第一行数据: 10 20 30\n";
        outFile << "第二行数据: 40 50 60\n";
        outFile.close();
        std::cout << "文件写入成功!" << std::endl;
    }
    else {
        std::cerr << "无法打开文件!" << std::endl;
        return 1;
    }

    // 读取文本文件
    std::ifstream inFile("example.txt");
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            std::cout << line << std::endl;
        }
        inFile.close();
        std::cout << "文件读取成功!" << std::endl;
    }
    else {
        std::cerr << "无法打开文件!" << std::endl;
        return 1;
    }
    system("pause");
    return 0;
}