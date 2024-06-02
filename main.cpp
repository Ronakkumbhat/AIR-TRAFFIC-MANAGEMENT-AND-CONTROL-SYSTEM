#include "mainwindow.h"
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    if (!w.createDatabase()) {
        qDebug() << "Failed to initialize database.";
        return -1;
    }

    // Create the required table
    if (!w.createTable()) {
        qDebug() << "Failed to create table.";
        return -1;
    }

    qDebug() << "Database and table initialization completed successfully.";
    w.show();
    return a.exec();
}
