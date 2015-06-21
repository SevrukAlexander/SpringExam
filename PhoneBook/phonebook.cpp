#include <QtGui>
 #include "phonebook.h"

 PhoneBook::PhoneBook(QWidget *parent)
     : QWidget(parent)
 {
     QLabel *nameLabel = new QLabel(tr("Name:"));
     nameLine = new QLineEdit;
     nameLine->setReadOnly(true);

     QLabel *phoneLabel = new QLabel(tr("Phone:"));
     phoneText = new QTextEdit;
     phoneText->setReadOnly(true);

     addButton = new QPushButton(tr("&Add"));

     editButton = new QPushButton(tr("&Edit"));
     editButton->setEnabled(false);
     removeButton = new QPushButton(tr("&Remove"));
     removeButton->setEnabled(false);
     findButton = new QPushButton(tr("&Find"));
     findButton->setEnabled(false);
     submitButton = new QPushButton(tr("&Submit"));
     submitButton->hide();
     cancelButton = new QPushButton(tr("&Cancel"));
     cancelButton->hide();

     nextButton = new QPushButton(tr("&Next"));
     nextButton->setEnabled(false);
     previousButton = new QPushButton(tr("&Previous"));
     previousButton->setEnabled(false);

     loadButton = new QPushButton(tr("&Load..."));
     loadButton->setToolTip(tr("Load contacts from a file"));
     saveButton = new QPushButton(tr("Sa&ve..."));
     saveButton->setToolTip(tr("Save contacts to a file"));
     saveButton->setEnabled(false);

     dialog = new FindDialog;

     connect(addButton, SIGNAL(clicked()), this, SLOT(addContact()));
     connect(submitButton, SIGNAL(clicked()), this, SLOT(submitContact()));
     connect(editButton, SIGNAL(clicked()), this, SLOT(editContact()));
     connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
     connect(removeButton, SIGNAL(clicked()), this, SLOT(removeContact()));
     connect(findButton, SIGNAL(clicked()), this, SLOT(findContact()));
     connect(nextButton, SIGNAL(clicked()), this, SLOT(next()));
     connect(previousButton, SIGNAL(clicked()), this, SLOT(previous()));
     connect(loadButton, SIGNAL(clicked()), this, SLOT(loadFromFile()));
     connect(saveButton, SIGNAL(clicked()), this, SLOT(saveToFile()));

     QVBoxLayout *buttonLayout1 = new QVBoxLayout;
     buttonLayout1->addWidget(addButton);
     buttonLayout1->addWidget(editButton);
     buttonLayout1->addWidget(removeButton);
     buttonLayout1->addWidget(findButton);
     buttonLayout1->addWidget(submitButton);
     buttonLayout1->addWidget(cancelButton);
     buttonLayout1->addWidget(loadButton);
     buttonLayout1->addWidget(saveButton);
     buttonLayout1->addStretch();

     QHBoxLayout *buttonLayout2 = new QHBoxLayout;
     buttonLayout2->addWidget(previousButton);
     buttonLayout2->addWidget(nextButton);

     QGridLayout *mainLayout = new QGridLayout;
     mainLayout->addWidget(nameLabel, 0, 0);
     mainLayout->addWidget(nameLine, 0, 1);
     mainLayout->addWidget(phoneLabel, 1, 0, Qt::AlignTop);
     mainLayout->addWidget(phoneText, 1, 1);
     mainLayout->addLayout(buttonLayout1, 1, 2);
     mainLayout->addLayout(buttonLayout2, 2, 1);

     setLayout(mainLayout);
     setWindowTitle(tr("Phone Book"));
 }

 void AddressBook::addContact()
 {
     oldName = nameLine->text();
     oldPhone = phoneText->toPlainText();

     nameLine->clear();
     phoneText->clear();

     updateInterface(AddingMode);
 }

 void AddressBook::editContact()
 {
     oldName = nameLine->text();
     oldPhone = phoneText->toPlainText();

     updateInterface(EditingMode);
 }

 void AddressBook::submitContact()
 {
     QString name = nameLine->text();
     QString phone = phoneText->toPlainText();

     if (name == "" || phone == "") {
         QMessageBox::information(this, tr("Empty Field"),
             tr("Please enter a name and phone."));
         return;
     }

     if (currentMode == AddingMode) {

         if (!contacts.contains(name)) {
             contacts.insert(name, phone);
             QMessageBox::information(this, tr("Add Successful"),
                 tr("\"%1\" has been added to your phone book.").arg(name));
         } else {
             QMessageBox::information(this, tr("Add Unsuccessful"),
                 tr("Sorry, \"%1\" is already in your phone book.").arg(name));
             return;
         }
     } else if (currentMode == EditingMode) {

         if (oldName != name) {
             if (!contacts.contains(name)) {
                 QMessageBox::information(this, tr("Edit Successful"),
                     tr("\"%1\" has been edited in your phone book.").arg(oldName));
                 contacts.remove(oldName);
                 contacts.insert(name, phone);
             } else {
                 QMessageBox::information(this, tr("Edit Unsuccessful"),
                     tr("Sorry, \"%1\" is already in your phone book.").arg(name));
                 return;
             }
         } else if (oldPhone != phone) {
             QMessageBox::information(this, tr("Edit Successful"),
                 tr("\"%1\" has been edited in your phone book.").arg(name));
             contacts[name] = phone;
         }
     }

     updateInterface(NavigationMode);
 }

 void PhoneBook::cancel()
 {
     nameLine->setText(oldName);
     phoneText->setText(oldPhone);
     updateInterface(NavigationwMode);
 }

 void PhoneBook::removeContact()
 {
     QString name = nameLine->text();
     QString phone = phoneText->toPlainText();

     if (contacts.contains(name)) {

         int button = QMessageBox::question(this,
             tr("Confirm Remove"),
             tr("Are you sure you want to remove \"%1\"?").arg(name),
             QMessageBox::Yes | QMessageBox::No);

         if (button == QMessageBox::Yes) {

             previous();
             contacts.remove(name);

             QMessageBox::information(this, tr("Remove Successful"),
                 tr("\"%1\" has been removed from your phone book.").arg(name));
         }
     }

     updateInterface(NavigationMode);
 }

 void PhoneBook::next()
 {
     QString name = nameLine->text();
     QMap<QString, QString>::iterator i = contacts.find(name);

     if (i != contacts.end())
         i++;

     if (i == contacts.end())
         i = contacts.begin();

     nameLine->setText(i.key());
     addressText->setText(i.value());
 }

 void PhoneBook::previous()
 {
     QString name = nameLine->text();
     QMap<QString, QString>::iterator i = contacts.find(name);

     if (i == contacts.end()) {
         nameLine->clear();
         phoneText->clear();
         return;
     }

     if (i == contacts.begin())
         i = contacts.end();

     i--;
     nameLine->setText(i.key());
     phoneText->setText(i.value());
 }

 void PhoneBook::findContact()
 {
     dialog->show();

     if (dialog->exec() == 1) {
         QString contactName = dialog->getFindText();

         if (contacts.contains(contactName)) {
             nameLine->setText(contactName);
             phoneText->setText(contacts.value(contactName));
         } else {
             QMessageBox::information(this, tr("Contact Not Found"),
                 tr("Sorry, \"%1\" is not in your phone book.").arg(contactName));
             return;
         }
     }

     updateInterface(NavigationMode);
 }

 void PhoneBook::updateInterface(Mode mode)
 {
     currentMode = mode;

     switch (currentMode) {

     case AddingMode:
     case EditingMode:

         nameLine->setReadOnly(false);
         nameLine->setFocus(Qt::OtherFocusReason);
         phoneText->setReadOnly(false);

         addButton->setEnabled(false);
         editButton->setEnabled(false);
         removeButton->setEnabled(false);

         nextButton->setEnabled(false);
         previousButton->setEnabled(false);

         submitButton->show();
         cancelButton->show();

         loadButton->setEnabled(false);
         saveButton->setEnabled(false);
         break;

     case NavigationMode:

         if (contacts.isEmpty()) {
             nameLine->clear();
             phoneText->clear();
         }

         nameLine->setReadOnly(true);
         phoneText->setReadOnly(true);
         addButton->setEnabled(true);

         int number = contacts.size();
         editButton->setEnabled(number >= 1);
         removeButton->setEnabled(number >= 1);
         findButton->setEnabled(number > 2);
         nextButton->setEnabled(number > 1);
         previousButton->setEnabled(number > 1);

         submitButton->hide();
         cancelButton->hide();

         loadButton->setEnabled(true);
         saveButton->setEnabled(number >= 1);
         break;
     }
 }

 void PhoneBook::saveToFile()
 {
     QString fileName = QFileDialog::getSaveFileName(this,
         tr("Save Phone Book"), "",
         tr("Phone Book (*.pbk);;All Files (*)"));

     if (fileName.isEmpty())
         return;
     else {
         QFile file(fileName);
         if (!file.open(QIODevice::WriteOnly)) {
             QMessageBox::information(this, tr("Unable to open file"),
                 file.errorString());
             return;
         }

         QDataStream out(&file);
         out.setVersion(QDataStream::Qt_4_3);
         out << contacts;
     }
 }

 void PhoneBook::loadFromFile()
 {
     QString fileName = QFileDialog::getOpenFileName(this,
         tr("Open Phone Book"), "",
         tr("Phone Book (*.abk);;All Files (*)"));

     if (fileName.isEmpty())
         return;
     else {

         QFile file(fileName);

         if (!file.open(QIODevice::ReadOnly)) {
             QMessageBox::information(this, tr("Unable to open file"),
                 file.errorString());
             return;
         }

         QDataStream in(&file);
         in.setVersion(QDataStream::Qt_4_3);
         contacts.empty();   // очистка существующих контактов
         in >> contacts;

         if (contacts.isEmpty()) {
             QMessageBox::information(this, tr("No contacts in file"),
                 tr("The file you are attempting to open contains no contacts."));
         } else {
             QMap<QString, QString>::iterator i = contacts.begin();
             nameLine->setText(i.key());
             phoneText->setText(i.value());
         }
     }

     updateInterface(NavigationMode);
 }
