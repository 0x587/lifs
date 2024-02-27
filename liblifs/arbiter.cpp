//
// Created by shawn on 2/13/24.
//

#include "arbiter.h"

namespace lifs::arbiter {
    std::vector<std::string> Arbiter::listNodes() const {
        std::vector<std::string> res;
        std::transform(nodes.begin(), nodes.end(), std::back_inserter(res),
                       [](const auto &pair) { return pair.first; });
        return res;
    }

    void Arbiter::sendTo(const std::string &path, const std::string &nodeName) {
        if (!exist(path)) { return; }

        auto &buf = inodes[path];
        if (std::find(buf.nodeNames.begin(), buf.nodeNames.end(), nodeName) == buf.nodeNames.end()) {
            buf.nodeNames.push_back(nodeName);
            buf.dirty = true;
        }
    }

    void Arbiter::addNode(const std::string &name, dictionary::Dictionary *d) {
        nodes[name] = d;
    }

    dictionary::Dictionary *Arbiter::existInNode(const std::string &path) {
        for (const auto &pair: nodes) {
            if (pair.second->exist(path)) { return pair.second; }
        }
        return nullptr;
    }

    bool Arbiter::exist(const std::string &path) {
        return inodes.find(path) != inodes.end();
    }

    std::vector<NodeInfo> Arbiter::nodeStatus(const std::string &path) {
        std::vector<NodeInfo> res;

        if (!exist(path)) { return res; }

        for (const auto &name: inodes[path].nodeNames) {
            res.push_back(NodeInfo{name});
        }
        return res;
    }

    bool Arbiter::create(const std::string &path) {
        if (exist(path)) { return false; }

        inodes[path] = INode();
        return true;
    }

    int Arbiter::read(const std::string &path, std::vector<uint8_t> &buf, int offset, int size) {
        if (!exist(path)) { return 0; }

        if (isDirty(path)) {
            int len = (int) inodes[path].buffer.size();
            if (offset + size > len) {
                size = len - offset;
            }
            buf.resize(size);
            std::copy(inodes[path].buffer.begin(), inodes[path].buffer.begin() + size, buf.begin());
            return size;
        }

        auto inode = inodes[path];
        // TODO: Checking the correctness of multiple copies
        auto dict = nodes[inode.nodeNames.front()];
        auto fs = dict->open(path, std::ios::in);
        fs->seekp(offset);
        buf.resize(size);
        int readLen = (int) fs->readsome((char *) buf.data(), size);
        dict->close(fs);
        return readLen;
    }

    int Arbiter::write(const std::string &path, const std::vector<uint8_t> &buf, int offset) {
        if (!exist(path)) { return 0; }

        if (inodes[path].buffer.size() < offset + buf.size()) {
            inodes[path].buffer.resize(offset + buf.size());
        }

        std::copy(buf.begin(), buf.end(), inodes[path].buffer.begin() + offset);
        inodes[path].dirty = true;
        return (int) buf.size();
    }

    bool Arbiter::isDirty(const std::string &path) {
        if (!exist(path)) { return false; }

        return inodes[path].dirty;
    }

    bool Arbiter::flush() {
        for (auto &pair: inodes) {
            const auto &path = pair.first;
            auto &inode = pair.second;
            if (!inode.dirty) { continue; }
            for (const auto &nodeName: inode.nodeNames) {
                auto fs = nodes[nodeName]->open(path, std::ios::out);
                fs->write((char *) inode.buffer.data(), (int) inode.buffer.size());
                inode.dirty = false;
                inode.buffer.resize(0);
            }
        }
        return true;
    }
} // lifs