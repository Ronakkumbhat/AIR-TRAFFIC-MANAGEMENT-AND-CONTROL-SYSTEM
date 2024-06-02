#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool createDatabase();
    bool createTable();
    bool isDuplicateRecord(const QString &time, const QString &operation);
    void adjustOperationsForEmergencyLanding(const QString &emergencyTime);
    void handleEmergencyLanding(const QString &emergencyTime);
private slots:
    void onAddRecordClicked();
    void onEditRecordClicked();
    void onFindClicked();
    void onEmergencyClicked();
    void onViewClicked();

private:
    QPushButton *addRecordButton;
    QPushButton *editRecordButton;
    QPushButton *findButton;
    QPushButton *viewButton;
    QPushButton *emergencyButton;
    QLineEdit *timeLineEdit;
    QLineEdit *airlineLineEdit;
    QComboBox *operationComboBox;
    QLineEdit *aircraftLineEdit;
    QLineEdit *passengersLineEdit;
    // Other necessary widgets for input/output

    QSqlDatabase db;
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
