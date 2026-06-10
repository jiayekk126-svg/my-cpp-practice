#include <iostream>
#include <iomanip> // 用于流操纵符
#include <string> 

int main() {
    // 基本输出
    std::cout << "Hello, World!" << std::endl;

    // 格式化输出
    double pi = 3.1415926535;
    std::cout << "默认精度: " << pi << std::endl;
    std::cout << "设置精度为4: " << std::setprecision(4) << pi << std::endl;

    // 宽度和填充
    std::cout << std::setw(10) << std::setfill('*') << "Hello" << std::endl;

    // 基本输入
    int age;
    std::cout << "请输入您的年龄: ";
    std::cin >> age;
    std::cout << "您的年龄是: " << age << std::endl;

    // 读取包含空格的字符串
    std::cin.ignore(); // 忽略缓冲区中的换行符
    std::string name;
    std::cout << "请输入您的姓名: ";
    std::getline(std::cin, name);
    std::cout << "您好, " << name << "!" << std::endl;
    system("pause");
    return 0;
}