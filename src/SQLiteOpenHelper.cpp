/* 
 * File:   SQLiteDatabaseHelper.cpp
 * Author: Matthew Landowski <matthew.landowski@gmail.com>
 * 
 * Created on July 4, 2015, 4:43 PM
 */

#include "SQLiteOpenHelper.h"

namespace sqlite {

SQLiteOpenHelper::SQLiteOpenHelper(const std::string &database_name, const int version)
        : database_name_(database_name),
          filename_(database_name + ".db"),
          version_(version),
          read_only_(false) {
    if(version_ <= 0){
        throw new SQLiteDatabaseException("Database version must be an integer greater than 0");
    }
}

SQLiteOpenHelper::~SQLiteOpenHelper() {
}

SQLiteDatabase& SQLiteOpenHelper::getReadableDatabase() {
    return getDatabase(filename_, SQLITE_OPEN_READONLY);
}

SQLiteDatabase& SQLiteOpenHelper::getWriteableDatabase() {
    return getDatabase(filename_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
}

SQLiteDatabase& SQLiteOpenHelper::getDatabase(const std::string& filename, const int flags){

    // Lock other threads from trying to open the database at the same time
    std::lock_guard<std::mutex> lock(db_mutex);

    if(db_.isOpen()){
        // database already open
        return db_;
    }

    // Open the database
    db_.open(filename, flags);

    // Create, upgrade, or downgrade if needed
    if(db_.getVersion() == 0){
        onCreate(db_);
        db_.setVersion(version_);
    }
    else if(db_.getVersion() < version_){
        onDowngrade(db_);
        db_.setVersion(version_);
    }
    else if(db_.getVersion() > version_){
        onUpgrade(db_);
        db_.setVersion(version_);
    }

    if(flags == SQLITE_OPEN_READONLY){
        read_only_ = true;
    }

    return db_;
}

void SQLiteOpenHelper::close() {
    std::lock_guard<std::mutex> lock(db_mutex);

    if (db_.isOpen()) {
        db_.close();
    }

    read_only_ = false;
}

} /* namespace sqlite */



