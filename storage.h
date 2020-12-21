#ifndef STORAGE_H
#define STORAGE_H

#include <set>
#include <string>

class Storage
{
    public:
        Storage() = default;
        ~Storage() = default;

        std::set<std::string> get_files_list() const noexcept;

    private:
        std::string get_current_dir() const noexcept;
};

#endif // STORAGE_H
