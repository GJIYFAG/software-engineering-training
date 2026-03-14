# 设计文档
1. 设计原则

    贴合领域层实体：库表结构与Student/Teacher/Course等实体属性严格对应，降低 Repository 层映射成本；
    满足业务逻辑：支撑选课学分限制、课程人数限制、教师成绩录入权限等核心规则；
    遵循数据库规范：合理设置主键、外键、约束，避免数据冗余与脏数据；
    适配代管者模式：每个表对应一个 Repository 类，单表操作独立，多表关联通过业务层控制（非数据库级联）。

2. 命名规范

    数据库名：course_selection_system（英文小写，下划线分隔）；
    表名：小写 + 下划线，如students/teaching_tasks（实体名复数形式）；
    字段名：小写 + 下划线，如student_id/total_credits（与领域层成员变量m_studentId/m_totalCredits语义一致）；
    主键 / 外键：主键为表名_id，外键直接引用关联表主键，如course_id引用courses.course_id。

3. 数据类型约定
表格
业务字段类型	PostgreSQL 数据类型	适用场景
唯一标识	VARCHAR(20)	学号、教师号、课程号（固定长度，便于索引）
名称 / 描述	VARCHAR(50)	姓名、课程名、院系、教室、上课时间
数值 / 计数	INT	年级、学分、人数、成绩、总学分（非负，加范围约束）
时间戳	TIMESTAMP DEFAULT CURRENT_TIMESTAMP	可选，如记录选课时间、成绩录入时间（实训基础版可暂不添加）
二、数据库整体结构
1. 数据库创建语句
sql

-- 创建数据库（编码UTF-8，支持中文）
CREATE DATABASE course_selection_system
WITH 
OWNER = postgres  -- 数据库管理员，可根据本地配置修改
ENCODING = 'UTF8'
LC_COLLATE = 'zh_CN.UTF-8'
LC_CTYPE = 'zh_CN.UTF-8'
TABLESPACE = pg_default
CONNECTION LIMIT = -1;

-- 切换到目标数据库
\c course_selection_system;

2. 表结构关系

    核心主表：students（学生）、teachers（教师）、courses（课程）；
    关联表：student_courses（学生选课关联，多对多）；
    业务表：scores（成绩，依赖学生选课关联）、teaching_tasks（教学任务，关联课程与教师）；
    外键关联：所有关联表 / 业务表均通过主键关联主表，取消级联操作（由业务层控制增删改逻辑，符合代管者模式）。

三、详细表结构设计
1. 学生表（students）
对应领域层：Student类作用：存储学生基础信息，记录已选总学分（核心业务约束：总学分≤30）Repository 对应：StudentRepository
sql

CREATE TABLE students (
    student_id VARCHAR(20) PRIMARY KEY,  -- 主键：学号，如S001/S2024001
    name VARCHAR(50) NOT NULL,           -- 学生姓名，非空
    grade INT NOT NULL CHECK (grade > 2000),  -- 年级，如2024，加范围约束
    total_credits INT NOT NULL DEFAULT 0 CHECK (total_credits >= 0 AND total_credits <= 30)  -- 已选总学分，非负且≤30
);
-- 索引：学号为主键，自动创建索引，提升查询效率

字段说明：

    total_credits：由业务层更新（学生选课 / 退课时），数据库通过CHECK约束保证不超限，与Student类m_totalCredits一致；
    grade：仅做基础存储，无业务约束，贴合实体属性。

2. 教师表（teachers）
对应领域层：Teacher类作用：存储教师基础信息，关联所教课程Repository 对应：TeacherRepository
sql

CREATE TABLE teachers (
    teacher_id VARCHAR(20) PRIMARY KEY,  -- 主键：教师号，如T001/TJSJ001
    name VARCHAR(50) NOT NULL,           -- 教师姓名，非空
    department VARCHAR(50) NOT NULL      -- 所属院系，如计算机学院、文学院
);

字段说明：

    department：仅做基础属性存储，无业务约束，与Teacher类m_department一致；
    教师所教课程通过courses.teacher_id关联，不单独存储，避免数据冗余。

3. 课程表（courses）
对应领域层：Course类作用：存储课程基础信息，记录选课人数（核心业务约束：当前人数≤最大人数）Repository 对应：CourseRepository
sql

CREATE TABLE courses (
    course_id VARCHAR(20) PRIMARY KEY,   -- 主键：课程号，如C001/CS101
    course_name VARCHAR(50) NOT NULL,    -- 课程名，非空，如C++程序设计、数据结构
    credit INT NOT NULL CHECK (credit >= 1 AND credit <= 5),  -- 学分，1-5分（高校常规学分范围）
    max_students INT NOT NULL CHECK (max_students >= 10),     -- 最大选课人数，≥10（实训基础版约束）
    current_students INT NOT NULL DEFAULT 0 CHECK (current_students >= 0),  -- 当前选课人数，非负
    teacher_id VARCHAR(20) REFERENCES teachers(teacher_id) ON DELETE SET NULL  -- 外键：授课教师号，关联教师表
);
-- 索引：按教师号索引，提升教师查询所带课程效率
CREATE INDEX idx_courses_teacher_id ON courses(teacher_id);

字段说明：

    current_students：由业务层更新（学生选课 / 退课时），数据库通过CHECK约束保证非负，与Course类m_currentStudents一致；
    teacher_id：外键关联教师表，删除教师时置空（避免因教师删除导致课程数据丢失，由教学秘书后续重新分配）；
    max_students：教学秘书添加课程时设置，与Course类m_maxStudents一致。

4. 学生选课关联表（student_courses）
对应领域层：Student类m_selectedCourses、Course类选课关联作用：实现学生与课程的多对多关联，是成绩表、选课业务的基础Repository 对应：由StudentRepository和CourseRepository共同操作
sql

CREATE TABLE student_courses (
    student_id VARCHAR(20) REFERENCES students(student_id) ON DELETE CASCADE,  -- 外键：学号，关联学生表
    course_id VARCHAR(20) REFERENCES courses(course_id) ON DELETE CASCADE,    -- 外键：课程号，关联课程表
    PRIMARY KEY (student_id, course_id)  -- 复合主键：一个学生对一个课程只能选一次
);
-- 索引：双字段索引，提升选课/退课/查询已选课程效率
CREATE INDEX idx_student_courses_sid ON student_courses(student_id);
CREATE INDEX idx_student_courses_cid ON student_courses(course_id);

字段说明：

    复合主键：保证学生不会重复选同一门课，数据库层面做基础约束，业务层再做二次校验；
    级联删除：删除学生 / 课程时，自动删除对应的选课记录，避免脏数据（唯一开启的级联操作，贴合业务逻辑）；
    无额外字段：仅做关联，所有业务属性（如选课时间）实训基础版暂不添加，如需扩展可后续增加。

5. 成绩表（scores）
对应领域层：Score类作用：存储学生课程成绩，支持平时成绩、期末成绩、总评成绩（计算规则：平时 ×30%+ 期末 ×70%）Repository 对应：ScoreRepository
sql

CREATE TABLE scores (
    student_id VARCHAR(20),  -- 外键：学号
    course_id VARCHAR(20),   -- 外键：课程号
    usual_score INT NOT NULL DEFAULT 0 CHECK (usual_score >= 0 AND usual_score <= 100),  -- 平时成绩，0-100
    final_score INT NOT NULL DEFAULT 0 CHECK (final_score >= 0 AND final_score <= 100),  -- 期末成绩，0-100
    total_score INT NOT NULL DEFAULT 0 CHECK (total_score >= 0 AND total_score <= 100),  -- 总评成绩，0-100
    PRIMARY KEY (student_id, course_id),  -- 复合主键：一个学生一门课一个成绩
    FOREIGN KEY (student_id, course_id) REFERENCES student_courses(student_id, course_id) ON DELETE CASCADE
);
-- 索引：按学号+课程号索引，提升成绩录入/查询效率
CREATE INDEX idx_scores_sid_cid ON scores(student_id, course_id);

字段说明：

    复合主键 + 外键：关联学生选课关联表，只有选课后才能录入成绩，数据库层面做强制约束，避免无选课记录的成绩；
    成绩范围：所有成绩均通过CHECK约束限制在 0-100，与Score类的成绩校验逻辑一致；
    total_score：由业务层计算后写入（Score类calculateTotalScore方法），数据库仅做存储，不做计算（符合业务层封装逻辑）。

6. 教学任务表（teaching_tasks）
对应领域层：TeachingTask类作用：存储教学秘书安排的教学任务，关联课程与教师，记录上课时间、教室Repository 对应：TeachingTaskRepository
sql

CREATE TABLE teaching_tasks (
    task_id VARCHAR(20) PRIMARY KEY,  -- 主键：教学任务号，如TK001/TK2024C001
    course_id VARCHAR(20) REFERENCES courses(course_id) ON DELETE CASCADE,  -- 外键：课程号，关联课程表
    teacher_id VARCHAR(20) REFERENCES teachers(teacher_id) ON DELETE SET NULL,  -- 外键：教师号，关联教师表
    class_time VARCHAR(50) NOT NULL,  -- 上课时间，如周一1-2节、周三5-6节（字符串存储，实训基础版无需复杂时间类型）
    classroom VARCHAR(50) NOT NULL    -- 教室，如教学楼A301、实验楼B205
);
-- 索引：按课程号+教师号索引，提升教学任务查询效率
CREATE INDEX idx_teaching_tasks_cid ON teaching_tasks(course_id);
CREATE INDEX idx_teaching_tasks_tid ON teaching_tasks(teacher_id);

字段说明：

    task_id：唯一标识教学任务，教学秘书手动分配，如课程号 + 序号；
    class_time/classroom：字符串存储，贴合实训终端交互的输入形式，与TeachingTask类m_time/m_classroom一致；
    外键约束：删除课程时级联删除教学任务，删除教师时置空教师号（由教学秘书后续重新分配）。

四、初始化测试数据
作用：提供基础测试数据，支持系统编译后直接进行终端交互测试（学生选课、教师录成绩、教学秘书排课），无需手动添加基础数据执行时机：所有表创建完成后执行
sql

-- 1. 插入教师数据
INSERT INTO teachers (teacher_id, name, department) 
VALUES 
('T001', '张教授', '计算机学院'),
('T002', '李老师', '数学学院'),
('T003', '王讲师', '计算机学院');

-- 2. 插入学生数据
INSERT INTO students (student_id, name, grade, total_credits) 
VALUES 
('S001', '小明', 2024, 0),
('S002', '小红', 2024, 3),
('S003', '小刚', 2023, 12);

-- 3. 插入课程数据（关联教师）
INSERT INTO courses (course_id, course_name, credit, max_students, current_students, teacher_id) 
VALUES 
('C001', 'C++程序设计', 3, 50, 0, 'T001'),
('C002', '数据结构', 4, 40, 0, 'T003'),
('C003', '高等数学', 5, 60, 0, 'T002');

-- 4. 插入测试选课记录（S002选了C001）
INSERT INTO student_courses (student_id, course_id) 
VALUES ('S002', 'C001');
-- 更新学生总学分和课程当前人数
UPDATE students SET total_credits = 3 WHERE student_id = 'S002';
UPDATE courses SET current_students = 1 WHERE course_id = 'C001';

-- 5. 插入测试成绩记录（S002的C001成绩）
INSERT INTO scores (student_id, course_id, usual_score, final_score, total_score) 
VALUES ('S002', 'C001', 85, 90, 89);

-- 6. 插入测试教学任务
INSERT INTO teaching_tasks (task_id, course_id, teacher_id, class_time, classroom) 
VALUES 
('TK001', 'C001', 'T001', '周一1-2节', '教学楼A301'),
('TK002', 'C002', 'T003', '周三3-4节', '实验楼B205'),
('TK003', 'C003', 'T002', '周二5-6节', '教学楼B402');

五、数据库操作规范（适配代管者模式 + 四层架构）
1. 操作权限

    系统仅通过单用户连接数据库（如postgres），在DBUtil中配置固定连接信息；
    所有数据库操作均由数据管理层 Repository 类执行，领域层、应用逻辑层、表现层不直接执行任何 SQL 语句。

2. 操作原则

    单表操作：每个 Repository 类仅操作对应的数据表，如StudentRepository仅操作students和关联的student_courses；
    事务控制：核心业务（如选课、退课）需开启数据库事务，保证操作原子性（如选课：更新学生总学分→更新课程人数→插入选课记录，一步失败则全部回滚）；
    避免跨表查询：复杂查询（如教师查询所带课程的学生）由业务层通过多个 Repository 类联合查询，数据库层仅做单表查询。

3. C++ 代码中连接配置（DBUtil 类）
cpp

// 适配PostgreSQL连接，修改为本地数据库配置
std::string connStr = "host=localhost dbname=course_selection_system user=postgres password=你的数据库密码 port=5432";

Manjaro/Arch 系 PostgreSQL 初始密码设置：
bash

# 初始化数据库
sudo -i -u postgres initdb -D /var/lib/postgres/data
# 启动服务
sudo systemctl start postgresql
sudo systemctl enable postgresql
# 设置postgres用户密码
sudo -i -u postgres psql
\password postgres  # 输入密码，如123456
\q

六、数据库维护与扩展
1. 基础维护命令（PostgreSQL）
sql

-- 查看所有表
\dt;
-- 查看表结构
\d 表名;  -- 如\d students
-- 查看数据
SELECT * FROM 表名;  -- 如SELECT * FROM students;
-- 删除表（谨慎使用，实训测试用）
DROP TABLE 表名 CASCADE;
-- 清空表数据（保留表结构）
TRUNCATE TABLE 表名 CASCADE;

2. 实训扩展方向（可选，提升作品分数）

    增加选课时间戳：在student_courses中添加select_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP，记录选课时间；
    增加成绩录入时间：在scores中添加input_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP，记录教师录成绩时间；
    增加用户密码：在students/teachers中添加password VARCHAR(50) NOT NULL DEFAULT '123456'，实现终端登录密码校验；
    增加课程类型：在courses中添加course_type VARCHAR(20) NOT NULL DEFAULT '必修课'，区分必修课 / 选修课。

七、与实训要求的一致性说明

    适配代管者模式：每个表对应一个 Repository 类，数据管理层完全封装数据库操作，领域层无任何 SQL 语句，符合实训 “非数据库中心开发” 要求；
    支撑核心业务：库表结构完全覆盖 “学生修课、教学秘书安排教学任务、教师评定课程成绩” 三大核心业务，所有业务约束（学分、人数、成绩范围）均在数据库 + 业务层双重校验；
    适配四层架构：数据管理层通过该数据库实现持久化，与应用逻辑层、领域层、表现层解耦，层间仅通过类方法调用，符合实训架构要求；
    支持版本控制：该数据库设计文档可放入 GitHub 仓库dev/doc目录，与源码一起提交，满足实训 “doc 目录存放文档” 的版本控制要求。

八、配套使用说明

    先在 Manjaro 中安装并配置 PostgreSQL，创建数据库并执行所有表结构语句；
    执行初始化测试数据语句，获得基础测试数据；
    修改 C++ 代码中DBUtil类的数据库连接信息（用户、密码、端口）；
    编译运行系统，即可基于测试数据进行终端交互测试（如学生 S001 选课 C002、教师 T001 录入 C001 成绩）。
