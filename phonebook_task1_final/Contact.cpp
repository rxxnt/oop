#include "Contact.h"
#include <iomanip>
#include <cctype>

// Реализация методов структуры Date
std::string Date::toString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << day << "."
        << std::setfill('0') << std::setw(2) << month << "."
        << year;
    return oss.str();
}

bool Date::fromString(const std::string& str) {
    std::regex dateRegex(R"((\d{1,2})\.(\d{1,2})\.(\d{4}))");
    std::smatch match;
    
    if (std::regex_match(str, match, dateRegex)) {
        day = std::stoi(match[1]);
        month = std::stoi(match[2]);
        year = std::stoi(match[3]);
        return isValid();
    }
    return false;
}

bool Date::isLeapYear(int year) const {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool Date::isValid() const {
    // Проверка года (должен быть меньше текущего)
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    int currentYear = now->tm_year + 1900;
    int currentMonth = now->tm_mon + 1;
    int currentDay = now->tm_mday;
    
    if (year < 1900 || year > currentYear) return false;
    if (year == currentYear && month > currentMonth) return false;
    if (year == currentYear && month == currentMonth && day > currentDay) return false;
    
    // Проверка месяца
    if (month < 1 || month > 12) return false;
    
    // Проверка дня в зависимости от месяца
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Проверка на високосный год для февраля
    if (month == 2 && isLeapYear(year)) {
        daysInMonth[1] = 29;
    }
    
    if (day < 1 || day > daysInMonth[month - 1]) return false;
    
    return true;
}

// Реализация методов класса Contact
Contact::Contact() : firstName(""), lastName(""), patronymic(""), address(""), email("") {}

Contact::Contact(const std::string& fName, const std::string& lName, 
                 const std::string& mail, const std::string& phone) {
    if (!setFirstName(fName)) {
        throw std::invalid_argument("Invalid first name");
    }
    if (!setLastName(lName)) {
        throw std::invalid_argument("Invalid last name");
    }
    if (!setEmail(mail)) {
        throw std::invalid_argument("Invalid email");
    }
    if (!addPhoneNumber(phone)) {
        throw std::invalid_argument("Invalid phone number");
    }
}

std::string Contact::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool Contact::validateName(const std::string& name) {
    if (name.empty()) return false;
    
    std::string trimmedName = trim(name);
    if (trimmedName.empty()) return false;
    
    // Проверка что имя не начинается и не заканчивается на дефис
    if (trimmedName[0] == '-' || trimmedName[trimmedName.length() - 1] == '-') return false;
    
    // Проверка что первый символ - буква (латинская или кириллица)
    unsigned char firstChar = static_cast<unsigned char>(trimmedName[0]);
    
    // Проверяем, что первый символ - это буква
    // Для ASCII
    if (firstChar < 128) {
        if (!std::isalpha(firstChar)) {
            return false;
        }
    } else {
        // Для UTF-8 кириллицы (первый байт русской буквы в UTF-8)
        // Русские буквы в UTF-8 начинаются с 0xD0 или 0xD1
        if (firstChar != 0xD0 && firstChar != 0xD1) {
            return false;
        }
    }
    
    // Проверка допустимых символов в имени
    size_t i = 0;
    while (i < trimmedName.length()) {
        unsigned char c = static_cast<unsigned char>(trimmedName[i]);
        
        if (c < 128) {
            // ASCII символ - проверяем, что это буква, цифра, пробел или дефис
            if (!std::isalnum(c) && c != ' ' && c != '-') {
                return false;
            }
            i++;
        } else if (c == 0xD0 || c == 0xD1) {
            // Начало русской буквы в UTF-8
            // Проверяем, что есть следующий байт
            if (i + 1 >= trimmedName.length()) {
                return false;
            }
            // Пропускаем два байта (русская буква в UTF-8)
            i += 2;
        } else {
            // Недопустимый символ
            return false;
        }
    }
    
    return true;
}

bool Contact::validateEmail(const std::string& email) {
    std::string trimmedEmail = trim(email);
    
    // Регулярное выражение для email
    std::regex emailRegex(R"(^[a-zA-Z0-9]+@[a-zA-Z0-9]+(\.[a-zA-Z0-9]+)*$)");
    return std::regex_match(trimmedEmail, emailRegex);
}

bool Contact::validatePhone(const std::string& phone) {
    // Регулярные выражения для разных форматов телефона
    std::vector<std::regex> phoneRegexes = {
        std::regex(R"(^\+7\d{10}$)"),                    // +78121234567
        std::regex(R"(^8\d{10}$)"),                      // 88121234567
        std::regex(R"(^\+7\(\d{3}\)\d{7}$)"),           // +7(812)1234567
        std::regex(R"(^8\(\d{3}\)\d{7}$)"),             // 8(812)1234567
        std::regex(R"(^\+7\(\d{3}\)\d{3}-\d{2}-\d{2}$)"), // +7(812)123-45-67
        std::regex(R"(^8\(\d{3}\)\d{3}-\d{2}-\d{2}$)")    // 8(812)123-45-67
    };
    
    for (const auto& regex : phoneRegexes) {
        if (std::regex_match(phone, regex)) {
            return true;
        }
    }
    
    return false;
}

std::string Contact::normalizePhone(const std::string& phone) {
    std::string normalized = phone;
    
    // Удаляем все нецифровые символы кроме +
    std::string result;
    for (char c : normalized) {
        if (std::isdigit(c) || c == '+') {
            result += c;
        }
    }
    
    // Приводим к единому формату +7XXXXXXXXXX
    if (result[0] == '8') {
        result = "+7" + result.substr(1);
    } else if (result[0] != '+') {
        result = "+7" + result;
    }
    
    return result;
}

bool Contact::setFirstName(const std::string& name) {
    std::string trimmedName = trim(name);
    if (validateName(trimmedName)) {
        firstName = trimmedName;
        return true;
    }
    return false;
}

bool Contact::setLastName(const std::string& name) {
    std::string trimmedName = trim(name);
    if (validateName(trimmedName)) {
        lastName = trimmedName;
        return true;
    }
    return false;
}

bool Contact::setPatronymic(const std::string& name) {
    if (name.empty()) {
        patronymic = "";
        return true;
    }
    std::string trimmedName = trim(name);
    if (validateName(trimmedName)) {
        patronymic = trimmedName;
        return true;
    }
    return false;
}

bool Contact::setAddress(const std::string& addr) {
    address = trim(addr);
    return true;
}

bool Contact::setBirthDate(const Date& date) {
    if (date.isValid()) {
        birthDate = date;
        return true;
    }
    return false;
}

bool Contact::setBirthDate(int day, int month, int year) {
    Date date(day, month, year);
    return setBirthDate(date);
}

bool Contact::setEmail(const std::string& mail) {
    std::string trimmedEmail = trim(mail);
    if (validateEmail(trimmedEmail)) {
        email = trimmedEmail;
        return true;
    }
    return false;
}

bool Contact::addPhoneNumber(const std::string& phone, PhoneType type) {
    if (validatePhone(phone)) {
        phoneNumbers.push_back(PhoneNumber(normalizePhone(phone), type));
        return true;
    }
    return false;
}

bool Contact::removePhoneNumber(size_t index) {
    if (index < phoneNumbers.size() && phoneNumbers.size() > 1) {
        phoneNumbers.erase(phoneNumbers.begin() + index);
        return true;
    }
    return false;
}

bool Contact::updatePhoneNumber(size_t index, const std::string& phone, PhoneType type) {
    if (index < phoneNumbers.size() && validatePhone(phone)) {
        phoneNumbers[index] = PhoneNumber(normalizePhone(phone), type);
        return true;
    }
    return false;
}

std::string Contact::serialize() const {
    std::ostringstream oss;
    oss << firstName << "|" << lastName << "|" << patronymic << "|"
        << address << "|" << birthDate.toString() << "|" << email << "|";
    
    oss << phoneNumbers.size() << "|";
    for (const auto& phone : phoneNumbers) {
        oss << phone.number << "," << static_cast<int>(phone.type) << "|";
    }
    
    return oss.str();
}

bool Contact::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string token;
    std::vector<std::string> tokens;
    
    while (std::getline(iss, token, '|')) {
        tokens.push_back(token);
    }
    
    if (tokens.size() < 7) return false;
    
    firstName = tokens[0];
    lastName = tokens[1];
    patronymic = tokens[2];
    address = tokens[3];
    birthDate.fromString(tokens[4]);
    email = tokens[5];
    
    size_t phoneCount = std::stoi(tokens[6]);
    phoneNumbers.clear();
    
    for (size_t i = 0; i < phoneCount && (7 + i) < tokens.size(); ++i) {
        std::istringstream phoneStream(tokens[7 + i]);
        std::string phoneNum;
        std::string typeStr;
        
        std::getline(phoneStream, phoneNum, ',');
        std::getline(phoneStream, typeStr, ',');
        
        PhoneType type = static_cast<PhoneType>(std::stoi(typeStr));
        phoneNumbers.push_back(PhoneNumber(phoneNum, type));
    }
    
    return true;
}

std::string Contact::toString() const {
    std::ostringstream oss;
    oss << "ФИО: " << lastName << " " << firstName;
    if (!patronymic.empty()) oss << " " << patronymic;
    oss << "\n";
    
    if (!address.empty()) {
        oss << "Адрес: " << address << "\n";
    }
    
    oss << "Дата рождения: " << birthDate.toString() << "\n";
    oss << "Email: " << email << "\n";
    oss << "Телефоны:\n";
    
    for (const auto& phone : phoneNumbers) {
        oss << "  ";
        switch (phone.type) {
            case PhoneType::WORK: oss << "[Рабочий] "; break;
            case PhoneType::HOME: oss << "[Домашний] "; break;
            case PhoneType::SERVICE: oss << "[Служебный] "; break;
            case PhoneType::OTHER: oss << "[Другой] "; break;
        }
        oss << phone.number << "\n";
    }
    
    return oss.str();
}

std::string Contact::toShortString() const {
    std::ostringstream oss;
    oss << lastName << " " << firstName;
    if (!patronymic.empty()) oss << " " << patronymic;
    oss << " | " << email;
    if (!phoneNumbers.empty()) {
        oss << " | " << phoneNumbers[0].number;
    }
    return oss.str();
}

bool Contact::operator<(const Contact& other) const {
    if (lastName != other.lastName) return lastName < other.lastName;
    if (firstName != other.firstName) return firstName < other.firstName;
    return patronymic < other.patronymic;
}

bool Contact::operator==(const Contact& other) const {
    return lastName == other.lastName && 
           firstName == other.firstName && 
           email == other.email;
}