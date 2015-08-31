/* 
 * File:   Cursor.cpp
 * Author: Matthew Landowski <matthew.landowski@gmail.com>
 * 
 * Created on July 3, 2015, 4:52 PM
 */

#include <SQLiteDatabase.h>
#include "Cursor.h"

namespace sqlite {

Cursor::Cursor() : count_(0), pos_(-1) {
}

Cursor::~Cursor() {
}

bool Cursor::next() {
    if(pos_ < count_){
        pos_++;
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

    return rs[pos_][columnIndex - 1];
}

int Cursor::getInt(const int columnIndex) const {
    if(columnIndex < 0 || columnIndex > columnNames.size()){
        throw new SQLiteDatabaseException("Invalid column index");
    }

    return stoi(rs[pos_][columnIndex - 1]);
}

long Cursor::getLong(const int columnIndex) const {
    if(columnIndex < 0 || columnIndex > columnNames.size()){
        throw new SQLiteDatabaseException("Invalid column index");
    }

    return stol(rs[pos_][columnIndex - 1]);
}

void Cursor::addRow(const std::vector<std::string>& resultRow){    
    rs.push_back(resultRow);
    count_++;
}

void Cursor::reset(){
    // Reset count_ and position
    pos_ = -1;
    count_ = 0;
    
    // Clear Result Set
    rs.clear();
    
    // Clear column name to index mapping
    columnNames.clear();
}

bool Cursor::hasNext() {
    return (pos_++ < count_);
}

} /* namespace sqlite */

