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
#include <mutex>

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
    SQLiteDatabase();
    virtual ~SQLiteDatabase();

    void open(const std::string& filename, const int flags);
    void close();

    void beginTransaction();
    void endTransaction();
    void rollback();
    
    int getVersion();
    void setVersion(const int version);

    Cursor query(const std::string& table, const std::vector<std::string>& columns, const std::string& selection,
                 const std::vector<std::string>& selectionArgs, const std::string& groupBy, const std::string& orderBy,
                 const std::string& limit);

    Cursor query(bool distinct, const std::string& table, const std::vector<std::string>& columns,
                 const std::string& selection, const std::vector<std::string>& selectionArgs, const std::string& groupBy,
                 const std::string& orderBy, const std::string& limit);

    Cursor query(const std::string& sql);

    void execQuery(const std::string& sql);

    bool isOpen();

protected:

private:
    sqlite3* db_;
    bool open_;

    std::string getStdString(const unsigned char* text);
    std::string getSQLite3ErrorMessage();
};

} /* namespace sqlite */

#endif /* SQLITEDATABASE_H_ */
