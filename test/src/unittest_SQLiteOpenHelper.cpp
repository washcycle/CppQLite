#include "../gtest/src/gtest/include/gtest/gtest.h"
#include "SQLiteDatabaseHelper.h"

TEST(SQLiteDatabaseHelper, database_public_api_test) {

    SQLiteDatabaseHelper dbHelper;

    auto db = dbHelper.getWriteableDatabase();

    EXPECT_TRUE(db.isOpen());

    db.close();

    EXPECT_THROW(db.execQuery("SELECT * from cars"), sqlite::SQLiteDatabaseException);

    //EXPECT_NO_THROW(dbHelper.close());
}
