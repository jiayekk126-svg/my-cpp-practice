#include <iostream>
#include <fstream>
struct Student {
    char name[50];
    int age;
    float score;
};
int main() {
    // 写入二进制文件
    Student s1 = { "张三", 20, 95.5 };
    std::ofstream outFile("student.dat", std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(reinterpret_cast<const char*>(&s1), sizeof(Student));
        outFile.close();
        std::cout << "二进制文件写入成功!" << std::endl;
    }
    else {
        std::cerr << "无法打开文件!" << std::endl;
        return 1;
    }

    // 读取二进制文件
    Student s2;
    std::ifstream inFile("student.dat", std::ios::binary);
    if (inFile.is_open()) {
        inFile.read(reinterpret_cast<char*>(&s2), sizeof(Student));
        inFile.close();

        std::cout << "读取的学生信息:" << std::endl;
        std::cout << "姓名: " << s2.name << std::endl;
        std::cout << "年龄: " << s2.age << std::endl;
        std::cout << "分数: " << s2.score << std::endl;
    }
    else {
        std::cerr << "无法打开文件!" << std::endl;
        return 1;
    }
    system("pause");
    return 0;
}