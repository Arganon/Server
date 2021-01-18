#include <chrono>
#include <thread>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <iostream>

#include "../../utils/public/logger.h"
#include "../../common/public/storage.h"
#include "../../data/public/data_structures.h"

#define DEFAULT_FOLDER "/files_storage/"

Storage::Storage() {
	m_cache = new Cache;
	m_root_storage_folder = get_current_dir();

	std::thread t
	(
		[&]() {
			while (true) {
				std::this_thread::sleep_for (std::chrono::seconds(60*10));
				{
					std::unique_lock < std::mutex > lock { m_mutex };
					if (m_cache) {
						m_cache->m_files_cache.clear();
					}
				}
			}
		}
	);

	t.detach();
}

Storage::~Storage() {
	delete m_cache;
}

Storage& Storage::instance() {
	static Storage storage;
	return storage;
}

std::string Storage::get_current_dir(void) const noexcept {
    char curr_folder_path[PATH_MAX];
    std::string storage_folder_path;

    if (getcwd(curr_folder_path, sizeof(curr_folder_path)) == nullptr) {
        LOG(LOG_LVL::LOGS::DEBUG, "Can't receive path to execution folder.");
    }
    else {
        storage_folder_path = curr_folder_path;
		storage_folder_path.append(DEFAULT_FOLDER);
    }

    return storage_folder_path;
}

void Storage::set_root_folder(std::string _path) {
	m_root_storage_folder = _path;
}

size_t Storage::get_file_size(const std::string & _filename) const noexcept {
    size_t size = 0;
	const std::string path = m_root_storage_folder + _filename;
	FILE *fp = fopen(path.c_str(), "rb");

    if (fp != nullptr) {
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fclose(fp);
    }

    return size;
}

bool Storage::is_get_file_from_cache(const std::string & _path,
									 std::string & _result_buffer,
									 size_t _start_read_pos,
									 size_t _size_to_read) noexcept
{
	bool result = false;

	{
		std::unique_lock < std::mutex > lock { m_mutex };
		auto it = m_cache->m_files_cache.find(_path);

		if (it != m_cache->m_files_cache.end()) {
			_result_buffer = it->second.substr(
						_start_read_pos,
						_size_to_read);

			result = true;
		}
	}

	return result;
}


bool Storage::get_file(const std::string &_filename,
					   std::string &_result_buffer,
					   size_t _start_read_pos,
					   size_t _size_to_read) noexcept {

	const std::string path = m_root_storage_folder + _filename;
	FILE* fp = fopen(path.c_str(), "rb");
	bool result { false };
	result = is_get_file_from_cache(path, _result_buffer, _start_read_pos, _size_to_read);
	if (!result && fp != nullptr) {
		fseek(fp, _start_read_pos, SEEK_SET);

		char *buffer = (char*)malloc(_size_to_read);
		size_t size = fread(buffer, 1, _size_to_read, fp);
		_result_buffer.append(buffer, size);
		result = true;

		delete buffer;
		fclose(fp);
	}

	return result;

}
