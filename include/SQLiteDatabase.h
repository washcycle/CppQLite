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
#include <sqlite3.h>

// Project includes
#include "CppSQLiteGlobals.h"
#include "Cursor.h"

namespace sqlite {

/** SQLiteDatabaseException custom exception thrown by SQLiteDabase functions.
 *
 */
class CPPSQLITE_API SQLiteDatabaseException : public std::exception
{
public:
    SQLiteDatabaseException(const std::string& msg) : msg(msg) {}
    ~SQLiteDatabaseException() throw() {}
    const char* what() const throw() { return msg.c_str(); }
private:
    std::string msg;
};

/** SQLiteDatabase manages the connection to a SQLite3 database file. Provides convenience and C++ compatible wrapper
 * functions around the SQLite3 C API.
 *
 */
class CPPSQLITE_API SQLiteDatabase {
public:
    SQLiteDatabase();
    virtual ~SQLiteDatabase();

    /** Opens a connection to the SQLite3 database file. Calls the low level C API to open a connection to the
     * SQLite3 database. The input flags open the database in either read or write mode. The flags can be looked up on
     * the SQLite3 homepage for a complete listing https://sqlite.org/c3ref/open.html.
     *
     *  @filename [in] filename filename included path if required pointing to the SQLite3 database file
     *  @flags [in] flags most common flags are SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE or SQLITE_OPEN_READWRITE
     */
    void open(const std::string& filename, const int flags);

    /** Closes the connection to the SQLite3 database file. */
    void close();

    /** Beings a database transaction. */
    void beginTransaction();
    /** End the database transaction and commits changes to the database. */
    void endTransaction();
    /** Rollback all database changes from the transaction starting point. */
    void rollback();

    /** Gets the database version. This variable is store in the database internal data. */
    int getVersion();
    /** Sets the database version. This variable is store in the database internal data. */
    void setVersion(const int version);

    /** Convenience query function returns non-distinct results.
     *
     * @param table [in] table to query
     * @param columns [in] columns to return
     * @param selection [in] where column restrictions eg. "field1 = ? AND field2 = ?"
     * @param selectionArgs [in] where column binding arguments
     * @param groupBy [in] group by columns
     * @param orderBy [in] order by columns
     * @param limit [in] limit result rows
     *
     * @return Cursor [out] Cursor containing the result set from the query, will be empty if no results are found.
     */
    Cursor query(const std::string& table, const std::vector<std::string>& columns, const std::string& selection,
                 const std::vector<std::string>& selectionArgs, const std::string& groupBy, const std::string& orderBy,
                 const std::string& limit);


    /** Convenience query function
     *
     * @param distinct [in] if true will return distinct results else all results will be returned.
     * @param table [in] table to query
     * @param columns [in] columns to return
     * @param selection [in] where column restrictions eg. "field1 = ? AND field2 = ?"
     * @param selectionArgs [in] where column binding arguments
     * @param groupBy [in] group by columns
     * @param orderBy [in] order by columns
     * @param limit [in] limit result rows
     *
     * @return Cursor [out] Cursor containing the result set from the query, will be empty if no results are found.
     */
    Cursor query(bool distinct, const std::string& table, const std::vector<std::string>& columns,
                 const std::string& selection, const std::vector<std::string>& selectionArgs, const std::string& groupBy,
                 const std::string& orderBy, const std::string& limit);

    /** Query function that executes the input sql and returns a Cursor of the results.
     *
     * @return Cursor [out] Cursor containing the result set from the query, will be empty if not results are found.
     */
    Cursor query(const std::string& sql);

    /** Executes the sql and expects no results to be returned. */
    void execQuery(const std::string& sql);

    /** Executes the sql and expects no results to be returned.
     *
     * @return bool [out] true if the database connection is open
     * */
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
