// 学生成绩管理系统
// 功能：添加/删除/修改/查询学生记录，统计成绩，支持按索引快速访问。
//时间：2024-06-10
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <limits>
#include <iomanip>

const char* DATA_FILE = "students.dat";
const int COURSES = 3;

// 固定长度的学生记录，便于按偏移直接读/写到二进制文件中。
// 注意：不能在此 struct 中使用 std::string，否则不能直接使用 reinterpret_cast 写入整个结构体。
struct Student {
    char id[20];
	char name[50];// 固定长度字符串，末尾自动添加 '\0' 以确保安全
    int age;
    char gender; 
    float scores[COURSES];
    bool active; // 记录是否有效
};

// 文件头结构，他的作用是记录当前文件中总的记录槽位数（包括被标记删除的槽位），以便于按索引访问和追加新记录时知道写入位置。
//槽位的概念是指每条记录在文件中的位置索引，0 表示第一个槽位，1 表示第二个槽位，以此类推。即使某些槽位被标记为删除（active = false），它们仍然占用一个槽位索引，这样可以保持索引的连续性和稳定性。
struct FileHeader {
    int recordCount;
};

const char* genderToChinese(char g) {
    if (g == 'M') return "男";
    if (g == 'F') return "女";
    return "未知";
}

// 确保数据文件存在；如果不存在则创建并写入初始文件头（recordCount = 0）。
void ensureFile() {
    std::ifstream in(DATA_FILE, std::ios::binary);
    if (!in.is_open()) {
        std::ofstream out(DATA_FILE, std::ios::binary);
        FileHeader h{0};
        out.write(reinterpret_cast<const char*>(&h), sizeof(h));
        out.close();
    } else {
        in.close();
    }
}

// 从文件头读取当前记录槽位数（包含被标记删除的槽位）。
int getRecordCount() {
    std::ifstream in(DATA_FILE, std::ios::binary);
    if (!in.is_open()) return 0;
    FileHeader h;
    in.read(reinterpret_cast<char*>(&h), sizeof(h));
    in.close();
    return h.recordCount;
}

// 更新文件头中的记录槽位数。
void setRecordCount(int n) {
    std::fstream f(DATA_FILE, std::ios::in | std::ios::out | std::ios::binary);
    if (!f.is_open()) return;
    FileHeader h{n};
    f.seekp(0);
    f.write(reinterpret_cast<const char*>(&h), sizeof(h));
    f.close();
}

// 从文件中按索引读取某条学生记录（按记录槽位索引，0 表示第一个槽位）。
// 返回 true 表示读取成功
bool readStudentAt(int index, Student &s) {
    std::ifstream in(DATA_FILE, std::ios::binary);
    if (!in.is_open()) return false;
    FileHeader h;
    in.read(reinterpret_cast<char*>(&h), sizeof(h));
    if (index < 0 || index >= h.recordCount) { in.close(); return false; }
    in.seekg(sizeof(h) + index * sizeof(Student));
    in.read(reinterpret_cast<char*>(&s), sizeof(s));
    in.close();
    return true;
}

// 将学生记录写入指定索引位置。如果 index 等于当前 recordCount，则追加一条新记录并更新文件头。
// 返回 true 表示写入成功。
bool writeStudentAt(int index, const Student &s) {
    std::fstream f(DATA_FILE, std::ios::in | std::ios::out | std::ios::binary);
    if (!f.is_open()) return false;
    FileHeader h;
    f.read(reinterpret_cast<char*>(&h), sizeof(h));
    if (index < 0 || index > h.recordCount) { f.close(); return false; }
    if (index == h.recordCount) {
        // append 到文件末尾并更新文件头
        f.seekp(0, std::ios::end);
        f.write(reinterpret_cast<const char*>(&s), sizeof(s));
        h.recordCount++;
        f.seekp(0);
        f.write(reinterpret_cast<const char*>(&h), sizeof(h));
        f.close();
        return true;
    }
    // 就地覆盖写入已有槽位
    f.seekp(sizeof(h) + index * sizeof(Student));
    f.write(reinterpret_cast<const char*>(&s), sizeof(s));
    f.close();
    return true;
}

// 根据学号查找记录索引，找不到返回 -1。
int findIndexById(const char* id) {
    int n = getRecordCount();
    Student s;
    for (int i = 0; i < n; ++i) {
        if (!readStudentAt(i, s)) continue;
        if (s.active && std::strncmp(s.id, id, sizeof(s.id)) == 0) return i;
    }
    return -1;
}

// 从标准输入读取学生信息到结构体（用于添加或修改）。
void inputStudent(Student &s) {
    std::string tmp;
    std::cout << "学号: "; std::cin >> tmp; strncpy_s(s.id, sizeof(s.id), tmp.c_str(), _TRUNCATE); s.id[sizeof(s.id)-1] = '\0';
    // 读取姓名时需要清空剩余换行
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "姓名: "; std::getline(std::cin, tmp); strncpy_s(s.name, sizeof(s.name), tmp.c_str(), _TRUNCATE); s.name[sizeof(s.name)-1] = '\0';
    std::cout << "年龄: "; std::cin >> s.age;
    // 支持输入中文 "男"/"女"，也兼容 M/F
    {
        std::string gen;
        std::cout << "性别 (男/女): "; std::cin >> gen;
        if (gen == "男" || gen == "m" || gen == "M") s.gender = 'M';
        else if (gen == "女" || gen == "f" || gen == "F") s.gender = 'F';
        else s.gender = 'U';
    }
    for (int i = 0; i < COURSES; ++i) {
        std::cout << "成绩 " << (i+1) << ": "; std::cin >> s.scores[i];
    }
    s.active = true; // 新录入或修改后都标记为有效
}

// 添加学生记录：优先重用被标记删除的槽位，否则追加到文件末尾。
void addStudent() {
    Student s;
    inputStudent(s);
   
    int n = getRecordCount();
    Student tmp;
    for (int i = 0; i < n; ++i) {
        if (!readStudentAt(i, tmp)) continue;
        if (!tmp.active) {
            writeStudentAt(i, s);
            std::cout << "已在索引 " << i << " 处重用空位添加学生。\n";
            return;
        }
    }
 
    writeStudentAt(n, s);
    std::cout << "学生已添加。\n";
}

// 删除学生：将 active 标志设为 false 并就地写回文件。
void deleteStudent() {
    std::string id;
    std::cout << "输入要删除的学号: "; std::cin >> id;
    int idx = findIndexById(id.c_str());
    if (idx < 0) { std::cout << "未找到学号。\n"; return; }
    Student s;
    readStudentAt(idx, s);
    s.active = false;
    writeStudentAt(idx, s);
    std::cout << "已标记删除 (索引 " << idx << ")。\n";
}

// 修改学生信息：按学号查找并覆盖原有记录。
void modifyStudent() {
    std::string id;
    std::cout << "输入要修改的学号: "; std::cin >> id;
    int idx = findIndexById(id.c_str());
    if (idx < 0) { std::cout << "未找到学号。\n"; return; }
    Student s;
    readStudentAt(idx, s);
    std::cout << "当前信息:\n";
    std::cout << "学号: " << s.id << " 姓名: " << s.name << " 年龄: " << s.age << " 性别: " << genderToChinese(s.gender) << "\n";
    for (int i = 0; i < COURSES; ++i) std::cout << "成绩" << (i+1) << ": " << s.scores[i] << " \n";
    std::cout << "输入新信息：\n";
    inputStudent(s);
    writeStudentAt(idx, s);
    std::cout << "已修改。\n";
}

// 按学号查询并打印学生信息。
void queryById() {
    std::string id;
    std::cout << "输入查询学号: "; std::cin >> id;
    int idx = findIndexById(id.c_str());
    if (idx < 0) { std::cout << "未找到学号。\n"; return; }
    Student s;
    readStudentAt(idx, s);
    std::cout << "索引 " << idx << " 的学生信息：\n";
    std::cout << "学号: " << s.id << "\n姓名: " << s.name << "\n年龄: " << s.age << "\n性别: " << genderToChinese(s.gender) << "\n";
    for (int i = 0; i < COURSES; ++i) std::cout << "成绩" << (i+1) << ": " << s.scores[i] << "\n";
}

// 按索引快速访问（直接按文件偏移读取记录）。
void queryByIndex() {
    int idx;
    std::cout << "输入索引 (从0开始): "; std::cin >> idx;
    Student s;
    if (!readStudentAt(idx, s)) { std::cout << "索引无效。\n"; return; }
    if (!s.active) { std::cout << "该记录为空或已删除。\n"; return; }
    std::cout << "学号: " << s.id << " 姓名: " << s.name << " 年龄: " << s.age << " 性别: " << genderToChinese(s.gender) << "\n";
    for (int i = 0; i < COURSES; ++i) std::cout << "成绩" << (i+1) << ": " << s.scores[i] << "\n";
}

// 列出所有槽位（包括已删除的槽位）并打印有效记录。
void listAll() {
    int n = getRecordCount();
    Student s;
    std::cout << "共槽位: " << n << " (其中可能包含已删除记录)\n";
    for (int i = 0; i < n; ++i) {
        if (!readStudentAt(i, s)) continue;
        std::cout << "[" << i << "] ";
        if (!s.active) { std::cout << "<已删除>\n"; continue; }
        std::cout << s.id << " " << s.name << " " << s.age << " " << genderToChinese(s.gender) << " ";
        for (int j = 0; j < COURSES; ++j) std::cout << s.scores[j] << (j+1==COURSES?"":" ");
        std::cout << "\n";
    }
}

// 统计每门课程的平均、最高、最低分（仅统计 active 的记录）。
void statistics() {
    int n = getRecordCount();
    Student s;
    std::vector<float> sum(COURSES, 0.0f);
    std::vector<float> mx(COURSES, std::numeric_limits<float>::lowest());
    std::vector<float> mn(COURSES, std::numeric_limits<float>::max());
    int cnt = 0;
    for (int i = 0; i < n; ++i) {
        if (!readStudentAt(i, s)) continue;
        if (!s.active) continue;
        cnt++;
        for (int j = 0; j < COURSES; ++j) {
            float v = s.scores[j];
            sum[j] += v;
            if (v > mx[j]) mx[j] = v;
            if (v < mn[j]) mn[j] = v;
        }
    }
    if (cnt == 0) { std::cout << "没有学生记录用于统计。\n"; return; }
    std::cout << std::fixed << std::setprecision(2);
    for (int j = 0; j < COURSES; ++j) {
        std::cout << "课程 " << (j+1) << ": 平均=" << (sum[j]/cnt) << " 最高=" << mx[j] << " 最低=" << mn[j] << "\n";
    }
}

int main() {
    // 初始化数据文件并进入交互式菜单
    ensureFile();
    while (true) {
        std::cout << "\n学生成绩管理系统\n";
        std::cout << "1. 添加学生\n2. 删除学生\n3. 修改学生\n4. 按学号查询\n5. 按索引快速访问\n6. 列出所有\n7. 成绩统计\n8. 退出\n";
        std::cout << "选择: ";
        int choice; if (!(std::cin >> choice)) break;
        switch (choice) {
            case 1: addStudent(); break;
            case 2: deleteStudent(); break;
            case 3: modifyStudent(); break;
            case 4: queryById(); break;
            case 5: queryByIndex(); break;
            case 6: listAll(); break;
            case 7: statistics(); break;
            case 8: std::cout << "退出。\n"; return 0;
            default: std::cout << "无效选择。\n"; break;
        }
    }
    return 0;
}
