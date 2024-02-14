//
// Created by Shawn on 2/13/24.
//

#include <fstream>
#include <functional>
#include <mio/mmap.hpp>

#include "storage.h"


namespace lifs::storage {
    std::future<std::unique_ptr<Buf>> Storage::read(int32_t pos, size_t size) {
        std::packaged_task<std::unique_ptr<Buf>()> task(
                [this, pos, size]() {
                    return _read(pos, size);
                });
        task();
        return task.get_future();
    }

    std::future<size_t> Storage::write(const lifs::storage::Buf &data, int32_t pos) {
        std::packaged_task<size_t()> task(
                [this, data, pos]() {
                    return _write(data, pos);
                });
        task();
        return task.get_future();
    }

    int Storage::handle_error(const std::error_code &error) {
        if (!error)
            return 0;
        std::cerr << "Storage error: " << error.message() << std::endl;
        return error.value();
    }

    FileStorage::FileStorage(std::string path) : path(std::move(path)) {}


    bool FileStorage::open() {
        file.open(path, std::ios::out);
        file.seekp(MMAP_FILE_SIZE - 1);
        file.write("", 1);
        file.flush();
        if (!(file.is_open() && file.good())) { return false; }
        std::error_code err;
        mmap = mio::make_mmap_sink(path, 0, mio::map_entire_file, err);
        if (handle_error(err)) { return false; }
        return true;
    }

    bool FileStorage::close() {
        if (sync()) { return false; }
        mmap.unmap();
        file.close();
        return true;
    }

    std::unique_ptr<Buf> FileStorage::_read(int32_t pos, size_t size) {
        if (pos < 0) {
            std::cerr << "Error parameters <pos>." << std::endl;
            return nullptr;
        }
        if (mmap.begin() + size + pos >= mmap.end()) {
            std::cerr << "Read overflow." << std::endl;
            return nullptr;
        }
        std::shared_lock lock(file_mutex);
        auto res = std::make_unique<Buf>(size);
        std::copy(mmap.begin() + pos, mmap.begin() + pos + size, res->begin());
        return res;
    }

    size_t FileStorage::_write(const Buf &data, int32_t pos) {
        if (mmap.begin() + pos + data.size() >= mmap.end()) {
            std::cerr << "Write overflow." << std::endl;
            return 0;
        }
        std::unique_lock lock(file_mutex);
        std::copy(data.begin(), data.end(), mmap.begin() + pos);
        if (sync()) { return 0; }
        return data.size();
    }

    int FileStorage::sync() {
        std::error_code err;
        mmap.sync(err);
        return handle_error(err);
    }

} // lifs.storage