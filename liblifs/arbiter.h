//
// Created by shawn on 2/13/24.
//

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <memory>

#include "dictionary.h"

#ifndef LIFS_MAIN_ARBITER_H
#define LIFS_MAIN_ARBITER_H

namespace lifs::arbiter {

    struct NodeInfo {
        std::string name;
    };

    struct INode {
        std::vector<uint8_t> buffer;
        std::vector<std::string> nodeNames;
        bool dirty;

        explicit INode() : dirty(false) {}
    };

    class Arbiter {
    public:
        Arbiter() = default;

        /*---------- node ----------*/
        std::vector<std::string> listNodes() const;

        void addNode(const std::string &name, dictionary::Dictionary *d);

        void sendTo(const std::string &path, const std::string &nodeName);

        /*---------- file ----------*/
        bool exist(const std::string &path);

        std::vector<NodeInfo> devStatus(const std::string &path);

        bool create(const std::string &path);

        int read(const std::string &path, std::vector<uint8_t> &buf, int offset, int size);

        int write(const std::string &path, const std::vector<uint8_t> &buf, int offset);

        bool isDirty(const std::string &path);

        bool flush();

    private:
        std::unordered_map<std::string, dictionary::Dictionary *> nodes;
        std::unordered_map<std::string, INode> sendBuffer;

        dictionary::Dictionary *existInNode(const std::string &path);

        bool existInBuffer(const std::string &path);
    };

} // lifs

#endif //LIFS_MAIN_ARBITER_H
