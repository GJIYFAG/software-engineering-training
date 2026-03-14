# 指定 CMake
cmake_minimum_required(VERSION 3.10)

# 项目名称（自定义）
project(untitled)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



# 添加可执行文件
add_executable(untitled main.cpp)


# # 自动安装的 libpqxx 头文件默认在 /usr/include，直接指定即可
target_include_directories(untitled PRIVATE /usr/include)

# 直接链接 pqxx（
target_link_libraries(untitled
   PRIVATE
   pqxx pq  # pq 是 libpqxx 依赖的 PostgreSQL C 库
)
