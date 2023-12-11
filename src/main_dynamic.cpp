#include <iostream>
#include <dlfcn.h>

const char* libs[2] = {"./lib/dynamic/lib1.so", "./lib/dynamic/lib2.so"};

int main(){
    int selectedLibrary;
    std::cout << "Select library: enter (0) or (1): ";
    std::cin >> selectedLibrary;
    if(selectedLibrary != 0 && selectedLibrary != 1){
        std::cerr << "No such library" << std::endl;
        exit(-1);
    }

    void* lib_header = dlopen(libs[selectedLibrary], RTLD_LAZY);
    if(lib_header == NULL) {
        std::cerr << "Cannot load library" << std::endl;
    }

    char* (*translation) (long x) = (char* (*)(long)) dlsym(lib_header, "translation");
    float (*e) (int x) = (float (*)(int)) dlsym(lib_header, "e");

    if(translation == NULL || e == NULL){
        std::cerr << "Cannot load library functions" << std::endl;
        exit(-1);
    }

    int choice;
    std::cout << "Select library function: (0) || (1) to select a function\n(2) to toggle library\n(3) to exit\n";
    std::cin >> choice;
    while(choice != 3) {
        switch(choice) {
            case (0):
                int xTranslation;
                std::cout << "X: ";
                std::cin >> xTranslation;
                std::cout << translation(xTranslation) << std::endl;
                break;

            case (1):
                float xE;
                std::cout << "X: ";
                std::cin >> xE;
                std::cout << e(xE) << std::endl;
                break;

            case (2):
                if(dlclose(lib_header) != 0){
                    std::cerr << "Cannot close library" << std::endl;
                    exit(-1);
                }

                selectedLibrary = !selectedLibrary;
                lib_header = dlopen(libs[selectedLibrary], RTLD_LAZY);
                if(lib_header == NULL){
                    std::cerr << "Cannot load library" << std::endl;
                }

                translation = (char* (*)(long)) dlsym(lib_header, "translation");
                e = (float (*)(int)) dlsym(lib_header, "e");

                if(translation == NULL || e == NULL){
                    std::cerr << "Cannot load functions" << std::endl;
                    exit(-1);
                }
                break;

            default:
                std::cout << "No such function" << std::endl;
        }
        std::cin >> choice;
    }

    if(dlclose(lib_header) != 0) {
        std::cerr << "Cannot close library" << std::endl;
        exit(-1);
    }

    return 0;
}