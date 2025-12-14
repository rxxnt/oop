#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include "Contact.h"
#include <vector>
#include <string>
#include <memory>
#include <functional>

enum class SortField {
    FIRST_NAME,
    LAST_NAME,
    EMAIL,
    BIRTH_DATE
};

enum class SortOrder {
    ASCENDING,
    DESCENDING
};

class PhoneBook {
private:
    std::vector<Contact> contacts;
    std::string fileName;
    
    bool loadFromFile();
    bool saveToFile() const;
    
public:
    PhoneBook(const std::string& file = "phonebook.txt");
    ~PhoneBook();
    
    // Основные операции
    bool addContact(const Contact& contact);
    bool removeContact(size_t index);
    bool updateContact(size_t index, const Contact& contact);
    
    // Получение данных
    Contact* getContact(size_t index);
    const Contact* getContact(size_t index) const;
    std::vector<Contact> getAllContacts() const;
    size_t getContactCount() const;
    
    // Поиск
    std::vector<size_t> searchByName(const std::string& query) const;
    std::vector<size_t> searchByEmail(const std::string& query) const;
    std::vector<size_t> searchByPhone(const std::string& query) const;
    std::vector<size_t> searchMultiField(const std::string& query) const;
    
    // Сортировка
    void sortContacts(SortField field, SortOrder order = SortOrder::ASCENDING);
    
    // Работа с файлами
    bool save() const;
    bool reload();
    bool exportToFile(const std::string& filename) const;
    bool importFromFile(const std::string& filename);
    
    // Вспомогательные методы
    void clear();
    bool isEmpty() const;
};

#endif // PHONEBOOK_H