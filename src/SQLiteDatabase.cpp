/*
 * SQLiteDatabase.cpp
 *
 *  Created on: Jun 28, 2015
 *      Author: Matthew Landowski <matthew.landowski@gmail.com>
 */

#include "SQLiteDatabase.h"

using namespace sqlite;

SQLiteDatabase::SQLiteDatabase(const std::string& filename) {
    auto rc = sqlite3_open_v2(filename.c_str(), 
                              &db, 
                              SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                              nullptr);

    if (rc) {
        std::string errorMsg("Can't open database: %s\n", sqlite3_errmsg(db));
        throw SQLiteDatabaseException(errorMsg);
    }
}

void SQLiteDatabase::close(){
    auto rc = sqlite3_close(db);
    
    if (rc) {
        std::string errorMsg("Can't close database: %s\n", sqlite3_errmsg(db));
        throw SQLiteDatabaseException(errorMsg);
    }
}

const int SQLiteDatabase::getVersion(){
    auto c = execQuery("PRAGMA user_version;");
    
    if(c.next()){
        return c.getInt(1);
    }
    else{
        return -1;
    }
}

void SQLiteDatabase::setVersion(int version){
    execQuery("PRAGMA user_version = " + std::to_string(version));
}

Cursor SQLiteDatabase::execQuery(const std::string& sql){
    Cursor c;
    
    sqlite3_stmt* pStmt;
    
    auto rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, nullptr);

    // columns in the returned result set
    unsigned int cols = 0;

    // Step through all rows in the result set
    // building the cursor result set
    while(sqlite3_step(pStmt) == SQLITE_ROW){
        static bool firstRow = true;

        if(firstRow){
            cols = sqlite3_column_count(pStmt);

            std::vector<std::string> columnNames;
            for(auto col = 0; col < cols; col++){
                c.columnNames.push_back(std::string(sqlite3_column_name(pStmt, col)));
                c.columnNamesIndexMap[c.columnNames.back()] = col;
            }




            firstRow = false;
        }

        std::vector<std::string> row;

        for(auto col = 0; col < cols; col++){
            row.push_back(getStdString(sqlite3_column_text(pStmt, col)));
        }

        c.addRow(row);
    }

    sqlite3_finalize(pStmt);

    return c;
}

std::string SQLiteDatabase::getStdString(const unsigned char* text) {
    if(text == nullptr){
        return "NULL";
    }

    return std::string(reinterpret_cast<const char*>(text));
}

SQLiteDatabase::~SQLiteDatabase() {

}
