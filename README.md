# CppQLite
SQLite C++ API that provides convince functions and low level access to the SQLite C API.

# List of Classes
* SQLiteDatabase - provides CPP convenience API and wrapper around SQLite C API.
* SQLiteOpenHelper - provides base class for database helper classes.
* Cursor - provides common cursor functionality for query result sets.

```cpp
#include <SQLiteOpenHelper.h>
#include "SQLiteDatabaseHelper.h"

const std::string SQLiteDatabaseHelper::table = "cars";
const int SQLiteDatabaseHelper::version = 1;

SQLiteDatabaseHelper::SQLiteDatabaseHelper() : sqlite::SQLiteOpenHelper(database_name, version){}

void SQLiteDatabaseHelper::onCreate(sqlite::SQLiteDatabase& db) {

    const std::string kCreateTable =
            "CREATE TABLE IF NOT EXISTS " + table_name + " "
            "(mpg text, "
            "weight text)";

    db.execQuery(kCreateTable);
}

void SQLiteDatabaseHelper::onUpgrade(sqlite::SQLiteDatabase& db) {
    db.execQuery("DROP cars");
}

SQLiteDatabaseHelper::~SQLiteDatabaseHelper() {}

struct Car{
    std::string make;
    int mpg;
    int weight;
};

int main(){
    
    SQLiteDatabaseHelper dbHelper;
    
    dhHelper.getWritableDatabase();
    
    dbHelper.insert(Car{"Ford", 27, 2000});
    dbHelper.insert(Car{"Tesla", 0, 3000});
    dbHelper.insert(Car{"Toyota", 40, 2600});
}

```