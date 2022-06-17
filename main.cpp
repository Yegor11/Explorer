#include <iostream>
#include "explorer.h"

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
    Explorer explorer(
            {
                    L"D:\\",
                    L"C:\\Users"
            });

    std::string command;
    std::string text;

    while (command != "exit")
    {
        std::cin >> command;

        if (command == "search")
        {
            getline(text);
            system("cls");
            explorer.SearchFile(text);
        }
    }
    return 0;
}