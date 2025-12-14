#ifndef CONSOLEUI_H
#define CONSOLEUI_H

#include "PhoneBook.h"
#include <string>

class ConsoleUI {
private:
    PhoneBook phoneBook;
    bool running;
    
    // Вспомогательные методы для ввода
    std::string readLine(const std::string& prompt) const;
    int readInt(const std::string& prompt, int min = 0, int max = 100) const;
    bool confirm(const std::string& question) const;
    void clearScreen() const;
    void pauseScreen() const;
    
    // Методы меню
    void showMainMenu() const;
    void showContactList() const;
    void showContact(size_t index) const;
    void addContactMenu();
    void editContactMenu();
    void deleteContactMenu();
    void searchMenu();
    void sortMenu();
    void importExportMenu();
    
    // Вспомогательные методы для работы с контактами
    Contact inputContact(bool fullInput = true) const;
    PhoneType selectPhoneType() const;
    void editContactField(Contact& contact);
    
public:
    ConsoleUI(const std::string& filename = "phonebook.txt");
    void run();
};

#endif // CONSOLEUI_H