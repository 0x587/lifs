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
        if (!existInBuffer(path)) { return; }
        auto &buf = sendBuffer[path];
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

    bool Arbiter::existInBuffer(const std::string &path) {
        return sendBuffer.find(path) != sendBuffer.end();
    }

    bool Arbiter::exist(const std::string &path) {
        if (existInNode(path) != nullptr) { return true; }
        return existInBuffer(path);
    }

    std::vector<NodeInfo> Arbiter::devStatus(const std::string &path) {
        std::vector<NodeInfo> res;
        if (existInBuffer(path)) {
            for (const auto &name: sendBuffer[path].nodeNames) {
                res.push_back(NodeInfo{name});
            }
        }
        return res;
    }

    bool Arbiter::create(const std::string &path) {
        if (exist(path)) { return false; }
        sendBuffer[path] = INode();
        return true;
    }

    int Arbiter::read(const std::string &path, std::vector<uint8_t> &buf, int offset, int size) {
        dictionary::Dictionary *d = existInNode(path);
        if (d != nullptr) {
            auto fs = d->open(path, std::ios::in);
            fs->seekp(offset);
            buf.resize(size);
            int readLen = (int) fs->readsome((char *) buf.data(), size);
            d->close(fs);
            return readLen;
        }
        if (existInBuffer(path)) {
            int len = (int) sendBuffer[path].buffer.size();
            if (offset + size > len) {
                size = len - offset;
            }
            buf.resize(size);
            std::copy(sendBuffer[path].buffer.begin(), sendBuffer[path].buffer.begin() + size, buf.begin());
            sendBuffer[path].dirty = true;
            return size;
        }
        return 0;
    }

    int Arbiter::write(const std::string &path, const std::vector<uint8_t> &buf, int offset) {
        dictionary::Dictionary *d = existInNode(path);
        if (d != nullptr) {
            auto fs = d->open(path, std::ios::out);
            fs->seekp(offset);
            fs->write((char *) buf.data(), (int) buf.size());
            d->close(fs);
            return (int) buf.size();
        }
        if (existInBuffer(path)) {
            if (sendBuffer[path].buffer.size() < offset + buf.size()) {
                sendBuffer[path].buffer.resize(offset + buf.size());
            }
            std::copy(buf.begin(), buf.end(), sendBuffer[path].buffer.begin() + offset);
            sendBuffer[path].dirty = true;
            return (int) buf.size();
        }
        return 0;
    }

    bool Arbiter::isDirty(const std::string &path) {
        if (!existInBuffer(path)) { return false; }
        return sendBuffer[path].dirty;
    }

    bool Arbiter::flush() {
        for (auto &pair: sendBuffer) {
            const auto &path = pair.first;
            auto &inode = pair.second;
            if (!inode.dirty) { continue; }
            for (const auto &nodeName: inode.nodeNames) {
                auto fs = nodes[nodeName]->open(path, std::ios::out);
                fs->write((char *) inode.buffer.data(), (int) inode.buffer.size());
            }
        }
        sendBuffer.clear();
        return true;
    }
} // lifs