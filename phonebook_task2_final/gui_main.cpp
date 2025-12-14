#include <QApplication>
#include <QTextCodec>
#include "QtMainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Настройка кодировки для правильного отображения русского текста
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    #endif
    
    std::string filename = "phonebook.txt";
    if (argc > 1) {
        filename = argv[1];
    }
    QtMainWindow w(filename);
    w.resize(800, 600);
    w.show();
    return app.exec();
}
