#include <iostream>
#include <string>
#include "logic.h"

class TerminalUI {
private:
    CourseController controller;
public:
    void run() {
        while (true) { // 增加循环，方便连续操作
            std::cout << "\n--- 重庆师范大学选课系统 ---" << std::endl;
            std::cout << "1. 教学秘书排课\n2. 学生选课\n3. 教师打分\n0. 退出系统\n请选择: ";
            int choice;
            std::cin >> choice;

            if (choice == 0) break;

            if (choice == 1) {
                int courseId, teacherId;
                std::string semester;
                std::cout << "请输入课程ID: "; std::cin >> courseId;
                std::cout << "请输入教师ID: "; std::cin >> teacherId;
                std::cout << "请输入学期(如2025-Fall): "; std::cin >> semester;

                controller.scheduleTask(courseId, teacherId, semester);
                std::cout << "排课成功！" << std::endl;

            } else if (choice == 2) {
                int studentId, taskId;
                std::cout << "请输入您的学生ID: "; std::cin >> studentId;
                std::cout << "请输入教学任务ID(TaskID): "; std::cin >> taskId;

                controller.enrollCourse(studentId, taskId);
                std::cout << "选课申请已提交！" << std::endl;

            } else if (choice == 3) {
                int teacherId, studentId, taskId;
                float grade;
                std::cout << "请输入教师ID: "; std::cin >> teacherId;
                std::cout << "请输入学生ID: "; std::cin >> studentId;
                std::cout << "请输入教学任务ID: "; std::cin >> taskId;
                std::cout << "请输入成绩: "; std::cin >> grade;

                controller.gradeStudent(teacherId, studentId, taskId, grade);
                std::cout << "成绩评定完成！" << std::endl;
            } else {
                std::cout << "无效选择，请重新输入。" << std::endl;
            }
        }
    }
};

int main() {
    try {
        TerminalUI ui;
        ui.run();
    } catch (const std::exception &e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }
    return 0;
}
