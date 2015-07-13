#include "../gtest/src/gtest/include/gtest/gtest.h"
#include "SQLiteDatabaseHelper.h"



TEST(SQLiteDatabaseHelper, database_public_api_test) {

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
