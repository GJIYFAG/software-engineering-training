*这个文件是用来讲解如何配置数据库的*
*this is file can tell you how to config database*

1. postgresql
首先，我们需要安装postgresql
❯ sudo pacman -S postgresql   
然后我们需要初始化数据库
initdb ... <略>

2. libpqxx
sudo pacman -S libpqxx
这个会自动安装这个C语言的libpq库
所以就算这个c++的库出现问题，也能使用c语言版本解决问题

3. 数据库使用的  data


CREATE TABLE IF NOT EXISTS users (
    user_id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(50) NOT NULL,
    role VARCHAR(20) CHECK (role IN ('student', 'teacher', 'secretary'))
);

CREATE TABLE IF NOT EXISTS courses (
    course_id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    credits INT NOT NULL
);

CREATE TABLE IF NOT EXISTS teaching_tasks (
    task_id SERIAL PRIMARY KEY,
    course_id INT REFERENCES courses(course_id),
    teacher_id INT REFERENCES users(user_id),
    semester VARCHAR(20) NOT NULL
);

CREATE TABLE IF NOT EXISTS enrollments (
    student_id INT REFERENCES users(user_id),
    task_id INT REFERENCES teaching_tasks(task_id),
    grade FLOAT DEFAULT NULL,
    PRIMARY KEY (student_id, task_id)
);
