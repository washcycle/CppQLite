#include "../gtest/src/gtest/include/gtest/gtest.h"
#include "../../include/SQLiteDatabase.h"

#include <iostream>
#include <fstream>
#include <stdio.h>

bool fexists(const std::string& filename) {
  std::ifstream ifile(filename.c_str());
  return ifile;
};

class SQLiteDatabaseTestFixture : public ::testing::Test {
public:
    SQLiteDatabaseTestFixture( ) {
        test_database_filename_ = "test.db";
    }

    void SetUp( ) {
        // code here will execute just before the test ensues
    }

    void TearDown( ) {
        // code here will be called just after the test completes
        // ok to through exceptions from here if need be
    }

    ~SQLiteDatabaseTestFixture( )  {
        remove("test.db");
    }

    // Test Member Variables
    std::string test_database_filename_;
};

TEST_F(SQLiteDatabaseTestFixture, create_database_test) {
    
    sqlite::SQLiteDatabase db;

    try{
        db.open(test_database_filename_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
        db.close();
    }
    catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
    }

    ASSERT_TRUE(fexists(test_database_filename_));
}

TEST_F(SQLiteDatabaseTestFixture, database_version_test) {

    sqlite::SQLiteDatabase db;
    try{
        db.open(test_database_filename_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
    }
    catch (const std::exception & e){
        std::cout << e.what() << std::endl;
    }

    int expected_version = 1;
    db.setVersion(expected_version);
    auto version = db.getVersion();

    db.close();

    EXPECT_EQ(expected_version, version);
}

TEST_F(SQLiteDatabaseTestFixture, get_readonly_database_test) {

    sqlite::SQLiteDatabase db;

    int expected_version = 1;
    int version = 0;

    try{
        db.open(test_database_filename_, SQLITE_OPEN_READONLY);

        EXPECT_ANY_THROW(db.setVersion(expected_version));

        db.close();
    }
    catch (const std::exception & e){
        std::cout << e.what() << std::endl;
    }
}

