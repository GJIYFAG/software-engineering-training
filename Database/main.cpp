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
