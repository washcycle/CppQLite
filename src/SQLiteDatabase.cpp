/*
 * SQLiteDatabase.cpp
 *
 *  Created on: Jun 28, 2015
 *      Author: Matthew Landowski <matthew.landowski@gmail.com>
 */

#include <fstream>
#include "SQLiteDatabase.h"

namespace sqlite {


namespace utility {
// Handy function for checking if file exists
bool fexists(const std::string& filename) {
    std::ifstream ifile(filename.c_str());
    return ifile;
};
}

SQLiteDatabase::SQLiteDatabase() : db_(nullptr), open_(false) { }

void SQLiteDatabase::open(const std::string& filename, const int flags) {

    // If trying to open a read only database connection make sure it is created and if not create it
    if(!utility::fexists(filename) && flags == SQLITE_OPEN_READONLY){
        auto rc = sqlite3_open(filename.c_str(), &db_);

        if (rc) {
            std::string errorMsg = "Can't open database: " + std::string(sqlite3_errstr(rc));
            sqlite3_close(db_);
            throw SQLiteDatabaseException(errorMsg);
        }

    }

    auto rc = sqlite3_open_v2(filename.c_str(), &db_, flags, nullptr);

    if (rc) {
        std::string errorMsg = "Can't open database: " + std::string(sqlite3_errstr(rc));
        sqlite3_close(db_);
        throw SQLiteDatabaseException(errorMsg);
    }

    open_ = true;
}

void SQLiteDatabase::close() {
    auto rc = sqlite3_close(db_);

    if (rc) {
        std::string errorMsg = "Can't close database: " + std::string(sqlite3_errstr(rc));
        throw SQLiteDatabaseException(errorMsg);
    }

    db_ = nullptr;
}

int SQLiteDatabase::getVersion() {
    sqlite3_stmt *pStmt;

    const std::string sql = "PRAGMA user_version;";

    auto rc = sqlite3_prepare_v2(db_, sql.c_str(), sql.size() + 1, &pStmt, nullptr);

    if (rc == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW) {
            int version = sqlite3_column_int(pStmt, 0);
            sqlite3_finalize(pStmt);
            return version;
        }
        else {
            throw SQLiteDatabaseException("Failed to query database version" + getSQLite3ErrorMessage());
        }
    }
    else {
        throw SQLiteDatabaseException("Failed to prepare statement" + getSQLite3ErrorMessage());
    }
}

void SQLiteDatabase::setVersion(const int version) {
    char *zErrMsg;
    std::string sql = "PRAGMA user_version = " + std::to_string(version);

    auto rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &zErrMsg);

    // If the sql executed return else throw exception
    if (rc == SQLITE_OK) {
        return;
    }
    else {
        throw SQLiteDatabaseException("Failed to get database version" + std::string(zErrMsg));
    }
}

void SQLiteDatabase::execQuery(const std::string& sql){
    char *zErrMsg;

    auto rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &zErrMsg);

    // If the sql executed return else throw exception
    if (rc == SQLITE_OK) {
        return;
    }
    else {
        throw SQLiteDatabaseException("Failed to execute query" + std::string(zErrMsg));
    }
}

Cursor SQLiteDatabase::query(const std::string& sql) {

    Cursor c;

    sqlite3_stmt *pStmt;

    auto rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &pStmt, nullptr);

    // If the sql executed return else throw exception
    if (rc != SQLITE_OK) {
        throw SQLiteDatabaseException("Failed to query database" + getSQLite3ErrorMessage());
    }

    // columns in the returned result set
    unsigned int cols = 0;

    // Step through all rows in the result set
    // building the cursor result set
    while (sqlite3_step(pStmt) == SQLITE_ROW) {
        static bool firstRow = true;

        if (firstRow) {
            cols = sqlite3_column_count(pStmt);

            std::vector<std::string> columnNames;
            for (auto col = 0; col < cols; col++) {
                c.columnNames.push_back(std::string(sqlite3_column_name(pStmt, col)));
                c.columnNamesIndexMap[c.columnNames.back()] = col;
            }

            firstRow = false;
        }

        std::vector<std::string> row;

        for (auto col = 0; col < cols; col++) {
            row.push_back(getStdString(sqlite3_column_text(pStmt, col)));
        }

        c.addRow(row);
    }

    sqlite3_finalize(pStmt);

    return c;
}

std::string SQLiteDatabase::getStdString(const unsigned char *text) {
    if (text == nullptr) {
        return "NULL";
    }

    return std::string(reinterpret_cast<const char *>(text));
}

SQLiteDatabase::~SQLiteDatabase() {

}

void SQLiteDatabase::open(const int flags) {

}

std::string SQLiteDatabase::getSQLite3ErrorMessage() {
    if (db_ == nullptr) {
        throw SQLiteDatabaseException("database cannot be null");
    }

    return std::string(sqlite3_errmsg(db_));
}

void SQLiteDatabase::beginTransaction() {
    execQuery("BEGIN");
}

void SQLiteDatabase::endTransaction() {
    execQuery("COMMIT");
}

void SQLiteDatabase::rollback() {
    execQuery("ROLLBACK");
}

bool SQLiteDatabase::isOpen() {
    return open_;
}
} /* namespace sqlite */