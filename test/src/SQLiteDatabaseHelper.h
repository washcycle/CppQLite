/*
 * File:   SQLiteDatabaseHelper.h
 * Author: Matthew Landowski <matthew.landowski@gmail.com>
 *
 * Created on July 12, 2015, 7:40 AM
 */

#ifndef SQLITECPPWRAPPER_SQLITEDATABASEHELPER_H
#define SQLITECPPWRAPPER_SQLITEDATABASEHELPER_H

#include "SQLiteOpenHelper.h"
#include "SQLiteDatabase.h"
#include <memory>


class SQLiteDatabaseHelper : public sqlite::SQLiteOpenHelper {
public:
    SQLiteDatabaseHelper();
    virtual ~SQLiteDatabaseHelper();

    virtual void onCreate(sqlite::SQLiteDatabase& db);
    virtual void onUpgrade(sqlite::SQLiteDatabase& db);

private:
    const static std::string table_name;
    const static int version;
};


#endif //SQLITECPPWRAPPER_SQLITEDATABASEHELPER_H
