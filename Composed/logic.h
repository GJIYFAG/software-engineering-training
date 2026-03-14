#pragma once
#define LOGIC_H
#include "database.h"

class CourseController {
public:
    // 业务逻辑：学生申请修课
    bool enrollCourse(int studentId, int taskId) {
        // 此处可添加业务规则检查，如：该生是否已选过此课
        EnrollmentProxy::save(studentId, taskId);
        return true;
    }

    // 业务逻辑：教师评定成绩
    void gradeStudent(int teacherId, int studentId, int taskId, float grade) {
        // 只有该任务的授课教师才能打分
        EnrollmentProxy::updateGrade(studentId, taskId, grade);
    }

    // 业务逻辑：教学秘书安排任务
    void scheduleTask(int courseId, int teacherId, std::string semester) {
        auto conn = DBConfig::connect();
        pqxx::work tx(*conn);
        tx.exec_params("INSERT INTO teaching_tasks (course_id, teacher_id, semester) VALUES ($1, $2, $3)",
                       courseId, teacherId, semester);
        tx.commit();
    }
};
