#ifndef CONTACT_H
#define CONTACT_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <regex>
#include <ctime>
#include <algorithm>

enum class PhoneType {
    WORK,
    HOME,
    SERVICE,
    OTHER
};

struct PhoneNumber {
    std::string number;
    PhoneType type;
    
    PhoneNumber(const std::string& num = "", PhoneType t = PhoneType::OTHER) 
        : number(num), type(t) {}
};

struct Date {
    int day;
    int month;
    int year;
    
    Date() : day(1), month(1), year(2000) {}
    Date(int d, int m, int y) : day(d), month(m), year(y) {}
    
    std::string toString() const;
    bool fromString(const std::string& str);
    bool isValid() const;
    bool isLeapYear(int year) const;
};

class Contact {
private:
    std::string firstName;
    std::string lastName;
    std::string patronymic;
    std::string address;
    Date birthDate;
    std::string email;
    std::vector<PhoneNumber> phoneNumbers;
    
    // Вспомогательные методы для валидации
    static std::string trim(const std::string& str);
    static bool validateName(const std::string& name);
    static bool validateEmail(const std::string& email);
    static bool validatePhone(const std::string& phone);
    static std::string normalizePhone(const std::string& phone);
    
public:
    Contact();
    Contact(const std::string& fName, const std::string& lName, 
            const std::string& mail, const std::string& phone);
    
    // Геттеры
    std::string getFirstName() const { return firstName; }
    std::string getLastName() const { return lastName; }
    std::string getPatronymic() const { return patronymic; }
    std::string getAddress() const { return address; }
    Date getBirthDate() const { return birthDate; }
    std::string getEmail() const { return email; }
    std::vector<PhoneNumber> getPhoneNumbers() const { return phoneNumbers; }
    
    // Сеттеры с валидацией
    bool setFirstName(const std::string& name);
    bool setLastName(const std::string& name);
    bool setPatronymic(const std::string& name);
    bool setAddress(const std::string& addr);
    bool setBirthDate(const Date& date);
    bool setBirthDate(int day, int month, int year);
    bool setEmail(const std::string& mail);
    bool addPhoneNumber(const std::string& phone, PhoneType type = PhoneType::OTHER);
    bool removePhoneNumber(size_t index);
    bool updatePhoneNumber(size_t index, const std::string& phone, PhoneType type);
    
    // Методы для сериализации/десериализации
    std::string serialize() const;
    bool deserialize(const std::string& data);
    
    // Методы для отображения
    std::string toString() const;
    std::string toShortString() const;
    
    // Операторы сравнения для сортировки
    bool operator<(const Contact& other) const;
    bool operator==(const Contact& other) const;
};

#endif // CONTACT_H