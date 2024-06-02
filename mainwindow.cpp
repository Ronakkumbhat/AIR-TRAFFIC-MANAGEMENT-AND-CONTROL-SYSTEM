#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
        // Set window title and size
        setWindowTitle("Flight Management System");
        resize(600, 400);

        // Create buttons
        addRecordButton = new QPushButton("Add Record", this);
        editRecordButton = new QPushButton("Edit Record", this);
        findButton = new QPushButton("Find", this);
        viewButton = new QPushButton("View", this);
        emergencyButton = new QPushButton("Emergency Landing", this);

        // Create line edits and combo box
        timeLineEdit = new QLineEdit(this);
        airlineLineEdit = new QLineEdit(this);
        operationComboBox = new QComboBox(this);
        operationComboBox->addItem("Arrival");
        operationComboBox->addItem("Departure");
        aircraftLineEdit = new QLineEdit(this);
        passengersLineEdit = new QLineEdit(this);

        // Create labels
        QLabel *timeLabel = new QLabel("Time:", this);
        QLabel *airlineLabel = new QLabel("Airline:", this);
        QLabel *operationLabel = new QLabel("Operation:", this);
        QLabel *aircraftLabel = new QLabel("Aircraft Type:", this);
        QLabel *passengersLabel = new QLabel("Passengers:", this);

        // Create layout
        QVBoxLayout *mainLayout = new QVBoxLayout;
        QHBoxLayout *buttonLayout = new QHBoxLayout;

        // Add widgets to layouts
        mainLayout->addWidget(timeLabel);
        mainLayout->addWidget(timeLineEdit);
        mainLayout->addWidget(airlineLabel);
        mainLayout->addWidget(airlineLineEdit);
        mainLayout->addWidget(operationLabel);
        mainLayout->addWidget(operationComboBox);
        mainLayout->addWidget(aircraftLabel);
        mainLayout->addWidget(aircraftLineEdit);
        mainLayout->addWidget(passengersLabel);
        mainLayout->addWidget(passengersLineEdit);

        buttonLayout->addWidget(addRecordButton);
        buttonLayout->addWidget(editRecordButton);
        buttonLayout->addWidget(findButton);
        buttonLayout->addWidget(viewButton);
        buttonLayout->addWidget(emergencyButton);

        mainLayout->addLayout(buttonLayout);

        // Set central widget
        QWidget *centralWidget = new QWidget(this);
        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);

        // Connect button signals to slots
        connect(addRecordButton, &QPushButton::clicked, this, &MainWindow::onAddRecordClicked);

        connect(editRecordButton, &QPushButton::clicked, this, &MainWindow::onEditRecordClicked);

        connect(findButton, &QPushButton::clicked, this, &MainWindow::onFindClicked);

        connect(viewButton, &QPushButton::clicked, this, &MainWindow::onViewClicked);

        connect(emergencyButton, &QPushButton::clicked, this, &MainWindow::onEmergencyClicked);

}

MainWindow::~MainWindow()
{
    delete ui;
}
bool MainWindow::createDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("flight_records.db"); // Path to your SQLite database file

    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
        return false;
    }

    return true;
}
bool MainWindow::createTable() {
    QSqlQuery query;

    // Execute SQL query to create the 'atm' table if it doesn't exist
    if (!query.exec("CREATE TABLE IF NOT EXISTS atm ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "time TEXT,"
                    "operation TEXT,"
                    "airline TEXT,"
                    "aircraft TEXT,"
                    "owner TEXT,"
                    "passengers INTEGER)")) {
        qDebug() << "Failed to create table:" << query.lastError().text();
        return false;
    }

    return true;
}
void MainWindow::adjustOperationsForEmergencyLanding(const QString &emergencyTime) {
    QSqlQuery query;

    // Execute SQL query to update operation times for affected flights
    query.prepare("SELECT DISTINCT time FROM atm "
                  "WHERE time >= :emergencyTime "
                  "ORDER BY time ASC");

    query.bindValue(":emergencyTime", emergencyTime);

    if (query.exec()) {
        while (query.next()) {
            QString currentDepartureTime = query.value(0).toString();

            // Check for duplicates
            QSqlQuery duplicateQuery;
            duplicateQuery.prepare("SELECT COUNT(*) FROM atm "
                                   "WHERE time = :currentDepartureTime");
            duplicateQuery.bindValue(":currentDepartureTime", currentDepartureTime);
            if (duplicateQuery.exec() && duplicateQuery.next()) {
                int duplicateCount = duplicateQuery.value(0).toInt();
                QString adjustedTime;
                if (duplicateCount > 1) {
                    // Resolve duplicate by adding 5 minutes
                    adjustedTime = QDateTime::fromString(currentDepartureTime, "yyyyMMddhhmm").addSecs(300).toString("yyyyMMddhhmm");
                    query.prepare("UPDATE atm SET time =:newtime "
                                  "WHERE time = :currentDepartureTime");
                } else {
                    // No conflict, just adjust time by adding 5 minutes
                    adjustedTime = QDateTime::fromString(currentDepartureTime, "yyyyMMddhhmm").addSecs(300).toString("yyyyMMddhhmm");

                    query.prepare("UPDATE atm SET time =:newtime "
                                  "WHERE time = :currentDepartureTime");
                }

                query.bindValue(":currentDepartureTime", currentDepartureTime);
                query.bindValue(":newtime", adjustedTime);
                qDebug() << "Adjusting operation time:" << currentDepartureTime << "to" << adjustedTime;

                if (!query.exec()) {
                    qDebug() << "Failed to adjust operation time:" << query.lastError().text();
                }
            }
        }

        qDebug() << "Operations adjusted for emergency landing.";
    } else {
        qDebug() << "Failed to retrieve operations:" << query.lastError().text();
    }
}
void MainWindow::handleEmergencyLanding(const QString &emergencyTime) {
    // Example: Simulate an emergency landing scenario
    qDebug() << "Emergency landing at:" << emergencyTime;
    adjustOperationsForEmergencyLanding(emergencyTime);
}
bool MainWindow::isDuplicateRecord(const QString &time,  const QString &operation) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM atm "
                  "WHERE time = :time "
                  "AND operation = :operation");
    query.bindValue(":time", time);
    query.bindValue(":operation", operation);

    if (query.exec() && query.next()) {
        int count = query.value(0).toInt();
        return (count > 0); // Returns true if record exists (duplicate)
    }

    return false; // Error occurred or record does not exist
}
void MainWindow::onAddRecordClicked() {
    // Implement functionality for adding a record
    // Retrieve data from line edits and combo box
    QString time = timeLineEdit->text();
    QString airline = airlineLineEdit->text();
    QString operation = operationComboBox->currentText();
    QString aircraft = aircraftLineEdit->text();
    QString passengers = passengersLineEdit->text();
    QSqlQuery query(db);
    query.prepare("INSERT INTO atm (time, operation, airline, aircraft, passengers) "
                  "VALUES (:time, :operation, :airline, :aircraft, :passengers)");
    query.bindValue(":time", time);
    query.bindValue(":operation", operation);
    query.bindValue(":airline", airline);
    query.bindValue(":aircraft", aircraft);
    query.bindValue(":passengers", passengers);
    if (isDuplicateRecord(time, operation)) {
        QMessageBox::critical(this, "Error", "Duplicate record detected. Record not inserted.");
        return ;
    }
    if (query.exec()) {
        QMessageBox::information(this, "Success", "Record Inserted");
    } else {
        QMessageBox::critical(this, "Error", "Insert Error: " + query.lastError().text());
    }
    // Process the data (e.g., validate, insert into database)
    // Example:
    qDebug() << "Adding record with Time:" << time << "Airline:" << airline
             << "Operation:" << operation << "Aircraft:" << aircraft
             << "Passengers:" << passengers;
}

void MainWindow::onEditRecordClicked() {
    QString recordId = QInputDialog::getText(this, "Edit Record", "Enter Record ID:");

    // Perform database query to retrieve the record based on recordId
    QSqlQuery query(db);
    query.prepare("SELECT * FROM atm WHERE time = :id");
    query.bindValue(":id", recordId);

    if (query.exec() && query.first()) {
        // Record found, populate UI with retrieved data
        QString time = query.value("time").toString();
        QString operation = query.value("operation").toString();
        QString airline = query.value("airline").toString();
        QString aircraft = query.value("aircraft").toString();
        int passengers = query.value("passengers").toInt();
        //delete the record
        query.prepare("DELETE FROM atm WHERE time = :id");
        query.bindValue(":id", recordId);
        if (query.exec()) {
            QMessageBox::information(this, "Success", "Record Deleted");
            // Update UI components with retrieved data
            timeLineEdit->setText(time);
            airlineLineEdit->setText(airline);
            operationComboBox->setCurrentText(operation);
            aircraftLineEdit->setText(aircraft);
            passengersLineEdit->setText(QString::number(passengers));
        } else {
            QMessageBox::critical(this, "Error", "Delete Error: " + query.lastError().text());
        }
    } else {
        QMessageBox::warning(this, "Error", "Record not found!");
    }
}


void MainWindow::onFindClicked() {
    QString searchTime = QInputDialog::getText(this, "Find Record", "Enter Time:");

    // Perform database query to find records matching the searchTime
    QSqlQuery query(db);
    query.prepare("SELECT * FROM atm WHERE time = :time");
    query.bindValue(":time", searchTime);

    if (query.exec()) {
        // Display the results in a text output (e.g., QMessageBox or QTextEdit)
        QString resultText;
        while (query.next()) {
            QString time = query.value("time").toString();
            QString operation = query.value("operation").toString();
            QString airline = query.value("airline").toString();
            QString aircraft = query.value("aircraft").toString();
            int passengers = query.value("passengers").toInt();

            resultText += QString("Time: %1, Operation: %2, Airline: %3, Aircraft: %4, Passengers: %5\n")
                              .arg(time)
                              .arg(operation)
                              .arg(airline)
                              .arg(aircraft)
                              .arg(passengers);
        }

        // Display the search results
        if (!resultText.isEmpty()) {
            QMessageBox::information(this, "Search Results", resultText);
        } else {
            QMessageBox::information(this, "Search Results", "No records found matching the criteria.");
        }
    } else {
        QMessageBox::warning(this, "Error", "Query error: " + query.lastError().text());
    }
}

void MainWindow::onEmergencyClicked()
{
    QString emergencyTime = QInputDialog::getText(this, "Emergency Landing", "Enter Emergency Time:");
    QString Fuel = QInputDialog::getText(this, "Fuel", "Enter Fuel:");
    handleEmergencyLanding(emergencyTime);
}


void MainWindow::onViewClicked() {
    // Perform database query to retrieve all records
    QSqlQuery query("SELECT * FROM atm", db);

    if (query.exec()) {
        // Display the records in a text output (e.g., QMessageBox or QTextEdit)
        QString resultText = "Flight Records:\n";
        while (query.next()) {
            QString time = query.value("time").toString();
            QString operation = query.value("operation").toString();
            QString airline = query.value("airline").toString();
            QString aircraft = query.value("aircraft").toString();
            int passengers = query.value("passengers").toInt();

            resultText += QString("Time: %1, Operation: %2, Airline: %3, Aircraft: %4, Passengers: %5\n")
                              .arg(time)
                              .arg(operation)
                              .arg(airline)
                              .arg(aircraft)
                              .arg(passengers);
        }

        // Display the records
        if (!resultText.isEmpty()) {
            QMessageBox::information(this, "Flight Records", resultText);
        } else {
            QMessageBox::information(this, "Flight Records", "No records found.");
        }
    } else {
        QMessageBox::warning(this, "Error", "Query error: " + query.lastError().text());
    }
}
