//
// Created by Shawn on 2/13/24.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <future>
#include <shared_mutex>
#include <mio/mmap.hpp>


#ifndef LIFS_STORAGE_H
#define LIFS_STORAGE_H

namespace lifs::storage {
    const size_t MMAP_FILE_SIZE = 1024 * 1024 * 128; // 128MB
    using Buf = typename std::vector<uint8_t>;

    class Storage {
    public:
        virtual bool open() = 0;

        virtual bool close() = 0;

        std::future<size_t> write(const Buf &data, int32_t pos);

        std::future<std::unique_ptr<Buf>> read(int32_t pos, size_t size);

    protected:
        static int handle_error(const std::error_code &error);

        mutable std::shared_mutex file_mutex;

    private:
        virtual std::unique_ptr<Buf> _read(int32_t pos, size_t size) = 0;

        virtual size_t _write(const Buf &data, int32_t pos) = 0;
    };

    class FileStorage : public Storage {
    public:
        explicit FileStorage(std::string path);

        bool open() override;

        bool close() override;

    private:
        std::string path;
        std::fstream file;
        mio::mmap_sink mmap;

        std::unique_ptr<Buf> _read(int32_t pos, size_t size) override;

        size_t _write(const lifs::storage::Buf &data, int32_t pos) override;

        int sync();
    };

} // lifs.storage

#endif //LIFS_STORAGE_H
