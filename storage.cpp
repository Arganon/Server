#include <unistd.h>
#include <limits.h>
#include <dirent.h>

#include "storage.h"

std::string Storage::get_current_dir() const noexcept {
    char curr_folder_path[PATH_MAX];
    if (getcwd(curr_folder_path, sizeof(curr_folder_path)) != NULL) {
        printf("Current working dir: %s\n", curr_folder_path);
    } else {
        perror("getcwd() error");
    }
    return curr_folder_path;
}

std::set<std::string> Storage::get_files_list() const noexcept {
    DIR *d;
    struct dirent *dir;
    std::set<std::string> files_list;

    d = opendir(".");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type == DT_REG && dir->d_name[0] != '.')
            {
               files_list.insert(dir->d_name);
            }
        }
        closedir(d);
    }
    return files_list;
}
