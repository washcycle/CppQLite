/* 
 * File:   SQLiteOpenHelper.h
 * Author: Matthew Landowski <matthew.landowski@gmail.com>
 *
 * Created on July 4, 2015, 4:43 PM
 */

#ifndef SQLITEDATABASEHELPER_H
#define	SQLITEDATABASEHELPER_H

#include <string>
#include <memory>

#include "SQLiteDatabase.h"

namespace sqlite {

class CPPSQLITE_API SQLiteOpenHelper {
public:
    SQLiteOpenHelper(const std::string& database_name, const int version);
    virtual ~SQLiteOpenHelper();

    SQLiteDatabase& getReadableDatabase();
    SQLiteDatabase& getWriteableDatabase();

    virtual void onCreate(SQLiteDatabase& db) = 0;
    virtual void onUpgrade(SQLiteDatabase& db) = 0;
    virtual void onDowngrade(SQLiteDatabase& db) { throw new SQLiteDatabaseException("onDowngrade not implemented"); }

    virtual void close();

    const std::string& database_name() const { return database_name_; }

private:

    SQLiteDatabase db_;
    std::string database_name_;
    std::string filename_;
    int version_;
    bool read_only_;

    SQLiteDatabase& getDatabase(const std::string& filename, int flags);
};

} /* namespace sqlite */

#endif	/* SQLITEDATABASEHELPER_H */

