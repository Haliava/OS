#include <string>
#include <iostream>

std::string replace_spaces(std::string source, char replacement) {
    for (auto spaceChar: {' ', '\t', '\n', '\r', '\t', '\v'})
        std::replace(source.begin(), source.end(), spaceChar, replacement);

    return source;
}

int main(int argc, char *argv[])
{
    std::cout << "This is child process with pid: " << getpid() << std::endl;

    std::string source1;
    std::cin >> source1;
    std::cout << replace_spaces(source1, '_') << std::endl;

    return 0;
}
