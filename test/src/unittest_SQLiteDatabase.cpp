#include "../gtest/src/gtest/include/gtest/gtest.h"
#include "../../include/SQLiteDatabase.h"

#include <fstream>
bool fexists(const std::string& filename) {
  std::ifstream ifile(filename.c_str());
  return ifile;
};

TEST(SQLiteDatabase, create_database_test) {
    
    std::string filename = "test.db";
    
    sqlite::SQLiteDatabase db(filename);
    
    //ASSERT_TRUE(fexists(filename));
    ASSERT_TRUE(false);
}

TEST(SQLiteDatabase, database_version_test) {

    std::string filename = "test.db";

    sqlite::SQLiteDatabase db(filename);

    int expected_version = 1;

    db.setVersion(expected_version);
    auto version = db.getVersion();

    EXPECT_EQ(expected_version, version);
}

