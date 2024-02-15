//
// Created by Shawn on 2/14/24.
//

#ifndef LIFS_MAIN_DICTIONARY_H
#define LIFS_MAIN_DICTIONARY_H

#include <filesystem>
#include <memory>
#include <chrono>
#include <ctime>
#include <zip.h>

#include "storage.h"

namespace lifs::dictionary {
    namespace std_fs = std::filesystem;

    class Dictionary {
    public:
        virtual int initial() = 0;

        virtual bool exist(std_fs::path path) = 0;

        virtual void add(std_fs::path path) = 0;

        virtual bool remove(std_fs::path path) = 0;

        virtual bool move(std_fs::path oldPath, std_fs::path newPath) = 0;

        virtual std::unique_ptr<std::fstream> open(std_fs::path path, std::ios::openmode mode) = 0;

        virtual bool close(const std::unique_ptr<std::fstream> &fd) = 0;
    };

    class SystemDictionary : public Dictionary {

    public:
        explicit SystemDictionary(std::string rootPath);

        int initial() override;

        bool exist(std_fs::path path) override;

        void add(std_fs::path path) override;

        bool remove(std_fs::path path) override;

        bool move(std_fs::path oldPath, std_fs::path newPath) override;

        std::unique_ptr<std::fstream> open(std_fs::path path, std::ios::openmode mode) override;

        bool close(const std::unique_ptr<std::fstream> &fd) override;

    private:
        const std::string rootPath;
    };

} // lifs.dictionary

#endif //LIFS_MAIN_DICTIONARY_H
