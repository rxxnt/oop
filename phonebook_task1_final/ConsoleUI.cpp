#include "ConsoleUI.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <set>

ConsoleUI::ConsoleUI(const std::string& filename) : phoneBook(filename), running(true) {}

std::string ConsoleUI::readLine(const std::string& prompt) const {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

int ConsoleUI::readInt(const std::string& prompt, int min, int max) const {
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        
        try {
            int value = std::stoi(input);
            if (value >= min && value <= max) {
                return value;
            }
            std::cout << "Введите число от " << min << " до " << max << std::endl;
        } catch (...) {
            std::cout << "Неверный ввод. Пожалуйста, введите число." << std::endl;
        }
    }
}

bool ConsoleUI::confirm(const std::string& question) const {
    while (true) {
        std::string answer = readLine(question + " (yes/no): ");
        
        // Приводим к нижнему регистру для английского
        std::transform(answer.begin(), answer.end(), answer.begin(), ::tolower);
        
        // Проверяем английские варианты
        if (answer == "yes" || answer == "y") {
            return true;
        } else if (answer == "no" || answer == "n") {
            return false;
        }
        
        std::cout << "Пожалуйста, введите 'yes' или 'no'." << std::endl;
    }
}

void ConsoleUI::clearScreen() const {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void ConsoleUI::pauseScreen() const {
    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.get();
}

void ConsoleUI::showMainMenu() const {
    std::cout << "\n========== ТЕЛЕФОННЫЙ СПРАВОЧНИК ==========\n";
    std::cout << "1. Показать все контакты\n";
    std::cout << "2. Добавить контакт\n";
    std::cout << "3. Редактировать контакт\n";
    std::cout << "4. Удалить контакт\n";
    std::cout << "5. Поиск контактов\n";
    std::cout << "6. Сортировка контактов\n";
    std::cout << "7. Импорт/Экспорт\n";
    std::cout << "0. Выход\n";
    std::cout << "============================================\n";
    std::cout << "Всего контактов: " << phoneBook.getContactCount() << "\n";
}

void ConsoleUI::showContactList() const {
    if (phoneBook.isEmpty()) {
        std::cout << "\nСправочник пуст.\n";
        return;
    }
    
    std::cout << "\n========== СПИСОК КОНТАКТОВ ==========\n";
    auto contacts = phoneBook.getAllContacts();
    
    for (size_t i = 0; i < contacts.size(); ++i) {
        std::cout << std::setw(3) << i + 1 << ". " 
                  << contacts[i].toShortString() << std::endl;
    }
    std::cout << "=====================================\n";
}

void ConsoleUI::showContact(size_t index) const {
    const Contact* contact = phoneBook.getContact(index);
    if (contact) {
        std::cout << "\n========== ИНФОРМАЦИЯ О КОНТАКТЕ ==========\n";
        std::cout << contact->toString();
        std::cout << "==========================================\n";
    } else {
        std::cout << "Контакт не найден.\n";
    }
}

PhoneType ConsoleUI::selectPhoneType() const {
    std::cout << "Выберите тип телефона:\n";
    std::cout << "1. Рабочий\n";
    std::cout << "2. Домашний\n";
    std::cout << "3. Служебный\n";
    std::cout << "4. Другой\n";
    
    int choice = readInt("Выбор: ", 1, 4);
    switch (choice) {
        case 1: return PhoneType::WORK;
        case 2: return PhoneType::HOME;
        case 3: return PhoneType::SERVICE;
        default: return PhoneType::OTHER;
    }
}

Contact ConsoleUI::inputContact(bool fullInput) const {
    Contact contact;
    
    // Обязательные поля
    while (true) {
        std::string firstName = readLine("Имя (обязательно): ");
        if (contact.setFirstName(firstName)) {
            break;
        }
        std::cout << "Неверный формат имени. Имя должно начинаться с буквы и содержать только буквы, цифры, пробелы и дефисы.\n";
    }
    
    while (true) {
        std::string lastName = readLine("Фамилия (обязательно): ");
        if (contact.setLastName(lastName)) {
            break;
        }
        std::cout << "Неверный формат фамилии.\n";
    }
    
    // Отчество сразу после фамилии (необязательное поле)
    if (fullInput) {
        std::string patronymic = readLine("Отчество (необязательно): ");
        if (!patronymic.empty()) {
            if (!contact.setPatronymic(patronymic)) {
                std::cout << "Неверный формат отчества, поле пропущено.\n";
            }
        }
    }
    
    while (true) {
        std::string email = readLine("Email (обязательно): ");
        if (contact.setEmail(email)) {
            break;
        }
        std::cout << "Неверный формат email. Пример: user@domain.com\n";
    }
    
    while (true) {
        std::string phone = readLine("Телефон (обязательно): ");
        PhoneType type = selectPhoneType();
        if (contact.addPhoneNumber(phone, type)) {
            break;
        }
        std::cout << "Неверный формат телефона. Примеры: +78121234567, 8(812)123-45-67\n";
    }
    
    // Остальные необязательные поля
    if (fullInput) {
        std::string address = readLine("Адрес (необязательно): ");
        contact.setAddress(address);
        
        // Дата рождения
        std::string birthDateStr = readLine("Дата рождения (ДД.ММ.ГГГГ, необязательно): ");
        if (!birthDateStr.empty()) {
            Date birthDate;
            if (birthDate.fromString(birthDateStr)) {
                contact.setBirthDate(birthDate);
            } else {
                std::cout << "Неверный формат даты или дата в будущем, поле пропущено.\n";
            }
        }
        
        // Дополнительные телефоны
        while (confirm("Добавить еще телефон?")) {
            std::string phone = readLine("Телефон: ");
            PhoneType type = selectPhoneType();
            if (contact.addPhoneNumber(phone, type)) {
                std::cout << "Телефон добавлен.\n";
            } else {
                std::cout << "Неверный формат телефона.\n";
            }
        }
    }
    
    return contact;
}

void ConsoleUI::addContactMenu() {
    std::cout << "\n========== ДОБАВЛЕНИЕ КОНТАКТА ==========\n";
    
    Contact newContact = inputContact(true);
    
    if (phoneBook.addContact(newContact)) {
        std::cout << "Контакт успешно добавлен!\n";
    } else {
        std::cout << "Ошибка при добавлении контакта.\n";
    }
}

void ConsoleUI::editContactField(Contact& contact) {
    std::cout << "\nЧто вы хотите изменить?\n";
    std::cout << "1. Имя\n";
    std::cout << "2. Фамилию\n";
    std::cout << "3. Отчество\n";
    std::cout << "4. Адрес\n";
    std::cout << "5. Дату рождения\n";
    std::cout << "6. Email\n";
    std::cout << "7. Телефоны\n";
    std::cout << "0. Закончить редактирование\n";
    
    int choice = readInt("Выбор: ", 0, 7);
    
    switch (choice) {
        case 1: {
            std::string firstName = readLine("Новое имя: ");
            if (!contact.setFirstName(firstName)) {
                std::cout << "Неверный формат имени.\n";
            }
            break;
        }
        case 2: {
            std::string lastName = readLine("Новая фамилия: ");
            if (!contact.setLastName(lastName)) {
                std::cout << "Неверный формат фамилии.\n";
            }
            break;
        }
        case 3: {
            std::string patronymic = readLine("Новое отчество: ");
            if (!contact.setPatronymic(patronymic)) {
                std::cout << "Неверный формат отчества.\n";
            }
            break;
        }
        case 4: {
            std::string address = readLine("Новый адрес: ");
            contact.setAddress(address);
            break;
        }
        case 5: {
            std::string birthDateStr = readLine("Новая дата рождения (ДД.ММ.ГГГГ): ");
            Date birthDate;
            if (birthDate.fromString(birthDateStr)) {
                if (!contact.setBirthDate(birthDate)) {
                    std::cout << "Неверная дата.\n";
                }
            } else {
                std::cout << "Неверный формат даты.\n";
            }
            break;
        }
        case 6: {
            std::string email = readLine("Новый email: ");
            if (!contact.setEmail(email)) {
                std::cout << "Неверный формат email.\n";
            }
            break;
        }
        case 7: {
            auto phones = contact.getPhoneNumbers();
            std::cout << "Текущие телефоны:\n";
            for (size_t i = 0; i < phones.size(); ++i) {
                std::cout << i + 1 << ". " << phones[i].number << "\n";
            }
            
            std::cout << "1. Добавить телефон\n";
            std::cout << "2. Удалить телефон\n";
            std::cout << "3. Изменить телефон\n";
            
            int phoneChoice = readInt("Выбор: ", 1, 3);
            
            if (phoneChoice == 1) {
                std::string phone = readLine("Новый телефон: ");
                PhoneType type = selectPhoneType();
                if (!contact.addPhoneNumber(phone, type)) {
                    std::cout << "Неверный формат телефона.\n";
                }
            } else if (phoneChoice == 2 && phones.size() > 1) {
                int index = readInt("Номер телефона для удаления: ", 1, phones.size()) - 1;
                contact.removePhoneNumber(index);
            } else if (phoneChoice == 3) {
                int index = readInt("Номер телефона для изменения: ", 1, phones.size()) - 1;
                std::string phone = readLine("Новый номер: ");
                PhoneType type = selectPhoneType();
                if (!contact.updatePhoneNumber(index, phone, type)) {
                    std::cout << "Неверный формат телефона.\n";
                }
            }
            break;
        }
    }
}

void ConsoleUI::editContactMenu() {
    if (phoneBook.isEmpty()) {
        std::cout << "\nСправочник пуст.\n";
        return;
    }
    
    showContactList();
    int index = readInt("Введите номер контакта для редактирования: ", 1, phoneBook.getContactCount()) - 1;
    
    Contact* contact = phoneBook.getContact(index);
    if (!contact) {
        std::cout << "Контакт не найден.\n";
        return;
    }
    
    Contact editedContact = *contact;
    showContact(index);
    
    bool editing = true;
    while (editing) {
        editContactField(editedContact);
        
        std::cout << "\nИзмененный контакт:\n";
        std::cout << editedContact.toString();
        
        editing = confirm("Продолжить редактирование?");
    }
    
    if (confirm("Сохранить изменения?")) {
        phoneBook.updateContact(index, editedContact);
        std::cout << "Контакт обновлен.\n";
    }
}

void ConsoleUI::deleteContactMenu() {
    if (phoneBook.isEmpty()) {
        std::cout << "\nСправочник пуст.\n";
        return;
    }
    
    showContactList();
    int index = readInt("Введите номер контакта для удаления: ", 1, phoneBook.getContactCount()) - 1;
    
    showContact(index);
    
    if (confirm("Вы уверены, что хотите удалить этот контакт?")) {
        if (phoneBook.removeContact(index)) {
            std::cout << "Контакт удален.\n";
        } else {
            std::cout << "Ошибка при удалении контакта.\n";
        }
    }
}

void ConsoleUI::searchMenu() {
    if (phoneBook.isEmpty()) {
        std::cout << "\nСправочник пуст.\n";
        return;
    }
    
    std::cout << "\n========== ПОИСК КОНТАКТОВ ==========\n";
    std::cout << "1. Поиск по имени\n";
    std::cout << "2. Поиск по email\n";
    std::cout << "3. Поиск по телефону\n";
    std::cout << "4. Поиск по всем полям\n";
    
    int choice = readInt("Выбор: ", 1, 4);
    std::string query = readLine("Введите запрос: ");
    
    std::vector<size_t> results;
    
    switch (choice) {
        case 1:
            results = phoneBook.searchByName(query);
            break;
        case 2:
            results = phoneBook.searchByEmail(query);
            break;
        case 3:
            results = phoneBook.searchByPhone(query);
            break;
        case 4:
            results = phoneBook.searchMultiField(query);
            break;
    }
    
    if (results.empty()) {
        std::cout << "Контакты не найдены.\n";
    } else {
        std::cout << "\nНайдено контактов: " << results.size() << "\n";
        auto contacts = phoneBook.getAllContacts();
        
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "\n--- Результат " << i + 1 << " ---\n";
            std::cout << contacts[results[i]].toString();
        }
    }
}

void ConsoleUI::sortMenu() {
    if (phoneBook.isEmpty()) {
        std::cout << "\nСправочник пуст.\n";
        return;
    }
    
    std::cout << "\n========== СОРТИРОВКА КОНТАКТОВ ==========\n";
    std::cout << "1. По имени\n";
    std::cout << "2. По фамилии\n";
    std::cout << "3. По email\n";
    std::cout << "4. По дате рождения\n";
    
    int fieldChoice = readInt("Выберите поле для сортировки: ", 1, 4);
    
    std::cout << "1. По возрастанию\n";
    std::cout << "2. По убыванию\n";
    
    int orderChoice = readInt("Выберите порядок: ", 1, 2);
    
    SortField field = SortField::LAST_NAME;
    switch (fieldChoice) {
        case 1: field = SortField::FIRST_NAME; break;
        case 2: field = SortField::LAST_NAME; break;
        case 3: field = SortField::EMAIL; break;
        case 4: field = SortField::BIRTH_DATE; break;
    }
    
    SortOrder order = (orderChoice == 1) ? SortOrder::ASCENDING : SortOrder::DESCENDING;
    
    phoneBook.sortContacts(field, order);
    std::cout << "Контакты отсортированы.\n";
    
    showContactList();
}

void ConsoleUI::importExportMenu() {
    std::cout << "\n========== ИМПОРТ/ЭКСПОРТ ==========\n";
    std::cout << "1. Экспортировать в файл\n";
    std::cout << "2. Импортировать из файла\n";
    
    int choice = readInt("Выбор: ", 1, 2);
    
    if (choice == 1) {
        std::string filename = readLine("Введите имя файла для экспорта: ");
        if (phoneBook.exportToFile(filename)) {
            std::cout << "Данные экспортированы в файл " << filename << "\n";
        } else {
            std::cout << "Ошибка при экспорте.\n";
        }
    } else {
        std::string filename = readLine("Введите имя файла для импорта: ");
        if (phoneBook.importFromFile(filename)) {
            std::cout << "Данные импортированы из файла " << filename << "\n";
        } else {
            std::cout << "Ошибка при импорте. Проверьте существование файла.\n";
        }
    }
}

void ConsoleUI::run() {
    std::cout << "Добро пожаловать в телефонный справочник!\n";
    
    while (running) {
        showMainMenu();
        int choice = readInt("Выберите действие: ", 0, 7);
        
        switch (choice) {
            case 0:
                if (confirm("Вы уверены, что хотите выйти?")) {
                    running = false;
                    std::cout << "До свидания!\n";
                }
                break;
            case 1:
                showContactList();
                if (!phoneBook.isEmpty()) {
                    if (confirm("Показать подробную информацию о контакте?")) {
                        int index = readInt("Введите номер контакта: ", 1, phoneBook.getContactCount()) - 1;
                        showContact(index);
                    }
                }
                pauseScreen();
                break;
            case 2:
                addContactMenu();
                pauseScreen();
                break;
            case 3:
                editContactMenu();
                pauseScreen();
                break;
            case 4:
                deleteContactMenu();
                pauseScreen();
                break;
            case 5:
                searchMenu();
                pauseScreen();
                break;
            case 6:
                sortMenu();
                pauseScreen();
                break;
            case 7:
                importExportMenu();
                pauseScreen();
                break;
        }
    }
}