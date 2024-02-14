//
// Created by Shawn on 2/14/24.
//

#include <filesystem>
#include <iostream>
#include <vector>
#include <gtest/gtest.h>

#include "storage.h"

inline void clearFile(const std::string &fileName) {
    if (std::filesystem::exists(fileName))
        std::filesystem::remove(fileName);
}

TEST(TestFileStorage, OpenFile) {
    clearFile("./OpenFile.storage");
    lifs::storage::FileStorage fileStorage("./OpenFile.storage");
    EXPECT_TRUE(fileStorage.open());
    EXPECT_TRUE(fileStorage.close());
    clearFile("./OpenFile.storage");
}

TEST(TestFileStorage, BasicIO) {
    clearFile("./BasicIO.storage");
    lifs::storage::FileStorage fileStorage("./BasicIO.storage");
    EXPECT_TRUE(fileStorage.open());
    std::vector<uint8_t> data = {'a', 'b', 'c', 'd'};
    auto f = fileStorage.write(data, 0);
    EXPECT_EQ(f.get(), data.size());
    auto res = fileStorage.read(0, data.size());
    EXPECT_EQ(data, *res.get());
    EXPECT_TRUE(fileStorage.close());
    clearFile("./BasicIO.storage");
}

TEST(TestFileStorage, LargeIO) {
    clearFile("./LargeIO.storage");
    lifs::storage::FileStorage fileStorage("./LargeIO.storage");
    EXPECT_TRUE(fileStorage.open());
    std::vector<uint8_t> data(lifs::storage::MMAP_FILE_SIZE * 2);
    auto f = fileStorage.write(data, 0);
    EXPECT_EQ(f.get(), 0);
    clearFile("./LargeIO.storage");
}
