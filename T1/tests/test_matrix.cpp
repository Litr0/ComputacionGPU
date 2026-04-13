#include <gtest/gtest.h>
#include "../Matrix.h"

TEST(MatrixTest, ConstructorInitialization) {
    Matrix m(3, 2);
    auto [rows, cols] = m.size();
    EXPECT_EQ(rows, 3);
    EXPECT_EQ(cols, 2);
}

TEST(MatrixTest, AdditionSizeMismatchThrowsException) {
    Matrix m1(2, 2);
    Matrix m2(3, 3);
    
    EXPECT_THROW({
        m1 += m2;
    }, std::logic_error);
}