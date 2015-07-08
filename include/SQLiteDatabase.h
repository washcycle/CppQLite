/*
 * SQLiteDatabase.h
 *
 *  Created on: Jun 28, 2015
 *      Author: Matthew Landowski <matthew.landowski@gmail.com>
 */

#ifndef SQLITEDATABASE_H_
#define SQLITEDATABASE_H_

// STD includes
#include <string>
#include <exception>

// 3rd Party Includes
#include "sqlite3.h"

// Project includes
#include "CppSQLiteGlobals.h"
#include "Cursor.h"

namespace sqlite {

class CPPSQLITE_API SQLiteDatabaseException : public std::exception
{
public:
    SQLiteDatabaseException(const std::string& msg) : msg(msg) {}
    ~SQLiteDatabaseException() throw() {}
    const char* what() const throw() { return msg.c_str(); }
private:
    std::string msg;
};
    
class CPPSQLITE_API SQLiteDatabase {
public:
    SQLiteDatabase(const std::string& filename);
    virtual ~SQLiteDatabase();

    void close();
    
    const int getVersion();
    void setVersion(int version);
    
    Cursor execQuery(const std::string& sql);    
    
protected:
    sqlite3* db;

private:
    std::string getStdString(const unsigned char* text);
};

} /* namespace sqlite */

#endif /* SQLITEDATABASE_H_ */
