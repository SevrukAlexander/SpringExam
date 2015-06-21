#include <QtGui>
#include "phonebook.h"
#include <QApplication>

 int main(int argc, char *argv[])
 {
     QApplication app(argc, argv);

     PhoneBook *phoneBook = new PhoneBook;
     PhoneBook->show();

     return app.exec();
 }
