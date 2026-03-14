#pragma once
#define DATABASE_H
#include <pqxx/pqxx>
#include <memory>

class DBConfig {
public:
    // 建立数据库连接（请根据实际情况修改连接字符串）
    static std::unique_ptr<pqxx::connection> connect() {
        return std::make_unique<pqxx::connection>("dbname=selection user=postgres password=A");
    }
};

// 选课实体的代管者（代理）
class EnrollmentProxy {
public:
    // 存储选课记录
    static void save(int studentId, int taskId) {
        auto conn = DBConfig::connect();
        pqxx::work tx(*conn);
        tx.exec_params("INSERT INTO enrollments (student_id, task_id) VALUES ($1, $2)", studentId, taskId);
        tx.commit();
    }

    // 教师评定成绩
    static void updateGrade(int studentId, int taskId, float grade) {
        auto conn = DBConfig::connect();
        pqxx::work tx(*conn);
        tx.exec_params("UPDATE enrollments SET grade = $1 WHERE student_id = $2 AND task_id = $3",
                       grade, studentId, taskId);
        tx.commit();
    }
};
