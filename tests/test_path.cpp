#include <gtest/gtest.h>
#include <QDir>
#include <QFileInfo>
#include <QTemporaryDir>
#include "../common/pathsafety.h"

TEST(PathTest, SafePathWithinRoot)
{
    QTemporaryDir rootDir;
    ASSERT_TRUE(rootDir.isValid());
    QString rootPath = QDir::cleanPath(rootDir.path());

    QString testPath = QDir(rootPath).absoluteFilePath("subdir/file.txt");
    EXPECT_TRUE(isPathSafe(testPath, rootPath));
}

TEST(PathTest, UnsafePathOutsideRoot)
{
    QTemporaryDir rootDir;
    ASSERT_TRUE(rootDir.isValid());
    QString rootPath = QDir::cleanPath(rootDir.path());

    QString outsidePath = QDir::cleanPath(rootPath + "/../outside.txt");
    EXPECT_FALSE(isPathSafe(outsidePath, rootPath));
}

TEST(PathTest, RootPathItselfIsSafe)
{
    QTemporaryDir rootDir;
    ASSERT_TRUE(rootDir.isValid());
    QString rootPath = QDir::cleanPath(rootDir.path());

    EXPECT_TRUE(isPathSafe(rootPath, rootPath));
}

TEST(PathTest, DeepNestedPath)
{
    QTemporaryDir rootDir;
    ASSERT_TRUE(rootDir.isValid());
    QString rootPath = QDir::cleanPath(rootDir.path());

    QString deepPath = QDir(rootPath).absoluteFilePath("a/b/c/d/e/f.txt");
    EXPECT_TRUE(isPathSafe(deepPath, rootPath));
}

TEST(PathTest, TraversalAttempt)
{
    QTemporaryDir rootDir;
    ASSERT_TRUE(rootDir.isValid());
    QString rootPath = QDir::cleanPath(rootDir.path());

    // .. in the middle should be caught
    QString traversalPath = QDir::cleanPath(rootPath + "/subdir/../../../etc/passwd");
    EXPECT_FALSE(isPathSafe(traversalPath, rootPath));
}

TEST(PathTest, NonExistentPathIsSafe)
{
    QTemporaryDir rootDir;
    ASSERT_TRUE(rootDir.isValid());
    QString rootPath = QDir::cleanPath(rootDir.path());

    // Non-existent path inside root should be safe (it's where we'll create)
    QString newPath = QDir(rootPath).absoluteFilePath("nonexistent/dir");
    EXPECT_TRUE(isPathSafe(newPath, rootPath));
}

TEST(PathTest, RelativeRootPath)
{
    // isPathSafe should handle relative root paths by converting to absolute
    QString testPath = QDir::currentPath() + "/subdir/test.txt";
    QString rootPath = ".";
    EXPECT_TRUE(isPathSafe(testPath, rootPath));
}
