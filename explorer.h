
#ifndef TASK_EXPLORER_H
#define TASK_EXPLORER_H
#include <filesystem>
#include <vector>
#include <regex>
#include <functional>
#include <mutex>

class Explorer
{
    using Path = std::filesystem::path;

public:

    Explorer(const std::vector<Path>& roots);
    void SearchFile(const std::string& text);

private:

    void Collect(const Path& roots);
    void RunThreads(std::function<void(const size_t, const size_t)> foo);
    void SearchFile(const size_t begin, const size_t end, const std::string& text);

    std::vector<Path> m_files;
    std::mutex m_filesMutex;
    std::mutex m_coutMutex;
};
#endif 
