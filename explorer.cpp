#include "explorer.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
namespace fs = std::filesystem;

constexpr size_t gMaxThreadCount = 16;
size_t gCurrentThreads = 0;
std::atomic_uint32_t gCurrentThreadsMutex;

bool IsSubstrinStr(const std::string& substr, const std::string& str)
{
	std::string::const_iterator subStrItr = substr.begin();

	for (const auto& a : str)
	{
		if (a == *subStrItr)
		{
			++subStrItr;
			if (subStrItr == substr.end())
			{
				return true;
			}
		}
		else 
		{
			subStrItr = substr.begin();
		}
	}

	return false;
}

Explorer::Explorer(const std::vector<Path>& roots, const std::wregex& filters)
{
	for (const Path& root : roots)
	{
		Collect(root, filters);
	}
}

void Explorer::Collect(const Path& root, const std::wregex& filters)
{
	std::vector<std::thread> threads;

	std::vector<Path> directorysForSearch;
	directorysForSearch.reserve(3000);
	directorysForSearch.push_back(root);

	while (!directorysForSearch.empty())
	{
		const Path path = directorysForSearch.back();
		directorysForSearch.pop_back();

		for (const Path file : fs::directory_iterator(path))
		{
			DWORD flags = GetFileAttributesW(file.wstring().c_str());
			if (flags & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))
			{
				continue;
			}

			fs::file_status fileStatus = fs::status(file);
			if (fileStatus.type() == fs::file_type::directory)
			{
				if (gCurrentThreads < gMaxThreadCount)
				{ 
					threads.push_back(std::thread([this, file, filters]()
					{
						++gCurrentThreads;
						Collect(file, filters);
						--gCurrentThreads;
					}));
				}
				else
				{
					directorysForSearch.push_back(file);
				}
			}

			if (fileStatus.type() == fs::file_type::regular &&
				std::regex_search(file.wstring(), filters))
			{
				std::lock_guard<std::mutex> guard(m_filesMutex);
				m_files.push_back(file);
			}
		}
	}

	for (auto& thred : threads)
	{
		thred.join();
	}
}

void Explorer::SearchFile(const std::string& text) 
{
	RunThreads([&text, this](const size_t begin, const size_t end) {
		SearchFile(begin, end, text);
	});
}

void Explorer::SearchFile(const size_t begin, const size_t end, const std::string& text)
{
	std::string buff;
	for (size_t i = begin; i < end; ++i)
	{
		const std::wstring wideName = m_files[i].wstring();
		if (IsSubstrinStr(text, std::string(wideName.begin(), wideName.end())))
		{
			buff += m_files[i].string() + '\n';
		}
	}
	std::lock_guard<std::mutex> guard(m_coutMutex);
	std::cout << buff;
}

void Explorer::SearchText(const std::string& text)
{
	RunThreads([&text, this](const size_t begin, const size_t end) {
		SearchText(begin, end, text);
	});
}

void Explorer::SearchText(const size_t begin, const size_t end, const std::string& text)
{
	std::string outBuff;
	std::string buff;
	for (size_t i = begin; i < end; ++i)
	{
		bool printName = true;
		std::fstream file(m_files[i]);

		size_t lineNumber = 1;
		while (file)
		{
			std::getline(file, buff);
			if (IsSubstrinStr(text, buff))
			{
				if (printName)
				{
					const std::wstring wideName = m_files[i].wstring();
					outBuff += std::string(wideName.begin(), wideName.end()) + '\n';
					printName = false;
				}
				outBuff += "line " + std::to_string(lineNumber) + ": " + buff + '\n';
			}
			++lineNumber;
		}
	}
	std::lock_guard<std::mutex> guard(m_coutMutex);
	std::cout << outBuff;
}

void Explorer::RunThreads(
	std::function<void(const size_t, const size_t)> foo)
{
	const size_t itemsPerThread = m_files.size() / gMaxThreadCount;
	if (itemsPerThread < 20)
	{
		foo(0, m_files.size());
		return;
	}

	std::vector<std::thread> threads;
	threads.reserve(gMaxThreadCount);

	for (size_t i = 0; i < m_files.size(); i += itemsPerThread)
	{
		threads.push_back(std::thread(
			[begin = i,
			end = std::min(i + itemsPerThread, m_files.size()),
			&foo]() {
			foo(begin, end);
		}
		));
	}

	for (auto& thread : threads)
	{
		thread.join();
	}
}