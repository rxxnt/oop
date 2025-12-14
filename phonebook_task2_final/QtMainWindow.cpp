#include "QtMainWindow.h"
#include <QApplication>

QtMainWindow::QtMainWindow(const std::string& filename, QWidget* parent)
    : QMainWindow(parent), phoneBook(filename) {
    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    listWidget = new QListWidget(central);
    addButton = new QPushButton(QString::fromUtf8("Добавить"), central);
    editButton = new QPushButton(QString::fromUtf8("Редактировать"), central);
    deleteButton = new QPushButton(QString::fromUtf8("Удалить"), central);
    searchButton = new QPushButton(QString::fromUtf8("Поиск"), central);
    sortButton = new QPushButton(QString::fromUtf8("Сортировать"), central);
    importButton = new QPushButton(QString::fromUtf8("Импорт"), central);
    exportButton = new QPushButton(QString::fromUtf8("Экспорт"), central);
    QHBoxLayout* buttonsTop = new QHBoxLayout();
    buttonsTop->addWidget(addButton);
    buttonsTop->addWidget(editButton);
    buttonsTop->addWidget(deleteButton);
    QHBoxLayout* buttonsBottom = new QHBoxLayout();
    buttonsBottom->addWidget(searchButton);
    buttonsBottom->addWidget(sortButton);
    buttonsBottom->addWidget(importButton);
    buttonsBottom->addWidget(exportButton);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->addLayout(buttonsTop);
    mainLayout->addWidget(listWidget);
    mainLayout->addLayout(buttonsBottom);
    connect(addButton, &QPushButton::clicked, this, &QtMainWindow::addContact);
    connect(editButton, &QPushButton::clicked, this, &QtMainWindow::editSelectedContact);
    connect(deleteButton, &QPushButton::clicked, this, &QtMainWindow::deleteSelectedContact);
    connect(searchButton, &QPushButton::clicked, this, &QtMainWindow::searchContacts);
    connect(sortButton, &QPushButton::clicked, this, &QtMainWindow::sortContacts);
    connect(importButton, &QPushButton::clicked, this, &QtMainWindow::importFromFile);
    connect(exportButton, &QPushButton::clicked, this, &QtMainWindow::exportToFile);
    refreshList();
}

void QtMainWindow::refreshList() {
    listWidget->clear();
    auto contacts = phoneBook.getAllContacts();
    for (const auto& c : contacts) {
        listWidget->addItem(QString::fromStdString(c.toShortString()));
    }
}

int QtMainWindow::selectedIndex() const {
    auto item = listWidget->currentRow();
    return item;
}

Contact QtMainWindow::inputContact(Contact initial, bool fullInput) {
    Contact contact = initial;
    bool ok = false;
    QString firstName = QInputDialog::getText(this, QString::fromUtf8("Имя"), QString::fromUtf8("Имя:"), QLineEdit::Normal, QString::fromStdString(contact.getFirstName()), &ok);
    if (!ok || !contact.setFirstName(firstName.toStdString())) {
        throw std::runtime_error("invalid");
    }
    QString lastName = QInputDialog::getText(this, QString::fromUtf8("Фамилия"), QString::fromUtf8("Фамилия:"), QLineEdit::Normal, QString::fromStdString(contact.getLastName()), &ok);
    if (!ok || !contact.setLastName(lastName.toStdString())) {
        throw std::runtime_error("invalid");
    }
    QString email = QInputDialog::getText(this, QString::fromUtf8("Email"), QString::fromUtf8("Email:"), QLineEdit::Normal, QString::fromStdString(contact.getEmail()), &ok);
    if (!ok || !contact.setEmail(email.toStdString())) {
        throw std::runtime_error("invalid");
    }
    QString phone = QInputDialog::getText(this, QString::fromUtf8("Телефон"), QString::fromUtf8("Телефон:"), QLineEdit::Normal, ok ? "" : "", &ok);
    if (!initial.getPhoneNumbers().empty()) {
        phone = QString::fromStdString(initial.getPhoneNumbers()[0].number);
    }
    if (!ok || phone.isEmpty()) {
        throw std::runtime_error("invalid");
    }
    QStringList types;
    types << QString::fromUtf8("Рабочий") << QString::fromUtf8("Домашний") << QString::fromUtf8("Служебный") << QString::fromUtf8("Другой");
    QString typeSel = QInputDialog::getItem(this, QString::fromUtf8("Тип телефона"), QString::fromUtf8("Тип:"), types, 0, false, &ok);
    PhoneType type = PhoneType::OTHER;
    if (typeSel == QString::fromUtf8("Рабочий")) type = PhoneType::WORK;
    else if (typeSel == QString::fromUtf8("Домашний")) type = PhoneType::HOME;
    else if (typeSel == QString::fromUtf8("Служебный")) type = PhoneType::SERVICE;
    if (!contact.getPhoneNumbers().empty()) {
        contact.updatePhoneNumber(0, phone.toStdString(), type);
    } else {
        if (!contact.addPhoneNumber(phone.toStdString(), type)) {
            throw std::runtime_error("invalid");
        }
    }
    if (fullInput) {
        QString address = QInputDialog::getText(this, QString::fromUtf8("Адрес"), QString::fromUtf8("Адрес:"), QLineEdit::Normal, QString::fromStdString(contact.getAddress()), &ok);
        contact.setAddress(address.toStdString());
        QString birth = QInputDialog::getText(this, QString::fromUtf8("Дата рождения"), QString::fromUtf8("ДД.ММ.ГГГГ:"), QLineEdit::Normal, "", &ok);
        if (ok && !birth.isEmpty()) {
            Date d;
            if (d.fromString(birth.toStdString())) {
                contact.setBirthDate(d);
            }
        }
    }
    return contact;
}

void QtMainWindow::addContact() {
    try {
        Contact c = inputContact(Contact(), true);
        if (phoneBook.addContact(c)) {
            refreshList();
        } else {
            QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Не удалось добавить контакт"));
        }
    } catch (...) {
        QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Неверные данные"));
    }
}

void QtMainWindow::editSelectedContact() {
    int idx = selectedIndex();
    if (idx < 0 || idx >= static_cast<int>(phoneBook.getContactCount())) return;
    const Contact* current = phoneBook.getContact(idx);
    if (!current) return;
    try {
        Contact edited = inputContact(*current, true);
        if (phoneBook.updateContact(idx, edited)) {
            refreshList();
        } else {
            QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Не удалось обновить контакт"));
        }
    } catch (...) {
        QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Неверные данные"));
    }
}

void QtMainWindow::deleteSelectedContact() {
    int idx = selectedIndex();
    if (idx < 0 || idx >= static_cast<int>(phoneBook.getContactCount())) return;
    if (QMessageBox::question(this, QString::fromUtf8("Подтверждение"), QString::fromUtf8("Удалить выбранный контакт?")) == QMessageBox::Yes) {
        if (phoneBook.removeContact(idx)) {
            refreshList();
        } else {
            QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Не удалось удалить контакт"));
        }
    }
}

void QtMainWindow::searchContacts() {
    bool ok = false;
    QString query = QInputDialog::getText(this, QString::fromUtf8("Поиск"), QString::fromUtf8("Запрос:"), QLineEdit::Normal, "", &ok);
    if (!ok) return;
    auto idxs = phoneBook.searchMultiField(query.toStdString());
    listWidget->clear();
    auto all = phoneBook.getAllContacts();
    for (auto i : idxs) {
        listWidget->addItem(QString::fromStdString(all[i].toShortString()));
    }
}

void QtMainWindow::sortContacts() {
    QStringList fields;
    fields << QString::fromUtf8("Имя") << QString::fromUtf8("Фамилия") << QString::fromUtf8("Email") << QString::fromUtf8("Дата рождения");
    bool ok = false;
    QString field = QInputDialog::getItem(this, QString::fromUtf8("Сортировка"), QString::fromUtf8("Поле:"), fields, 1, false, &ok);
    if (!ok) return;
    QStringList orders;
    orders << QString::fromUtf8("По возрастанию") << QString::fromUtf8("По убыванию");
    QString order = QInputDialog::getItem(this, QString::fromUtf8("Сортировка"), QString::fromUtf8("Порядок:"), orders, 0, false, &ok);
    SortField f = SortField::LAST_NAME;
    if (field == QString::fromUtf8("Имя")) f = SortField::FIRST_NAME;
    else if (field == QString::fromUtf8("Фамилия")) f = SortField::LAST_NAME;
    else if (field == QString::fromUtf8("Email")) f = SortField::EMAIL;
    else if (field == QString::fromUtf8("Дата рождения")) f = SortField::BIRTH_DATE;
    SortOrder o = (order == QString::fromUtf8("По возрастанию")) ? SortOrder::ASCENDING : SortOrder::DESCENDING;
    phoneBook.sortContacts(f, o);
    refreshList();
}

void QtMainWindow::importFromFile() {
    QString path = QFileDialog::getOpenFileName(this, QString::fromUtf8("Импорт из файла"));
    if (path.isEmpty()) return;
    if (phoneBook.importFromFile(path.toStdString())) {
        refreshList();
        QMessageBox::information(this, QString::fromUtf8("Импорт"), QString::fromUtf8("Данные импортированы"));
    } else {
        QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Не удалось импортировать"));
    }
}

void QtMainWindow::exportToFile() {
    QString path = QFileDialog::getSaveFileName(this, QString::fromUtf8("Экспорт в файл"));
    if (path.isEmpty()) return;
    if (phoneBook.exportToFile(path.toStdString())) {
        QMessageBox::information(this, QString::fromUtf8("Экспорт"), QString::fromUtf8("Данные экспортированы"));
    } else {
        QMessageBox::warning(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Не удалось экспортировать"));
    }
}
