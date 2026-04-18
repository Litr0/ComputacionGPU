#include <gtest/gtest.h>
#include "../Matrix.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace {
std::string make_temp_path(const std::string &name) {
    return (std::filesystem::temp_directory_path() / name).string();
}
}  // namespace

TEST(MatrixTest, ConstructorInitialization) {
    Matrix m(3, 2);
    auto [rows, cols] = m.size();
    EXPECT_EQ(rows, 3);
    EXPECT_EQ(cols, 2);
}

TEST(MatrixTest, DefaultConstructorCreatesEmptyMatrix) {
    Matrix m;
    auto [rows, cols] = m.size();
    EXPECT_EQ(rows, 0);
    EXPECT_EQ(cols, 0);
}

TEST(MatrixTest, SingleDimensionConstructorCreatesColumnVector) {
    Matrix m(4);
    auto [rows, cols] = m.size();
    EXPECT_EQ(rows, 4);
    EXPECT_EQ(cols, 1);
    EXPECT_DOUBLE_EQ((m[0, 0]), 0.0);
    EXPECT_DOUBLE_EQ((m[3, 0]), 0.0);
}

TEST(MatrixTest, ConstructorsRejectNegativeDimensions) {
    EXPECT_THROW({
        Matrix(-1);
    }, std::invalid_argument);

    EXPECT_THROW({
        Matrix(2, -3);
    }, std::invalid_argument);
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

TEST(MatrixTest, FillAssignsAllValues) {
    Matrix m(2, 3);
    m.fill(7.25);

    EXPECT_DOUBLE_EQ((m[0, 0]), 7.25);
    EXPECT_DOUBLE_EQ((m[0, 2]), 7.25);
    EXPECT_DOUBLE_EQ((m[1, 1]), 7.25);
}

TEST(MatrixTest, OperatorIndexThrowsOutOfRange) {
    Matrix m(2, 2);
    EXPECT_THROW({
        (void)m.operator[](2, 0);
    }, std::out_of_range);

    EXPECT_THROW({
        (void)m.operator[](0, 2);
    }, std::out_of_range);
}

TEST(MatrixTest, AdditionSizeMismatchThrowsException) {
    Matrix m1(2, 2);
    Matrix m2(3, 3);
    
    EXPECT_THROW({
        m1 += m2;
    }, std::logic_error);
}

TEST(MatrixTest, SubtractionSizeMismatchThrowsException) {
    Matrix m1(2, 2);
    Matrix m2(1, 3);

    EXPECT_THROW({
        m1 -= m2;
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

TEST(MatrixTest, ScalarMultiplicationUpdatesAllValues) {
    Matrix m(2, 2);
    m[0, 0] = 1.5;
    m[0, 1] = -2.0;
    m[1, 0] = 0.5;
    m[1, 1] = 4.0;

    m *= 2.0;

    EXPECT_DOUBLE_EQ((m[0, 0]), 3.0);
    EXPECT_DOUBLE_EQ((m[0, 1]), -4.0);
    EXPECT_DOUBLE_EQ((m[1, 0]), 1.0);
    EXPECT_DOUBLE_EQ((m[1, 1]), 8.0);
}

TEST(MatrixTest, TransposeChangesDimensionsAndValues) {
    Matrix m(2, 3);
    m[0, 0] = 1.0;
    m[0, 1] = 2.0;
    m[0, 2] = 3.0;
    m[1, 0] = 4.0;
    m[1, 1] = 5.0;
    m[1, 2] = 6.0;

    m.transpose();
    auto [rows, cols] = m.size();

    EXPECT_EQ(rows, 3);
    EXPECT_EQ(cols, 2);
    EXPECT_DOUBLE_EQ((m[0, 0]), 1.0);
    EXPECT_DOUBLE_EQ((m[1, 0]), 2.0);
    EXPECT_DOUBLE_EQ((m[2, 0]), 3.0);
    EXPECT_DOUBLE_EQ((m[0, 1]), 4.0);
    EXPECT_DOUBLE_EQ((m[1, 1]), 5.0);
    EXPECT_DOUBLE_EQ((m[2, 1]), 6.0);
}

TEST(MatrixTest, CopyConstructorPerformsDeepCopy) {
    Matrix source(2, 2);
    source[0, 0] = 9.0;
    source[1, 1] = 8.0;

    Matrix copy(source);
    source[0, 0] = 0.0;

    EXPECT_DOUBLE_EQ((copy[0, 0]), 9.0);
    EXPECT_DOUBLE_EQ((copy[1, 1]), 8.0);
}

TEST(MatrixTest, AssignmentPerformsDeepCopyAndSupportsSelfAssignment) {
    Matrix source(2, 2);
    source[0, 0] = 5.5;
    source[0, 1] = 6.5;

    Matrix target(2, 2);
    target = source;
    source[0, 0] = -1.0;

    EXPECT_DOUBLE_EQ((target[0, 0]), 5.5);
    EXPECT_DOUBLE_EQ((target[0, 1]), 6.5);

    Matrix &self_ref = target;
    self_ref = self_ref;
    EXPECT_DOUBLE_EQ((target[0, 0]), 5.5);
}

TEST(MatrixTest, EqualityAndInequalityWorkWithTolerance) {
    Matrix a(2, 2);
    Matrix b(2, 2);

    a[0, 0] = 1.0;
    a[0, 1] = 2.0;
    a[1, 0] = 3.0;
    a[1, 1] = 4.0;

    b = a;
    b[1, 1] = 4.0 + 1e-10;
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);

    b[1, 1] = 4.0 + 1e-6;
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(MatrixTest, SaveAndLoadRoundTripPreservesValues) {
    Matrix m(2, 3);
    m[0, 0] = 1.25;
    m[0, 1] = -2.5;
    m[0, 2] = 3.75;
    m[1, 0] = 4.0;
    m[1, 1] = 5.5;
    m[1, 2] = -6.25;

    const std::string path = make_temp_path("matrix_roundtrip_t1.txt");
    m.save_to_file(path);

    Matrix loaded(path);
    EXPECT_TRUE(m == loaded);

    std::filesystem::remove(path);
}

TEST(MatrixTest, ConstructorFromFileThrowsForMissingFile) {
    const std::string path = make_temp_path("matrix_missing_t1.txt");
    std::filesystem::remove(path);

    EXPECT_THROW({
        Matrix loaded(path);
        (void)loaded;
    }, std::runtime_error);
}

TEST(MatrixTest, ConstructorFromFileThrowsForInvalidContent) {
    const std::string path = make_temp_path("matrix_bad_t1.txt");
    {
        std::ofstream out(path);
        out << "2 2\n1.0 2.0 3.0\n";
    }

    EXPECT_THROW({
        Matrix loaded(path);
        (void)loaded;
    }, std::runtime_error);

    std::filesystem::remove(path);
}

TEST(MatrixTest, SaveToFileThrowsForInvalidPath) {
    Matrix m(1, 1);
    EXPECT_THROW({
        m.save_to_file("/path/that/does/not/exist/matrix.txt");
    }, std::runtime_error);
}

TEST(MatrixTest, StreamOutputUsesRowByRowFormat) {
    Matrix m(2, 2);
    m[0, 0] = 1.0;
    m[0, 1] = 2.0;
    m[1, 0] = 3.0;
    m[1, 1] = 4.0;

    std::ostringstream out;
    out << m;
    EXPECT_EQ(out.str(), "1 2\n3 4");
}

