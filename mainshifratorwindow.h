#ifndef MAINSHIFRATORWINDOW_H
#define MAINSHIFRATORWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainShifratorWindow; }
QT_END_NAMESPACE


/* Класс основного виджета программы, реализующий
 * настройки интерфейса и обработку нажатия на кнопки управления
*/
class MainShifratorWindow : public QMainWindow
{
    Q_OBJECT

public:
    //Конструктор
    MainShifratorWindow(QWidget *parent = nullptr);
    //Деструктор
    ~MainShifratorWindow();

private slots:
    // Обработка нажатия кнопки "Закодировать"
    void on_pbEncode_clicked(bool);
    // Обработка нажатия кнопки "Декодировать"
    void on_pbDecode_clicked(bool);
    // Обработка нажатия кнопки выбора файла с ключом
    void on_pbChooseKey_clicked(bool);
    // Обработка нажатия кнопки сохранения закодированного сообщения
    void on_pbSaveEncoded_clicked(bool);
    // Обработка нажатия кнопки сохранения декодированного сообщения
    void on_pbSaveDecoded_clicked(bool);
    // Обработка нажатия кнопки "Очистить"
    void on_pbClean_clicked(bool);
    // Обработка изменения текста в поле исходного текста
    void on_teSourceText_textChanged();
    // Обработка изменения текста в поле закодированного текста
    void on_teEncodedText_textChanged();
    // Обработка изменения текста в поле декодированного текста
    void on_teDecodedText_textChanged();

private:
    // Проверка условий активирования кнопки "Закодировать"
    bool checkEncodeButtonActive() const;

    //Сохранение текста в файл по имени файла
    static bool saveTextToFile(const QString fileName, const QString text);

    //Чтение текста из файла по имени файла
    static bool readTextFromFile(const QString fileName, QString & text);

    // Кодирование текста с помощью ключа
    static QString encodeText(const QString sourceText, const QString keyText);

    // Декодирование текста с помощью ключа
    static QString decodeText(const QString sourceText, const QString keyText);

private:
    Ui::MainShifratorWindow *ui; // автогенерированная графическая форма
};
#endif // MAINSHIFRATORWINDOW_H
