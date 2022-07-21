#include <iostream>

#include "explorer.h"
#include "perfometer.h"

void RemoveFirstSpace(std::string& text)
{
    if (text.empty())
    {
        return;
    }

    size_t count = 0;
    while (++count < text.size() && std::isspace(text[count]));

    text.erase(0, count);
}

void getline(std::string& line)
{
    line = "";
    while (line.empty())
    {
        std::getline(std::cin, line);
        RemoveFirstSpace(line);
    }
}

int main()
{
   // Perfometer mainPerf("main");

    Explorer explorer(
            {
                    L"D:\\",
                    L"C:\\Users"
            },
            std::wregex(L"\\.(cpp|h|txt)$"));

    //mainPerf.time("explorer started");

    std::string command;
    std::string text;
    while (command != "exit")
    {
        std::cin >> command;

        if (command == "file")
        {
            getline(text);
            system("cls");
            Perfometer perf("search");
            explorer.SearchFile(text);
        }

        if(command == "text")
        {
            getline(text);
            system("cls");
            Perfometer perf("search");
            explorer.SearchText(text);
        }
    }
    return 0;
}