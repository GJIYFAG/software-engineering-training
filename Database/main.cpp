#define PQXX_NO_CXX20_MEMORY_RESOURCE 1
#include <iostream>
#include <pqxx/pqxx>

int main() {
    const std::string conn_info =
        "host=localhost "
        "port=5432 "
        "dbname=selection "
        "user=test_user ";
    pqxx::connection conn(conn_info);

    return 0;
}
