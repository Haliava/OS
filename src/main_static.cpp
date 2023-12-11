#include <iostream>

extern "C" {
    char* translation(int x);
    float e(int x);
}

int main(){
    size_t selectedLibrary;
    std::cout << "(0) - first func, (1) - second, (-1) - exit\n";
    std::cin >> selectedLibrary;
    while(selectedLibrary != -1) {
        switch(selectedLibrary) {
            case (0):
                std::cout << "X: ";
                int xTranslate;
                std::cin >> xTranslate;
                std::cout << translation(xTranslate) << std::endl;
                break;

            case (1):
                std::cout << "X: ";
                float xE;
                std::cin >> xE;
                std::cout << e(xE) << std::endl;
                break;

            default:
                std::cout << "No such library" << std::endl;
        }
        std::cout << "(0) - first func, (1) - second, (-1) - exit\n";
        std::cin >> selectedLibrary;
    }
    return 0;
}