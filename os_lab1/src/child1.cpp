#include <string>
#include <iostream>

std::string to_lower(const std::string& arg) {
    std::string res;

    for (auto ch: arg)
        res += tolower(ch);

    return res;
}

int main(int argc, char *argv[])
{
    std::cout << "This is child process with pid: " << getpid() << std::endl;

    std::string source1;
    std::cin >> source1;
    std::cout << to_lower(source1) << std::endl;

    return 0;
}
