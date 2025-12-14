QT += widgets core
CONFIG += c++11
TEMPLATE = app
TARGET = phonebook_gui

SOURCES += \
    gui_main.cpp \
    QtMainWindow.cpp \
    Contact.cpp \
    PhoneBook.cpp

HEADERS += \
    QtMainWindow.h \
    Contact.h \
    PhoneBook.h

