#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QTableWidget>
#include <QHeaderView>
#include "itemmanager.h"
#include "statistics.h"
#include "audittrail.h"
#include "usermanager.h"

class Settings : public QDialog {
    Q_OBJECT

public:
    explicit Settings(ItemManager* itemManager, Statistics* statistics, AuditTrail* auditTrail, UserManager* userManager, QWidget* parent = nullptr);
    ~Settings();

private slots:
    void openUserManager();
    void toggleUserManagerButtons();
    void displaySalesReport();
    void displayInventoryReport();
    void displayFinancialReport();
    void addUser();
    void editUser();
    void removeUser();
    void populateUserTable();
    void exportData();
    void onResetBudgetClicked(); // New slot for resetting budget

private:
    ItemManager* itemManager;
    Statistics* statistics;
    AuditTrail* auditTrail;
    UserManager* userManager;

    QVBoxLayout* layout;
    QPushButton* salesReportButton;
    QPushButton* inventoryReportButton;
    QPushButton* financialReportButton;
    QPushButton* userManagerButton;
    QTextEdit* reportTextEdit;
    QTableWidget* userTable;

    QPushButton* addUserButton;
    QPushButton* editUserButton;
    QPushButton* removeUserButton;
    bool userManagerButtonsVisible;

    QPushButton* exportDataButton;
    QPushButton* resetBudgetButton; // New button for resetting budget
};

#endif // SETTINGS_H
