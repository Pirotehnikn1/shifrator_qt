#include "mainshifratorwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // Создание приложения
    QApplication a(argc, argv);
    // Установка русской локали по-умолчанию
    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::Script::CyrillicScript, QLocale::Russia));

    // Создание главного окна
    MainShifratorWindow w;

    //Показ главного окна
    w.show();

    // Запуск главного цикла обработки событий
    return a.exec();
}
