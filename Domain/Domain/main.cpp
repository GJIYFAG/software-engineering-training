#include <string>
#include <vector>

// 基础用户实体
class User {
public:
    int id;
    std::string username;
    std::string role; // student, teacher, secretary
    User(int i, std::string u, std::string r) : id(i), username(u), role(r) {}
};

// 课程实体
class Course {
public:
    int courseId;
    std::string name;
    int credits;
    Course(int id, std::string n, int c) : courseId(id), name(n), credits(c) {}
};

// 教学任务实体（关联教师与课程）
class TeachingTask {
public:
    int taskId;
    int courseId;
    int teacherId;
    std::string semester;
    TeachingTask(int id, int cid, int tid, std::string sem)
        : taskId(id), courseId(cid), teacherId(tid), semester(sem) {}
};
