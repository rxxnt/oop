# Makefile для телефонного справочника

# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

# Имя исполняемого файла
TARGET = phonebook

# Исходные файлы
SOURCES = main.cpp Contact.cpp PhoneBook.cpp ConsoleUI.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Заголовочные файлы
HEADERS = Contact.h PhoneBook.h ConsoleUI.h

# Правило по умолчанию
all: $(TARGET)

# Сборка исполняемого файла
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

# Компиляция исходных файлов
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка временных файлов
clean:
	rm -f $(OBJECTS) $(TARGET)

# Запуск программы
run: $(TARGET)
	./$(TARGET)

# Полная пересборка
rebuild: clean all

.PHONY: all clean run rebuild