# CppQLite
SQLite C++ API that provides convince functions and low level access to the SQLite C API.
[![Build Status](https://travis-ci.org/washcycle/CppQLite.svg?branch=master)](https://travis-ci.org/washcycle/CppQLite)
# List of Classes
* SQLiteDatabase - provides C++ convenience API and wrapper around SQLite C API.
* SQLiteOpenHelper - provides base class for database helper classes.
* Cursor - provides common cursor functionality for query result sets.

# Example Code
```cpp
#include <SQLiteOpenHelper.h>
#include <iostream>

// Example Structure that represents a few motor vehicle properties
struct Car{
    std::string make;
    int mpg;
    int weight;
};

/*
 * Custom SQLiteDatabase helper class that managers user define object database interactions
 * creates, deletes, inserts, updates
 *
 */
class SQLiteDatabaseHelper : public sqlite::SQLiteOpenHelper {
public:
    SQLiteDatabaseHelper();
    virtual ~SQLiteDatabaseHelper() {};

    virtual void onCreate(sqlite::SQLiteDatabase& db);
    virtual void onUpgrade(sqlite::SQLiteDatabase& db);

    void addCar(Car car); // inserts car data into database

private:
    const static std::string table_name;
    const static int version;
};

const std::string SQLiteDatabaseHelper::table_name = "cars";
const int SQLiteDatabaseHelper::version = 1;
const std::string COMMA = ", ";

SQLiteDatabaseHelper::SQLiteDatabaseHelper() : sqlite::SQLiteOpenHelper(table_name, version){}

void SQLiteDatabaseHelper::onCreate(sqlite::SQLiteDatabase& db) {

    const std::string kCreateTable =
            "CREATE TABLE IF NOT EXISTS " + table_name + "( "
                     "make TEXT, "
                     "mpg INTEGER, "
                     "weight INTEGER)";

    db.execQuery(kCreateTable);
}

void SQLiteDatabaseHelper::onUpgrade(sqlite::SQLiteDatabase& db) {
    db.execQuery("DROP TABLE IF EXISTS cars");
}

void SQLiteDatabaseHelper::addCar(Car car) {
    // open a writeable database connection
    auto& db = getWriteableDatabase(); // WARNING: Make sure this returned object is a reference and not a copy

    std::string sql =
            "INSERT INTO " + table_name + " "
            "VALUES(" +
            "'" + car.make + "'" + COMMA
            + std::to_string(car.mpg) + COMMA
            + std::to_string(car.weight) +  ")";

    db.execQuery(sql);

    db.close();
}

int main(){

    // Remove old test database if present
    if(remove("cars.db")){
        perror("Error deleting old database");
    };

    try
    {
        SQLiteDatabaseHelper dbHelper;

        dbHelper.addCar(Car{"Ford", 27, 2000});
        dbHelper.addCar(Car{"Tesla", 0, 3000});
        dbHelper.addCar(Car{"Toyota", 40, 2600});
    }
    catch(const sqlite::SQLiteDatabaseException& e)
    {
        std::cout << e.what(); // Print out exception error
    }
} /* main */
```