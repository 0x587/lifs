//
// Created by Shawn on 2/14/24.
//

#include <filesystem>
#include <gtest/gtest.h>

#include "dictionary.h"

inline void clearFile(const std::string &fileName) {
    if (std::filesystem::exists(fileName))
        std::filesystem::remove(fileName);
}

TEST(TestSystemDictonary, CheckFileExist) {
    system("mkdir CheckFileExist");
    lifs::dictionary::SystemDictionary dictionary("CheckFileExist");
    EXPECT_EQ(dictionary.initial(), 0);
    EXPECT_FALSE(dictionary.exist("NoExist"));
    dictionary.add("Exist.txt");
    EXPECT_TRUE(dictionary.exist("Exist.txt"));
    system("rm -rf CheckFileExist");
}

TEST(TestSystemDictonary, CheckRemove) {
    system("mkdir CheckRemove");
    lifs::dictionary::SystemDictionary dictionary("CheckRemove");
    EXPECT_EQ(dictionary.initial(), 0);
    dictionary.add("Exist.txt");
    EXPECT_TRUE(dictionary.exist("Exist.txt"));
    dictionary.remove("Exist.txt");
    EXPECT_FALSE(dictionary.exist("Exist.txt"));
    system("rm -rf CheckRemove");
}

TEST(TestSystemDictonary, CheckMove) {
    system("mkdir CheckMove");
    lifs::dictionary::SystemDictionary dictionary("CheckMove");
    EXPECT_EQ(dictionary.initial(), 0);
    dictionary.add("Exist.txt");
    EXPECT_TRUE(dictionary.exist("Exist.txt"));
    EXPECT_TRUE(dictionary.move("Exist.txt", "New.txt"));
    EXPECT_FALSE(dictionary.exist("Exist.txt"));
    EXPECT_TRUE(dictionary.exist("New.txt"));
    system("rm -rf CheckMove");
}

TEST(TestSystemDictonary, CheckIO) {
    system("mkdir CheckIO");
    lifs::dictionary::SystemDictionary dictionary("CheckIO");
    EXPECT_EQ(dictionary.initial(), 0);
    dictionary.add("File.txt");
    EXPECT_TRUE(dictionary.exist("File.txt"));
    auto out_fd = dictionary.open("File.txt", std::ios::out);
    EXPECT_NE(out_fd, nullptr);
    std::string s1(1024, 'a');
    *out_fd << s1;
    EXPECT_TRUE(dictionary.close(out_fd));

    auto in_fd = dictionary.open("File.txt", std::ios::in);
    std::string s2;
    *in_fd >> s2;
    EXPECT_EQ(s1, s2);
    EXPECT_TRUE(dictionary.close(in_fd));
    system("rm -rf CheckIO");
}