#include <iostream>
#include <fstream>
#include <string>
int main() {
    // 创建一个测试文件
    std::ofstream outFile("test.txt");
    outFile << "0123456789";
    outFile.close();

    // 使用文件指针读取特定位置的数据
    std::ifstream inFile("test.txt");
    if (inFile.is_open()) {
        // 获取文件指针当前位置
        std::cout << "初始位置: " << inFile.tellg() << std::endl;

        // 移动文件指针到位置5
		inFile.seekg(5);//位置5其实是第6个字符，因为位置是从0开始的
		std::cout << "移动后位置: " << inFile.tellg() << std::endl;//tellg()函数返回当前文件指针的位置

        // 读取一个字符
        char ch;
        inFile.get(ch);
        std::cout << "位置5的字符是: " << ch << std::endl;

        inFile.close();
    }

    // 使用文件指针在文件中间插入数据
    std::fstream file("test.txt", std::ios::in | std::ios::out);
    if (file.is_open()) {
        // 移动到位置3
        file.seekp(3);
        // 写入新数据
        file << "XYZ";

        // 移动到文件开始读取整个文件
		file.seekg(1);//如果不移动文件指针，读取时会从当前位置开始，可能会导致读取不完整或错误
        std::string content;
        file >> content;
        std::cout << "更新后的文件内容: " << content << std::endl;

        file.close();
    }
    system("pause");
    return 0;
}