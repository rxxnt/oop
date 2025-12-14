#include "ConsoleUI.h"
#include <iostream>
#include <exception>
#include <locale>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char* argv[]) {
    try {
        // Настройка кодировки консоли для Windows
        #ifdef _WIN32
            // Устанавливаем кодовую страницу UTF-8
            SetConsoleOutputCP(65001);
            SetConsoleCP(65001);
            // Настраиваем шрифт консоли для поддержки UTF-8
            system("chcp 65001 > nul");
        #else
            // Для Linux/Unix
            std::locale::global(std::locale(""));
        #endif
        
        // Можно указать имя файла через аргумент командной строки
        std::string filename = "phonebook.txt";
        if (argc > 1) {
            filename = argv[1];
        }
        
        ConsoleUI ui(filename);
        ui.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Неизвестная ошибка" << std::endl;
        return 1;
    }
    
    return 0;
}