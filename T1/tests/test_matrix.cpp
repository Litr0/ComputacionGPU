#include <gtest/gtest.h>
#include "../Matrix.h"

TEST(MatrixTest, ConstructorInitialization) {
    Matrix m(3, 2);
    auto [rows, cols] = m.size();
    EXPECT_EQ(rows, 3);
    EXPECT_EQ(cols, 2);
}

TEST(MatrixTest, StoresDoubleValues) {
    Matrix m(2, 2);
    m[0, 0] = 1.5;
    m[0, 1] = 2.25;
    m[1, 0] = -3.75;
    m[1, 1] = 4.0;

    EXPECT_DOUBLE_EQ((m[0, 0]), 1.5);
    EXPECT_DOUBLE_EQ((m[0, 1]), 2.25);
    EXPECT_DOUBLE_EQ((m[1, 0]), -3.75);
    EXPECT_DOUBLE_EQ((m[1, 1]), 4.0);
}

TEST(MatrixTest, AdditionSizeMismatchThrowsException) {
    Matrix m1(2, 2);
    Matrix m2(3, 3);
    
    EXPECT_THROW({
        m1 += m2;
    }, std::logic_error);
}

TEST(MatrixTest, OperatorPlusReturnsNewMatrixWithoutMutatingOperands) {
    Matrix a(2, 2);
    Matrix b(2, 2);

    a[0, 0] = 1.0;
    a[0, 1] = 2.0;
    a[1, 0] = 3.0;
    a[1, 1] = 4.0;

    b[0, 0] = 0.5;
    b[0, 1] = 1.5;
    b[1, 0] = -2.0;
    b[1, 1] = 6.0;

    Matrix c = a + b;

    EXPECT_DOUBLE_EQ((c[0, 0]), 1.5);
    EXPECT_DOUBLE_EQ((c[0, 1]), 3.5);
    EXPECT_DOUBLE_EQ((c[1, 0]), 1.0);
    EXPECT_DOUBLE_EQ((c[1, 1]), 10.0);

    EXPECT_DOUBLE_EQ((a[0, 0]), 1.0);
    EXPECT_DOUBLE_EQ((a[1, 1]), 4.0);
    EXPECT_DOUBLE_EQ((b[0, 0]), 0.5);
    EXPECT_DOUBLE_EQ((b[1, 1]), 6.0);
}

TEST(MatrixTest, OperatorMinusReturnsNewMatrixWithoutMutatingOperands) {
    Matrix a(2, 2);
    Matrix b(2, 2);

    a[0, 0] = 5.0;
    a[0, 1] = 4.0;
    a[1, 0] = 3.0;
    a[1, 1] = 2.0;

    b[0, 0] = 1.0;
    b[0, 1] = 1.5;
    b[1, 0] = 0.5;
    b[1, 1] = -2.0;

    Matrix c = a - b;

    EXPECT_DOUBLE_EQ((c[0, 0]), 4.0);
    EXPECT_DOUBLE_EQ((c[0, 1]), 2.5);
    EXPECT_DOUBLE_EQ((c[1, 0]), 2.5);
    EXPECT_DOUBLE_EQ((c[1, 1]), 4.0);

    EXPECT_DOUBLE_EQ((a[0, 0]), 5.0);
    EXPECT_DOUBLE_EQ((a[1, 1]), 2.0);
    EXPECT_DOUBLE_EQ((b[0, 0]), 1.0);
    EXPECT_DOUBLE_EQ((b[1, 1]), -2.0);
}

TEST(MatrixTest, OperatorPlusAndMinusThrowOnSizeMismatch) {
    Matrix a(2, 2);
    Matrix b(3, 3);

    EXPECT_THROW({
        (void)(a + b);
    }, std::logic_error);

    EXPECT_THROW({
        (void)(a - b);
    }, std::logic_error);
}
