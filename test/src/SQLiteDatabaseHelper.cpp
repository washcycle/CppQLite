/*
 * File:   SQLiteDatabaseHelper.cpp
 * Author: Matthew Landowski <matthew.landowski@gmail.com>
 *
 * Created on July 12, 2015, 7:40 AM
 */

#include <SQLiteOpenHelper.h>
#include "SQLiteDatabaseHelper.h"

const std::string SQLiteDatabaseHelper::table_name = "cars";
const int SQLiteDatabaseHelper::version = 1;

SQLiteDatabaseHelper::SQLiteDatabaseHelper() : sqlite::SQLiteOpenHelper(table_name, version){}

void SQLiteDatabaseHelper::onCreate(sqlite::SQLiteDatabase& db) {

    const std::string kCreateTable =
            "CREATE TABLE IF NOT EXISTS " + table_name + " "
            "(mpg text, "
            "weight text)";

    db.execQuery(kCreateTable);

    db.execQuery("INSERT INTO cars "
                 "VALUES('34', '2000')");

    db.execQuery("INSERT INTO cars "
                 "VALUES('27', '25000')");

    db.execQuery("INSERT INTO cars "
                 "VALUES('16', '5000')");
}

void SQLiteDatabaseHelper::onUpgrade(sqlite::SQLiteDatabase& db) {
    db.execQuery("DROP cars");
}

SQLiteDatabaseHelper::~SQLiteDatabaseHelper() {}
