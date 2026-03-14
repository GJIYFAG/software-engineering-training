cmake_minimum_required(VERSION 3.16)

project(Test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Test main.cpp
    domain.h
    database.h
    logic.h)

include(GNUInstallDirs)
install(TARGETS Test
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


# # 自动安装的 libpqxx 头文件默认在 /usr/include，直接指定即可
target_include_directories(Test PRIVATE /usr/include)

# 直接链接 pqxx（
target_link_libraries(Test
   PRIVATE
   pqxx pq  # pq 是 libpqxx 依赖的 PostgreSQL C 库
)
