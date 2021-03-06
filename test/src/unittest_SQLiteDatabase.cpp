#include <gtest/gtest.h>
#include "../../include/SQLiteDatabase.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <thread> // for multithreaded unit test

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

    try{
        db.open(test_database_filename_, SQLITE_OPEN_READONLY);

        int expected_version = 2;

        EXPECT_ANY_THROW(db.setVersion(expected_version));

        db.close();
    }
    catch (const std::exception & e){
        std::cout << e.what() << std::endl;
    }
}

TEST_F(SQLiteDatabaseTestFixture, query_test) {

    sqlite::SQLiteDatabase db;

    try{
        db.open(test_database_filename_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);


        // Create Cars table test data
        const std::string kCreateTable =
                "CREATE TABLE IF NOT EXISTS cars "
                        "(mpg text, "
                        "weight text)";

        db.execQuery(kCreateTable);

        db.execQuery("INSERT INTO cars "
                             "VALUES('34', '2000')");

        db.execQuery("INSERT INTO cars "
                             "VALUES('27', '25000')");

        db.execQuery("INSERT INTO cars "
                             "VALUES('16', '5000')");

        std::vector<std::string> columns;
        columns.push_back("mpg");
        columns.push_back("weight");

        std::string selection;

        std::vector<std::string> selectionArgs;

        auto c = db.query(true, "cars", columns, selection, selectionArgs, "", "", "");

        c.next();
        EXPECT_STREQ(c.getString(1).c_str(), "34");
        EXPECT_STREQ(c.getString(2).c_str(), "2000");
        c.next();
        EXPECT_STREQ(c.getString(1).c_str(), "27");
        EXPECT_STREQ(c.getString(2).c_str(), "25000");
        c.next();
        EXPECT_STREQ(c.getString(1).c_str(), "16");
        EXPECT_STREQ(c.getString(2).c_str(), "5000");

        db.close();
    }
    catch (const std::exception & e){
        std::cout << e.what() << std::endl;
    }
}

TEST_F(SQLiteDatabaseTestFixture, insert_test) {

    sqlite::SQLiteDatabase db;

    try{
        db.open(test_database_filename_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

        // Create Cars table test data
        const std::string kCreateTable =
                "CREATE TABLE IF NOT EXISTS cars "
                        "(mpg text, "
                        "weight text)";

        db.execQuery(kCreateTable);

        const std::string table = "cars";
        long id = db.insert(table, std::vector<std::string>{"mpg", "weight"}, std::vector<std::string>{"34", "2000"}, "", std::vector<std::string>{});
        long id2 = db.insert(table, std::vector<std::string>{"mpg", "weight"}, std::vector<std::string>{"20", "1500"}, "", std::vector<std::string>{});

        EXPECT_EQ(id, 1);
        EXPECT_EQ(id2, 2);

        db.close();
    }
        catch (const std::exception & e){
        std::cout << e.what() << std::endl;
    }
}

TEST_F(SQLiteDatabaseTestFixture, update_test) {

    sqlite::SQLiteDatabase db;

    try{
        db.open(test_database_filename_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

        // Create Cars table test data
        const std::string kCreateTable =
            "CREATE TABLE IF NOT EXISTS cars "
                "(mpg text, "
                "weight text)";

        db.execQuery(kCreateTable);

        const std::string table = "cars";
        long id = db.insert(table, std::vector<std::string>{"mpg", "weight"}, std::vector<std::string>{"34", "2000"}, "", std::vector<std::string>{});
        long id2 = db.insert(table, std::vector<std::string>{"mpg", "weight"}, std::vector<std::string>{"20", "1500"}, "", std::vector<std::string>{});

        int updated_rows = db.update(table, std::vector<std::string>{"mpg", "weight"}, std::vector<std::string>{"1", "1"}, "", std::vector<std::string>{});

        EXPECT_EQ(updated_rows, 2);

        db.insert(table, std::vector<std::string>{"mpg", "weight"}, std::vector<std::string>{"20", "1500"}, "", std::vector<std::string>{});
        updated_rows = db.update(table, std::vector<std::string>{"mpg", "weight"},
                                     std::vector<std::string>{"2", "2"}, "mpg = ? AND weight = ?",
                                     std::vector<std::string>{"20", "1500"});

        EXPECT_EQ(updated_rows, 1);

        db.close();
    }
    catch (const std::exception & e){
        std::cout << e.what() << std::endl;
    }
}

TEST_F(SQLiteDatabaseTestFixture, remove_test) {

    sqlite::SQLiteDatabase db;

    try{
        db.open(test_database_filename_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

        // Create Cars table test data
        const std::string kCreateTable =
            "CREATE TABLE IF NOT EXISTS cars "
                "(mpg text, "
                "weight text)";

        db.execQuery(kCreateTable);

        const std::string table = "cars";
        db.insert(table, std::vector<std::string>{"mpg", "weight"}, std::vector<std::string>{"34", "2000"}, "", std::vector<std::string>{});
        db.insert(table, std::vector<std::string>{"mpg", "weight"}, std::vector<std::string>{"20", "1500"}, "", std::vector<std::string>{});

        int deleted_rows = db.remove(table, "mpg = ? AND weight = ?", std::vector<std::string>{"34", "2000"});

        EXPECT_EQ(deleted_rows, 1);

        db.close();
    }
    catch (const std::exception & e){
        std::cout << e.what() << std::endl;
    }
}

// Helper function for multi_threaded_insert_test
void call_from_thread(sqlite::SQLiteDatabase& db, std::string table) {
    db.insert(table, std::vector<std::string>{"mpg", "weight"}, std::vector<std::string>{"34", "2000"}, "", std::vector<std::string>{});
}

TEST_F(SQLiteDatabaseTestFixture, multi_threaded_insert_test) {

    sqlite::SQLiteDatabase db;

    try{
        db.open(test_database_filename_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

        // Create Cars table test data
        const std::string kCreateTable =
            "CREATE TABLE IF NOT EXISTS cars "
                "(mpg text, "
                "weight text)";

        db.execQuery(kCreateTable);

        const std::string table = "cars";

        std::thread t0(call_from_thread, std::ref(db), table);
        std::thread t1(call_from_thread, std::ref(db), table);
        std::thread t2(call_from_thread, std::ref(db), table);
        std::thread t3(call_from_thread, std::ref(db), table);

        t0.join();
        t1.join();
        t2.join();
        t3.join();

        sqlite::Cursor c = db.query("select * from cars;");

        int count = c.getCount();
        EXPECT_EQ(count, 4);

        db.close();
    }
    catch (const std::exception & e){
        std::cout << e.what() << std::endl;
    }
}