#pragma once
#define LOGIC_H
#include "database.h"

class CourseController{
public:
    bool enrollCourse(int studentId, int taskId) {
        EnrollmentProxy::save(studentId, taskId);
        return true;
    }
    //教师评定成绩
    void gradeStudent(int teacherId, int studentId, int taskId, float grade) {
        // 只有该任务的授课教师才能打分
        EnrollmentProxy::updateGrade(studentId, taskId, grade);
    }

    // 教学秘书安排
    void scheduleTask(int courseId, int teacherId, std::string semester) {
        auto conn = DBConfig::connect();
        pqxx::work tx(*conn);
        tx.exec_params("INSERT INTO teaching_tasks (course_id, teacher_id, semester) VALUES ($1, $2, $3)",
                       courseId, teacherId, semester);
        tx.commit();
    }
};
