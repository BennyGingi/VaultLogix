#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QStatusBar>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit> // Add this line
#include "itemmanager.h"
#include "statistics.h"
#include "audittrail.h"
#include "usermanager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, const QString& userRole = "", const QString& userName = "");
    ~MainWindow();

private slots:
    void updateTable();
    void updateDateTime();
    void searchItems(const QString &searchText);
    void addItem();
    void editItem();
    void removeItem();
    void sellItem();
    void openSettings();
    void openHelpDialog();
    void setupStatisticsCharts();
    void updateStatisticsCharts();
    void logUserAction(const QString &action, const Item &item);
    void showLowStockAlert(const QString &itemName, int quantity);
    void openBudgetOverview();
    void toggleTheme();
    void loadStyle(const QString &styleName);

private:
    void setupUserInterface();
    void setupConnections();

    ItemManager *itemManager;
    Statistics *statistics;
    AuditTrail *auditTrail;
    UserManager *userManager;
    QTabWidget *tabWidget;
    QTableWidget *itemTable;
    QVBoxLayout *statisticsLayout;
    QHBoxLayout *buttonLayout;
    QLabel *dateTimeLabel;
    QLabel *userGreetingLabel;
    QTimer *dateTimeTimer;
    QLineEdit *searchBar;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *removeButton;
    QPushButton *sellButton;
    QPushButton *settingsButton;
    QPushButton *helpButton;
    QPushButton *budgetButton;
    QPushButton *themeToggleButton;
    QString userRole;
    QString userName;
    double budget;

    void setButtonIcon(QPushButton* button, const QString& iconPath, int iconSize); // Add this line
};

#endif // MAINWINDOW_H
