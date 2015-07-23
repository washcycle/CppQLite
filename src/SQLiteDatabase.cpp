/*
 * SQLiteDatabase.cpp
 *
 *  Created on: Jun 28, 2015
 *  Author: Matthew Landowski <matthew.landowski@gmail.com>
 */

#include "SQLiteDatabase.h"

namespace sqlite {


namespace utility {
// Handy function for checking if file exists
inline bool fexists(const std::string& filename){
    if (FILE *file = fopen(filename.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
};

} /* namespace sqlite::utility */

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
    open_ = false;
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
            sqlite3_finalize(pStmt);
            throw SQLiteDatabaseException("Failed to query database version " + getSQLite3ErrorMessage());
        }
    }
    else {
        sqlite3_finalize(pStmt);
        throw SQLiteDatabaseException("Failed to prepare statement " + getSQLite3ErrorMessage());
    }
}

void SQLiteDatabase::setVersion(const int version) {
    std::string sql = "PRAGMA user_version = " + std::to_string(version);
    execQuery(sql);
}

void SQLiteDatabase::execQuery(const std::string& sql){
    char *zErrMsg = nullptr;

    // don't continue if the database is not open
    if(!open_){
        throw SQLiteDatabaseException("Can't execute query database connection not open");
    }

    auto rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &zErrMsg);

    // If the sql executed return else throw exception
    if (rc == SQLITE_OK) {
        sqlite3_free(zErrMsg);
        return;
    }
    else {
        auto msg = "Error executing sql " + std::string(zErrMsg);
        sqlite3_free(zErrMsg);
        throw SQLiteDatabaseException(msg);
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

Cursor SQLiteDatabase::query(bool distinct, const std::string& table, const std::vector<std::string>& columns,
             const std::string& selection, const std::vector<std::string>& selectionArgs, const std::string& groupBy,
             const std::string& orderBy, const std::string& limit) {
    std::string sql = "SELECT ";

    if(distinct){
        sql += "DISTINCT ";
    }

    if(columns.size() == 0){
        sql += "* ";
    }else{
        // add return columns
        for(auto ii = 0; ii < columns.size(); ii++){
            sql += columns[ii] + (ii < columns.size() - 1 ? ", " : " ");
        }
    }

    // Add table
    sql += "FROM " + table;

    // Add where clause if needed
    if(!selection.empty()){
        sql += " WHERE ";

        // add selections
        for(auto ii = 0; ii < selection.size(); ii++){
            sql += selection[ii] + (ii < selection.size() ? ", " : "");
        }
    }

    // Add where clause if needed
    if(!groupBy.empty()){
        sql += " GROUP BY " + groupBy;
    }

    // Add order by clause if needed
    if(!orderBy.empty()){
        sql += " ORDER BY " + orderBy;
    }

    // Add limit clause if needed
    if(!limit.empty()){
        sql += " LIMIT " + limit;
    }

    sqlite3_stmt *stmt;

    auto rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);

    if(rc){
        sqlite3_finalize(stmt);
        throw SQLiteDatabaseException("Error preparing statment" + getSQLite3ErrorMessage());
    }

    Cursor c;
    auto cols = sqlite3_column_count(stmt);
    for (auto col = 0; col < cols; col++) {
        c.columnNames.push_back(std::string(sqlite3_column_name(stmt, col)));
        c.columnNamesIndexMap[c.columnNames.back()] = col;
    }

    // Step through all rows in the result set
    // building the cursor result set
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::vector<std::string> row;

        for (auto col = 0; col < cols; col++) {
            row.push_back(getStdString(sqlite3_column_text(stmt, col)));
        }

        c.addRow(row);
    }

    sqlite3_finalize(stmt);

    return c;
}
} /* namespace sqlite */