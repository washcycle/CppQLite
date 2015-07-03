/*
 * SQLiteDatabase.cpp
 *
 *  Created on: Jun 28, 2015
 *      Author: Matthew Landowski <matthew.landowski@gmail.com>
 */

#include "SQLiteDatabase.h"

namespace sqlite {

    SQLiteDatabase::SQLiteDatabase(const std::string& filename) {
        auto rc = sqlite3_open_v2(filename.c_str(), 
                                  &db, 
                                  SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                                  nullptr);

        if (rc) {
            std::string errorMsg("Can't open database: %s\n", sqlite3_errmsg(db));
            throw SQLiteDatabaseException(errorMsg);
        }
        else {
            
        }
    }

    void SQLiteDatabase::close(){
        sqlite3_close(db);
    }
    
    SQLiteDatabase::~SQLiteDatabase() {
    }

} /* namespace sqlite */
