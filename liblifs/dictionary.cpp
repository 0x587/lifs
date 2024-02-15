//
// Created by Shawn on 2/14/24.
//

#include "dictionary.h"
#include <zipconf.h>

#include <utility>

namespace lifs::dictionary {
    SystemDictionary::SystemDictionary(std::string rootPath)
            : rootPath(std::move(rootPath)) {}

    int SystemDictionary::initial() {
        if (std_fs::is_directory(rootPath)) {
            return 0;
        }
        return -1;
    }

    bool SystemDictionary::exist(std_fs::path path) {
        return std_fs::exists(rootPath / path);
    }

    void SystemDictionary::add(std_fs::path path) {
        std::ofstream f;
        f.open(rootPath / path, std::ios::app | std::ios::out);
        f.close();
    }

    bool SystemDictionary::remove(std_fs::path path) {
        if (!exist(path)) { return false; }
        return std_fs::remove(rootPath / path);
    }

    bool SystemDictionary::move(std_fs::path oldPath, std_fs::path newPath) {
        if (!exist(oldPath)) { return false; }
        std_fs::rename(rootPath / oldPath, rootPath / newPath);
        return true;
    }

    std::unique_ptr<std::fstream> SystemDictionary::open(std_fs::path path, std::ios::openmode mode) {
        auto fd = std::make_unique<std::fstream>();
        fd->open(rootPath / path, mode);
        return fd;
    }

    bool SystemDictionary::close(const std::unique_ptr<std::fstream> &fd) {
        fd->close();
        return true;
    }
} // lifs.dictionary