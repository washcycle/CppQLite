/* 
 * File:   Cursor.h
 * Author: Matthew Landowski <matthew.landowski@gmail.com>
 *
 * Created on July 3, 2015, 4:52 PM
 */

#ifndef CURSOR_H
#define	CURSOR_H

// STL includes
#include <vector>
#include <string>
#include <map>

#include "CppSQLiteGlobals.h"



namespace sqlite{

typedef std::vector<std::vector<std::string>> ResultSet;

class CPPSQLITE_API Cursor {
    friend class SQLiteDatabase;
public:
    Cursor();
    Cursor(const Cursor& orig);
    virtual ~Cursor();
    
    bool hasNext();
    void reset();
    
    const int getCount() const { return ( count_ ); };
    const std::vector<std::string>& getColumnsNames() const { return columnNames; }
    int getColumnIndex(const std::string& columnName) const;

    unsigned char getBlob(const int columnIndex) const;
    unsigned char getBlob(std::string columnName) const;
    std::string getString(const int columnIndex) const;
    std::string getString(const std::string columnName) const;
    int getInt(const int columnIndex) const;
    int getInt(std::string columnName) const;
    int getDouble(const int columnIndex) const;
    int getDouble(std::string columnName) const;
    long getLong(const int columnIndex) const;
    long getLong(std::string columnName) const;

    // cursor navigation
    bool next();
    
private:
    std::vector<std::string> columnNames;
    std::map<std::string, int> columnNamesIndexMap;
    
    int count_;
    int pos_;
    
    ResultSet rs;
    
    void addRow(const std::vector<std::string>& resultRow);
    void addColumnName(const std::vector<std::string> &columnNames);

};

} /* namespace sqlite */

#endif	/* CURSOR_H */

