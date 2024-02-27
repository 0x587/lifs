//
// Created by szm on 2/27/24.
//

#include <gtest/gtest.h>

#include "dictionary.h"
#include "arbiter.h"

TEST(TestArbiter, TestNodesAddAndList) {
    system("mkdir TestNodes1");
    system("mkdir TestNodes2");
    lifs::dictionary::SystemDictionary dictionary1("TestNodes1");
    dictionary1.initial();
    lifs::dictionary::SystemDictionary dictionary2("TestNodes2");
    dictionary2.initial();
    lifs::arbiter::Arbiter arbiter{};
    arbiter.addNode("dictionary1", &dictionary1);
    arbiter.addNode("dictionary2", &dictionary2);
    std::vector<std::string> names = {"dictionary2", "dictionary1"};
    EXPECT_EQ(arbiter.listNodes(), names);
    system("rm -rf TestNodes1");
    system("rm -rf TestNodes2");
}

TEST(TestArbiter, TestIO) {
    system("mkdir TestIO1");
    system("mkdir TestIO2");
    lifs::dictionary::SystemDictionary dictionary1("TestIO1");
    dictionary1.initial();
    lifs::dictionary::SystemDictionary dictionary2("TestIO2");
    dictionary2.initial();
    lifs::arbiter::Arbiter arbiter{};
    arbiter.addNode("dictionary1", &dictionary1);
    arbiter.addNode("dictionary2", &dictionary2);

    std::string s(1024, 'a');
    std::vector<uint8_t> data(s.begin(), s.end());

    EXPECT_FALSE(arbiter.exist("file1"));
    arbiter.create("file1");
    EXPECT_TRUE(arbiter.exist("file1"));

    EXPECT_FALSE(arbiter.isDirty("file1"));
    EXPECT_EQ(arbiter.write("file1", data, 0), data.size());
    EXPECT_TRUE(arbiter.isDirty("file1"));

    std::vector<uint8_t> buf;
    EXPECT_EQ(arbiter.read("file1", buf, 0, (int) data.size()), data.size());
    EXPECT_EQ(buf, data);

    arbiter.sendTo("file1", "dictionary1");
    arbiter.flush();
    EXPECT_FALSE(arbiter.isDirty("file1"));

    buf.clear();
    EXPECT_EQ(arbiter.read("file1", buf, 0, (int) data.size()), data.size());
    EXPECT_EQ(buf, data);

    std::vector<std::string> nodes{"dictionary1"};
    EXPECT_EQ(arbiter.nodeStatus("file1").size(), 1);
    EXPECT_EQ(arbiter.nodeStatus("file1")[0].name, "dictionary1");

    system("rm -rf TestIO1");
    system("rm -rf TestIO2");
}