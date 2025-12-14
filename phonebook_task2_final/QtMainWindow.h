#ifndef QTMAINWINDOW_H
#define QTMAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include "PhoneBook.h"

class QtMainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit QtMainWindow(const std::string& filename = "phonebook.txt", QWidget* parent = nullptr);
private slots:
    void refreshList();
    void addContact();
    void editSelectedContact();
    void deleteSelectedContact();
    void searchContacts();
    void sortContacts();
    void importFromFile();
    void exportToFile();
private:
    PhoneBook phoneBook;
    QListWidget* listWidget;
    QPushButton* addButton;
    QPushButton* editButton;
    QPushButton* deleteButton;
    QPushButton* searchButton;
    QPushButton* sortButton;
    QPushButton* importButton;
    QPushButton* exportButton;
    int selectedIndex() const;
    Contact inputContact(Contact initial = Contact(), bool fullInput = true);
};

#endif
