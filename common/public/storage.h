/* SPDX-License-Identifier: GLWT(Good Luck With That) Public License
 * Copyright (c) Everyone, except Author
 * Everyone is permitted to copy, distribute, modify, merge, sell, publish,
 *sublicense or whatever they want with this software but at their OWN RISK.
 */
////////////////////////////////////////////////////////////////////////////////
/// @author Yuriy.Liahovskiy@gmail.com
/// @brief Files storage class. At this moment used only to open and read files.
/// @copyright Free to use.
////////////////////////////////////////////////////////////////////////////////
#ifndef STORAGE_H
#define STORAGE_H

#include <set>
#include <string>
#include <mutex>

struct Cache;

/// @brief Files storage class.
class Storage
{
private:
	/// @brief Class is singleton.
	///        Default Constructor moved to the private section.
	Storage();

	/// @brief Class is singleton.
	///        Destructor moved to the private section.
	~Storage();

public:
	/// @brief RequestHandler is a singleton. Removed copy constructor.
	Storage(const Storage &) = delete;

	/// @brief RequestHandler is a singleton. Removed move constructor.
	Storage(Storage &&) = delete;

	/// @brief RequestHandler is a singleton. Removed copy assignment.
	Storage & operator = (const Storage &) = delete;

	/// @brief RequestHandler is a singleton. Removed move assignment.
	Storage & operator = (Storage &&) = delete;

	/// @brief Method to take RequestHandler instance.
	/// return Reference to the static RequestHandler instance.
	static Storage & instance(void);

	/// @brief Return a folder with a storage files.
	/// return The string with file storage path.
	/// @throw No throw.
	std::string get_current_dir(void) const noexcept; // TODO: move to private section

	/// @brief Set path to the root folder.
	/// @param[in] _path Path to the root folder.
	void set_root_folder(std::string _path);

	/// @brief Return a file size if file exist.
	/// @param[in] _filename The filename. Nothing else.
	/// return File size.
	/// @throw No throw.
	size_t get_file_size(const std::string & _filename) const noexcept; // return 0 if not exist

	/// @brief This method open the file, read it to the buffer and send
	///        true if reading was successful.
	/// @param[in] _filename The filename.
	/// @param[in] _result_buffer The buffer where we put the data from the file.
	/// @param[in] _start_read_pos The start position to read data from the file.
	/// @param[in] _size_to_read The total size of the data to read from the file.
	/// return True if reading was successful or False if not.
	/// @throw No throw.
	bool get_file(const std::string & _filename,
				  std::string & _result_buffer,
				  const size_t _start_read_pos = 0,
				  const size_t _size_to_read = 0) noexcept;

	/// @brief This method check file in cache. File will be copied to the buffer
	///		   if exist.
	/// @param[in] _filename The filename.
	/// @param[in] _result_buffer The buffer where we put the data from the file.
	/// @param[in] _start_read_pos The start position to read data from the file.
	/// @param[in] _size_to_read The total size of the data to read from the file.
	/// return True if file exist in the cache and False if not.
	/// @throw No throw.
	bool is_get_file_from_cache(const std::string & _path,
								std::string & _result_buffer,
								size_t _start_read_pos,
								size_t _size_to_read) noexcept;

private:
	/// @brief The root folder path.
	std::string m_root_storage_folder;

	/// @brief The pointer for cache.
	Cache *m_cache;

	/// @brief The mutex value to control the access for the cache.
	std::mutex m_mutex;
};

#endif // STORAGE_H
