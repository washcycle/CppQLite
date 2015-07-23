#include <gtest/gtest.h>
#include "SQLiteDatabaseHelper.h"

TEST(SQLiteDatabaseHelper, read_write_test) {

    SQLiteDatabaseHelper dbHelper;

    // First call to get writeable creates the database
    auto& db = dbHelper.getWriteableDatabase();

    EXPECT_TRUE(db.isOpen());

    db.close();

    EXPECT_FALSE(db.isOpen());

    EXPECT_THROW(db.execQuery("SELECT * from cars"), sqlite::SQLiteDatabaseException);

    db = dbHelper.getReadableDatabase();
    EXPECT_TRUE(db.isOpen());
    db.close();
    //EXPECT_NO_THROW(dbHelper.close());
}

TEST(SQLiteDatabaseHelper, read_only_test) {

    SQLiteDatabaseHelper dbHelper;

    // First call to get writeable creates the database
    auto& db = dbHelper.getWriteableDatabase();
    db.close();

    // Test read only database
    db = dbHelper.getReadableDatabase();
    EXPECT_TRUE(db.isOpen());

    EXPECT_NO_THROW(db.execQuery("SELECT * from cars"));

    db.close();

    EXPECT_FALSE(db.isOpen());
}

