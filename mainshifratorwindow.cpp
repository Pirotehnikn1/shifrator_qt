#include "mainshifratorwindow.h"
#include "ui_mainshifratorwindow.h"

#include <cmath>

#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QByteArray>
#include <QTextCodec>

MainShifratorWindow::MainShifratorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainShifratorWindow)
{
    // Установка графического интерфейса
    ui->setupUi(this);

    ui->teSourceText->setTextColor(QColor("red"));

    //Изначально все кнопки кодирования/декодирования и сохранения деактивированы
    ui->pbDecode->setEnabled(false);
    ui->pbEncode->setEnabled(false);
    ui->pbSaveDecoded->setEnabled(false);
    ui->pbSaveEncoded->setEnabled(false);
}

MainShifratorWindow::~MainShifratorWindow()
{
    // Удаление графического интерфейса
    delete ui;
}

QString MainShifratorWindow::encodeText(const QString sourceText, const QString keyText)
{
    //Конвертация исходного текста в массив байтов с кодировкой "Windows-1251"
    QByteArray sourceUnicode;
    {
      QTextStream streamer(&sourceUnicode);
      streamer.setLocale(QLocale(QLocale::Russian, QLocale::CyrillicScript, QLocale::Russia));
      streamer.setCodec("Windows-1251");
      streamer << sourceText;
    }

    //Конвертация ключа в массив байтов с кодировкой "Windows-1251"
    QByteArray keyUnicode;
    {
      QTextStream streamer(&keyUnicode);
      streamer.setCodec("Windows-1251");
      streamer << keyText;
    }


    QByteArray encodedUnicode;
    //Проход по всем байтам исходного текста
    for(int index = 0; index < sourceUnicode.size(); ++index)
    {
        //Индекс байта в ключе: если индекс больше размера ключа - начать сначала
        const int keyIndex =  index % keyUnicode.size();

        // Байт - источник
        const char sourceByte = sourceUnicode.at(index);

        // Байт ключа
        const char keyByte = keyUnicode.at(keyIndex);

        // Добавление результата операции XOR к массиву закодированных байтов
        encodedUnicode.append(sourceByte^keyByte);
    }

    // Конвертация массива закодированных байтов в шеснадцатиричное числовое представление
    // и возвращение результата
    return QString(encodedUnicode.toHex());
}

QString MainShifratorWindow::decodeText(const QString sourceText, const QString keyText)
{
    //Конвертация зашифрованного текста из строкового представления в массив байтов
    QByteArray sourceHex;
    {
      QTextStream streamer(&sourceHex);
      streamer << sourceText;
    }
    // Конвертация из шеснадцатиричного числового вида в символьный
    QByteArray sourceUnicode = QByteArray::fromHex(sourceHex);

    //Конвертация ключа в массив байтов с кодировкой "Windows-1251"
    QByteArray keyUnicode;
    {
      QTextStream streamer(&keyUnicode);
      streamer.setCodec("Windows-1251");
      streamer << keyText;
    }

    QByteArray decodedUnicode;
    //Проход по всем байтам зашифрованного текста
    for(int index = 0; index < sourceUnicode.size(); ++index)
    {
        //Индекс байта в ключе: если индекс больше размера ключа - начать сначала
        const int keyIndex =  index % keyUnicode.size();
        // Байт - источник
        const char sourceByte = sourceUnicode.at(index);
        // Байт ключа
        const char keyByte = keyUnicode.at(keyIndex);
        // Добавление результата операции XOR к массиву декодированных байтов
        decodedUnicode.append(sourceByte^keyByte);
    }

    //Конвертация массива байтов в текста в  с кодировкой "Windows-1251"
    QString result;
    {
      QTextStream streamer(&decodedUnicode);
      streamer.setLocale(QLocale(QLocale::Russian, QLocale::CyrillicScript, QLocale::Russia));
      streamer.setCodec("Windows-1251");
      result = streamer.readAll();
    }

    // Возвращение результата
    return result;
}

void MainShifratorWindow::on_pbEncode_clicked(bool)
{
    QString keyText;
    //Получение имени файла из поля
    const QString fileName = ui->leKeyFileName->text();

    //Попытка чтения ключа из файла
    if(readTextFromFile(fileName, keyText))
    {
        //Если успешно получение текста из поля исходного текста
        QTextDocument * const doc = ui->teSourceText->document();
        const QString textSource = doc->toPlainText();
        //Кодирование текста
        const QString resultString = encodeText(textSource, keyText);
        //Запись полученного шифра в поле зашифрованного текста
        QTextDocument * const docEncoded = ui->teEncodedText->document();
        docEncoded->setPlainText(resultString);
    }
    else
    {
        //В случае ошибки доступа или чтения файла вывести сообщение об ошибке
        QMessageBox::critical(this, "Ошибка чтения", QString("Чтение ключа из файла по адресу \n %1 \n не удалась").arg(fileName));
    }
}

void MainShifratorWindow::on_pbDecode_clicked(bool)
{
    QString keyText;
    //Получение имени файла из поля
    const QString fileName = ui->leKeyFileName->text();
    //Попытка чтения ключа из файла
    if(readTextFromFile(fileName, keyText))
    {
        //Если успешно получение текста из поля шифра текста
        QTextDocument * const doc = ui->teEncodedText->document();
        const QString textEncoded = doc->toPlainText();
        //Декодирование текста
        const QString resultString = decodeText(textEncoded, keyText);
        //Запись полученного шифра в поле дешифрованного текста
        QTextDocument * const docDecoded = ui->teDecodedText->document();
        docDecoded->setPlainText(resultString);
    }
    else
    {
        //В случае ошибки доступа или чтения файла вывести сообщение об ошибке
        QMessageBox::critical(this, "Ошибка чтения", QString("Чтение ключа из файла по адресу \n %1 \n не удалась").arg(fileName));
    }
}

void MainShifratorWindow::on_pbChooseKey_clicked(bool)
{
   //Открытие файлового диалога и получение имени файла
   const QString fileName = QFileDialog::getOpenFileName(this, "Выберите файл содержащий ключ");
   //Проверка полноты полученного имени
   if(!fileName.isEmpty())
   {
       // Установка полученного имени в соответствующее поле
       ui->leKeyFileName->setText(fileName);
       // Проверка на активирование кнопки кодирования
       ui->pbEncode->setEnabled(checkEncodeButtonActive());
   }
}

void MainShifratorWindow::on_teSourceText_textChanged()
{
  // Проверка на активирование кнопки кодирования
  ui->pbEncode->setEnabled(checkEncodeButtonActive());
}

void MainShifratorWindow::on_teEncodedText_textChanged()
{
    // Проверка наличия текста в поле зашифрованного текста
    QTextDocument * const doc = ui->teEncodedText->document();
    const QString textEncoded = doc->toPlainText();
    const bool encodedExists = !textEncoded.isEmpty();

    //Активирование/Деактивирование кнопок
    ui->pbDecode->setEnabled(encodedExists);
    ui->pbSaveEncoded->setEnabled(encodedExists);
}

void MainShifratorWindow::on_teDecodedText_textChanged()
{
    // Проверка наличия текста в поле расшифрованного текста
    QTextDocument * const doc = ui->teDecodedText->document();
    const QString textDecoded = doc->toPlainText();
    const bool encodedExists = !textDecoded.isEmpty();
    //Активирование/Деактивирование кнопоки сохранения
    ui->pbSaveDecoded->setEnabled(encodedExists);
}

bool MainShifratorWindow::checkEncodeButtonActive() const
{
    // Проверка наличия текста в поле исходного текста
    QTextDocument * const doc = ui->teSourceText->document();
    const QString textSource = doc->toPlainText();
    // Проверка наличия файла с ключом
    const QString keyFileName = ui->leKeyFileName->text();
    return (!textSource.isEmpty() && !keyFileName.isEmpty());
}


bool MainShifratorWindow::saveTextToFile(const QString fileName, const QString text)
{
    QFile fileToSave(fileName);
    //Проверка существования файла
    if(!fileToSave.exists()) return false;
    // Попытка открытия файла для чтения/записи с перезаписыванием
    if(!fileToSave.open(QIODevice::ReadWrite | QIODevice::Truncate)) return false;

    // Запись текста в файл
    QTextStream ts(&fileToSave);
    ts<<text;
    ts.flush();
    return true;
}

bool MainShifratorWindow::readTextFromFile(const QString fileName, QString & text)
{
    QFile fileToRead(fileName);
    //Проверка существования файла
    if(!fileToRead.exists()) return false;
    // Попытка открытия файла для чтения
    if(!fileToRead.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    // Чтение текста из файла
    QTextStream ts(&fileToRead);
    text=ts.readAll();
    return true;
}

void MainShifratorWindow::on_pbSaveEncoded_clicked(bool)
{
    //Открытие файлового диалога и получение имени файла
    const QString fileName = QFileDialog::getSaveFileName(this, "Сохранить шифр как...");
    //Проверка полноты полученного имени
    if(!fileName.isEmpty())
    {
        //Получение зашифрованного текста
        QTextDocument * const doc = ui->teEncodedText->document();
        const QString textEncoded = doc->toPlainText();
        // Попытка сохранения в файл
        if(!saveTextToFile(fileName, textEncoded))
        {
           //В случае ошибки доступа или записи файла вывести сообщение об ошибке
           QMessageBox::critical(this, "Ошибка записи", QString("Запись в файл по адресу \n %1 \n не удалась").arg(fileName));
        }
    }
}


void MainShifratorWindow::on_pbSaveDecoded_clicked(bool)
{
    //Открытие файлового диалога и получение имени файла
    const QString fileName = QFileDialog::getSaveFileName(this, "Сохранить дешифрованный текст как...");
    //Проверка полноты полученного имени
    if(!fileName.isEmpty())
    {
        //Получение расшифрованного текста
        QTextDocument * const doc = ui->teDecodedText->document();
        const QString textDecoded = doc->toPlainText();
        // Попытка сохранения в файл
        if(!saveTextToFile(fileName, textDecoded))
        {
           //В случае ошибки доступа или записи файла вывести сообщение об ошибке
           QMessageBox::critical(this, "Ошибка записи", QString("Запись в файл по адресу \n %1 \n не удалась").arg(fileName));
        }
    }
}

void MainShifratorWindow::on_pbClean_clicked(bool)
{
  //Очистка текстовых полей
  ui->teSourceText->clear();
  ui->teEncodedText->clear();
  ui->teDecodedText->clear();
}

