/* 
 * File:   Cursor.cpp
 * Author: Matthew Landowski <matthew.landowski@gmail.com>
 * 
 * Created on July 3, 2015, 4:52 PM
 */

#include <SQLiteDatabase.h>
#include "Cursor.h"

namespace sqlite {

Cursor::Cursor() : count(0), pos(-1) {
}

Cursor::~Cursor() {
}

bool Cursor::next() {
    if(pos < count - 1){
        pos++;
        return true;
    }
    else{
        return false;
    }
}

int Cursor::getColumnIndex(const std::string& columnName) const{
    return columnNamesIndexMap.at(columnName);
}

std::string Cursor::getString(const int columnIndex) const {
    if(columnIndex < 0 || columnIndex > columnNames.size()){
        throw new SQLiteDatabaseException("Invalid column index");
    }

    rs[pos][columnIndex - 1];
}

int Cursor::getInt(const int columnIndex) const {
    if(columnIndex < 0 || columnIndex > columnNames.size()){
        throw new SQLiteDatabaseException("Invalid column index");
    }

    return stoi(rs[pos][columnIndex - 1]);
}

void Cursor::addRow(const std::vector<std::string>& resultRow){    
    rs.push_back(resultRow);
    count++;
}

void Cursor::reset(){
    // Reset count and position
    pos = -1;
    count = -1;
    
    // Clear Result Set
    rs.clear();
    
    // Clear column name to index mapping
    columnNames.clear();
}

} /* namespace sqlite */

