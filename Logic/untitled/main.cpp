#pragma once
#define LOGIC_H
#include "database.h"

class CourseController{
public:
    bool enrollCourse(int studentId, int taskId) {
        EnrollmentProxy::save(studentId, taskId);
        return true;
    }
    void gradeStudent(int teacherId, int studentId, int taskId, float grade);
    void scheduleTask(int courseId, int teacherId, std::string semester);
};
