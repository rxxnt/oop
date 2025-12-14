#include "PhoneBook.h"
#include <algorithm>
#include <iostream>
#include <set>
#include <QFile>
#include <QTextStream>
#include <QString>

PhoneBook::PhoneBook(const std::string& file) : fileName(file) {
    loadFromFile();
}

PhoneBook::~PhoneBook() {
    saveToFile();
}

bool PhoneBook::loadFromFile() {
    QFile file(QString::fromStdString(fileName));
    if (!file.exists()) {
        return true;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    contacts.clear();
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString qline = in.readLine();
        std::string line = qline.toStdString();
        if (!line.empty()) {
            Contact contact;
            if (contact.deserialize(line)) {
                contacts.push_back(contact);
            }
        }
    }
    file.close();
    return true;
}

bool PhoneBook::saveToFile() const {
    QFile file(QString::fromStdString(fileName));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        std::cerr << "Ошибка: не удалось открыть файл для записи: " << fileName << std::endl;
        return false;
    }
    QTextStream out(&file);
    for (const auto& contact : contacts) {
        out << QString::fromStdString(contact.serialize()) << "\n";
    }
    file.close();
    return true;
}

bool PhoneBook::addContact(const Contact& contact) {
    // Проверка на дубликат
    auto it = std::find(contacts.begin(), contacts.end(), contact);
    if (it != contacts.end()) {
        std::cerr << "Контакт уже существует!" << std::endl;
        return false;
    }
    
    contacts.push_back(contact);
    return saveToFile();
}

bool PhoneBook::removeContact(size_t index) {
    if (index >= contacts.size()) {
        return false;
    }
    
    contacts.erase(contacts.begin() + index);
    return saveToFile();
}

bool PhoneBook::updateContact(size_t index, const Contact& contact) {
    if (index >= contacts.size()) {
        return false;
    }
    
    contacts[index] = contact;
    return saveToFile();
}

Contact* PhoneBook::getContact(size_t index) {
    if (index >= contacts.size()) {
        return nullptr;
    }
    return &contacts[index];
}

const Contact* PhoneBook::getContact(size_t index) const {
    if (index >= contacts.size()) {
        return nullptr;
    }
    return &contacts[index];
}

std::vector<Contact> PhoneBook::getAllContacts() const {
    return contacts;
}

size_t PhoneBook::getContactCount() const {
    return contacts.size();
}

std::vector<size_t> PhoneBook::searchByName(const std::string& query) const {
    std::vector<size_t> results;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (size_t i = 0; i < contacts.size(); ++i) {
        std::string fullName = contacts[i].getLastName() + " " + 
                               contacts[i].getFirstName() + " " + 
                               contacts[i].getPatronymic();
        std::transform(fullName.begin(), fullName.end(), fullName.begin(), ::tolower);
        
        if (fullName.find(lowerQuery) != std::string::npos) {
            results.push_back(i);
        }
    }
    
    return results;
}

std::vector<size_t> PhoneBook::searchByEmail(const std::string& query) const {
    std::vector<size_t> results;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (size_t i = 0; i < contacts.size(); ++i) {
        std::string email = contacts[i].getEmail();
        std::transform(email.begin(), email.end(), email.begin(), ::tolower);
        
        if (email.find(lowerQuery) != std::string::npos) {
            results.push_back(i);
        }
    }
    
    return results;
}

std::vector<size_t> PhoneBook::searchByPhone(const std::string& query) const {
    std::vector<size_t> results;
    
    for (size_t i = 0; i < contacts.size(); ++i) {
        const auto& phones = contacts[i].getPhoneNumbers();
        for (const auto& phone : phones) {
            if (phone.number.find(query) != std::string::npos) {
                results.push_back(i);
                break;
            }
        }
    }
    
    return results;
}

std::vector<size_t> PhoneBook::searchMultiField(const std::string& query) const {
    std::vector<size_t> results;
    std::set<size_t> uniqueResults;
    
    // Поиск по имени
    auto nameResults = searchByName(query);
    uniqueResults.insert(nameResults.begin(), nameResults.end());
    
    // Поиск по email
    auto emailResults = searchByEmail(query);
    uniqueResults.insert(emailResults.begin(), emailResults.end());
    
    // Поиск по телефону
    auto phoneResults = searchByPhone(query);
    uniqueResults.insert(phoneResults.begin(), phoneResults.end());
    
    // Поиск по адресу
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (size_t i = 0; i < contacts.size(); ++i) {
        std::string address = contacts[i].getAddress();
        std::transform(address.begin(), address.end(), address.begin(), ::tolower);
        
        if (address.find(lowerQuery) != std::string::npos) {
            uniqueResults.insert(i);
        }
    }
    
    results.assign(uniqueResults.begin(), uniqueResults.end());
    return results;
}

void PhoneBook::sortContacts(SortField field, SortOrder order) {
    std::sort(contacts.begin(), contacts.end(), 
        [field, order](const Contact& a, const Contact& b) {
            bool less = false;
            
            switch (field) {
                case SortField::FIRST_NAME:
                    less = a.getFirstName() < b.getFirstName();
                    break;
                case SortField::LAST_NAME:
                    less = a.getLastName() < b.getLastName();
                    break;
                case SortField::EMAIL:
                    less = a.getEmail() < b.getEmail();
                    break;
                case SortField::BIRTH_DATE: {
                    Date dateA = a.getBirthDate();
                    Date dateB = b.getBirthDate();
                    if (dateA.year != dateB.year) less = dateA.year < dateB.year;
                    else if (dateA.month != dateB.month) less = dateA.month < dateB.month;
                    else less = dateA.day < dateB.day;
                    break;
                }
            }
            
            return (order == SortOrder::ASCENDING) ? less : !less;
        });
    
    saveToFile();
}

bool PhoneBook::save() const {
    return saveToFile();
}

bool PhoneBook::reload() {
    return loadFromFile();
}

bool PhoneBook::exportToFile(const std::string& filename) const {
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    for (const auto& contact : contacts) {
        out << QString::fromStdString(contact.serialize()) << "\n";
    }
    file.close();
    return true;
}

bool PhoneBook::importFromFile(const std::string& filename) {
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream in(&file);
    std::vector<Contact> newContacts;
    while (!in.atEnd()) {
        QString qline = in.readLine();
        std::string line = qline.toStdString();
        if (!line.empty()) {
            Contact contact;
            if (contact.deserialize(line)) {
                newContacts.push_back(contact);
            }
        }
    }
    file.close();
    for (const auto& contact : newContacts) {
        addContact(contact);
    }
    return true;
}

void PhoneBook::clear() {
    contacts.clear();
    saveToFile();
}

bool PhoneBook::isEmpty() const {
    return contacts.empty();
}
