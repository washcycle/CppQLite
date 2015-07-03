#include "gtest/gtest.h"
#include "../../include/SQLiteDatabase.h"

#include <fstream>
bool fexists(const std::string& filename) {
  std::ifstream ifile(filename.c_str());
  return ifile;
};

TEST(SQLiteDatabase, create_database_test) {
    
    std::string filename = "test.db";
    
    sqlite::SQLiteDatabase db(filename);
    
    EXPECT_TRUE(fexists(filename));
}

