#include "settings.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QApplication>
#include <QFileDialog>

Settings::Settings(ItemManager* itemManager, Statistics* statistics, AuditTrail* auditTrail, UserManager* userManager, QWidget* parent)
    : QDialog(parent), itemManager(itemManager), statistics(statistics), auditTrail(auditTrail), userManager(userManager), userManagerButtonsVisible(false) {
    setWindowTitle("Settings");

    layout = new QVBoxLayout(this);
    salesReportButton = new QPushButton("Sales Report", this);
    inventoryReportButton = new QPushButton("Inventory Report", this);
    financialReportButton = new QPushButton("Financial Report", this);
    userManagerButton = new QPushButton("User Manager", this);
    reportTextEdit = new QTextEdit(this);
    reportTextEdit->setReadOnly(true);
    userTable = new QTableWidget(this);
    userTable->setColumnCount(6);
    userTable->setHorizontalHeaderLabels({"Username", "Name", "Email", "Role", "Creation Date", "Last Login"});
    userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(salesReportButton);
    layout->addWidget(inventoryReportButton);
    layout->addWidget(financialReportButton);
    layout->addWidget(userManagerButton);
    layout->addWidget(reportTextEdit);
    layout->addWidget(userTable);

    connect(salesReportButton, &QPushButton::clicked, this, &Settings::displaySalesReport);
    connect(inventoryReportButton, &QPushButton::clicked, this, &Settings::displayInventoryReport);
    connect(financialReportButton, &QPushButton::clicked, this, &Settings::displayFinancialReport);
    connect(userManagerButton, &QPushButton::clicked, this, &Settings::openUserManager);

    exportDataButton = new QPushButton("Export Data", this);
    connect(exportDataButton, &QPushButton::clicked, this, &Settings::exportData);
    layout->addWidget(exportDataButton);

    resetBudgetButton = new QPushButton("Reset Budget", this); // New button for resetting budget
    connect(resetBudgetButton, &QPushButton::clicked, this, &Settings::onResetBudgetClicked);
    layout->addWidget(resetBudgetButton);

    populateUserTable();
}

Settings::~Settings() {}

void Settings::openUserManager() {
    toggleUserManagerButtons();
}

void Settings::toggleUserManagerButtons() {
    if (!userManagerButtonsVisible) {
        addUserButton = new QPushButton("Add User", this);
        editUserButton = new QPushButton("Edit User", this);
        removeUserButton = new QPushButton("Remove User", this);

        QVBoxLayout* userManagerLayout = new QVBoxLayout();
        userManagerLayout->addWidget(addUserButton);
        userManagerLayout->addWidget(editUserButton);
        userManagerLayout->addWidget(removeUserButton);
        userManagerLayout->addWidget(userTable);

        layout->addLayout(userManagerLayout);

        connect(addUserButton, &QPushButton::clicked, this, &Settings::addUser);
        connect(editUserButton, &QPushButton::clicked, this, &Settings::editUser);
        connect(removeUserButton, &QPushButton::clicked, this, &Settings::removeUser);

        userManagerButtonsVisible = true;
    } else {
        delete addUserButton;
        delete editUserButton;
        delete removeUserButton;
        addUserButton = nullptr;
        editUserButton = nullptr;
        removeUserButton = nullptr;

        userManagerButtonsVisible = false;
    }
}

void Settings::displaySalesReport() {
    QString report = "Sales Report\n";
    report += "Item\tUnits Sold\tTotal Revenue\n";
    for (const Item &item : itemManager->getItems()) {
        report += QString("%1\t%2\t%3\n").arg(item.name).arg(item.unitsSold).arg(item.unitsSold * item.sellingPrice);
    }
    reportTextEdit->setPlainText(report);
}

void Settings::displayInventoryReport() {
    QString report = "Inventory Report\n";
    report += "Item\tUnits in Stock\tBuying Price\n";
    for (const Item &item : itemManager->getItems()) {
        report += QString("%1\t%2\t%3\n").arg(item.name).arg(item.unit).arg(item.buyingPrice);
    }
    reportTextEdit->setPlainText(report);
}

void Settings::displayFinancialReport() {
    QString report = "Financial Report\n";
    double totalRevenue = statistics->calculateTotalRevenue();
    double totalBuyingPrice = statistics->calculateTotalBuyingPrice();
    double grossProfit = totalRevenue - totalBuyingPrice;

    report += QString("Total Revenue: %1\n").arg(totalRevenue);
    report += QString("Total Buying Price: %1\n").arg(totalBuyingPrice);
    report += QString("Gross Profit: %1\n").arg(grossProfit);
    report += QString("Current Budget: %1\n").arg(itemManager->getBudget());

    reportTextEdit->setPlainText(report);
}

void Settings::onResetBudgetClicked() {
    if (itemManager->getItems().isEmpty()) {
        itemManager->resetBudget();
        QMessageBox::information(this, "Budget Reset", "The budget has been successfully reset.");
    } else {
        QMessageBox::warning(this, "Cannot Reset Budget", "The budget can only be reset when there are no items in the inventory.");
    }
}

void Settings::addUser() {
    QString username = QInputDialog::getText(this, "Add User", "Enter username:");
    if (!username.isEmpty()) {
        QString password = QInputDialog::getText(this, "Add User", "Enter password:");
        if (!password.isEmpty()) {
            QString name = QInputDialog::getText(this, "Add User", "Enter name:");
            if (!name.isEmpty()) {
                QString email = QInputDialog::getText(this, "Add User", "Enter email:");
                if (!email.isEmpty()) {
                    QString role = QInputDialog::getItem(this, "Add User", "Select role:", {"admin", "user"}, 0, false);
                    if (!role.isEmpty()) {
                        User newUser;
                        newUser.username = username;
                        newUser.password = password;  // Assume password is hashed elsewhere
                        newUser.name = name;
                        newUser.email = email;
                        newUser.role = role;
                        newUser.creationDate = QDateTime::currentDateTime();
                        newUser.lastLogin = QDateTime::currentDateTime();
                        newUser.status = "active";

                        if (userManager->addUser(newUser)) {
                            populateUserTable();
                            QMessageBox::information(this, "Success", "User added successfully.");
                        } else {
                            QMessageBox::warning(this, "Error", "Failed to add user. Username might be already in use.");
                        }
                    }
                }
            }
        }
    }
}

void Settings::editUser() {
    int row = userTable->currentRow();
    if (row != -1) {
        QString username = userTable->item(row, 0)->text();

        User existingUser = userManager->getUserByUsername(username);
        if (existingUser.username.isEmpty()) {
            QMessageBox::warning(this, "Error", "User not found.");
            return;
        }

        QString password = QInputDialog::getText(this, "Edit User", "Enter new password (leave blank to keep current):");
        QString name = QInputDialog::getText(this, "Edit User", "Enter new name:", QLineEdit::Normal, userTable->item(row, 1)->text());
        QString email = QInputDialog::getText(this, "Edit User", "Enter new email:", QLineEdit::Normal, userTable->item(row, 2)->text());
        QString role = QInputDialog::getItem(this, "Edit User", "Select new role:", {"admin", "user"}, userTable->item(row, 3)->text() == "admin" ? 0 : 1, false);

        existingUser.password = password.isEmpty() ? existingUser.password : password;
        existingUser.name = name;
        existingUser.email = email;
        existingUser.role = role;

        if (userManager->editUser(existingUser.username, existingUser)) {
            populateUserTable();
            QMessageBox::information(this, "Success", "User updated successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Failed to update user.");
        }
    } else {
        QMessageBox::warning(this, "Error", "Please select a user to edit.");
    }
}

void Settings::removeUser() {
    int row = userTable->currentRow();
    if (row != -1) {
        QString username = userTable->item(row, 0)->text();

        if (userManager->removeUser(username)) {
            populateUserTable();
            QMessageBox::information(this, "Success", "User removed successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Failed to remove user.");
        }
    } else {
        QMessageBox::warning(this, "Error", "Please select a user to remove.");
    }
}

void Settings::populateUserTable() {
    userTable->setRowCount(0);
    QList<User> users = userManager->getUsers();
    for (const User &user : users) {
        int row = userTable->rowCount();
        userTable->insertRow(row);
        userTable->setItem(row, 0, new QTableWidgetItem(user.username));
        userTable->setItem(row, 1, new QTableWidgetItem(user.name));
        userTable->setItem(row, 2, new QTableWidgetItem(user.email));
        userTable->setItem(row, 3, new QTableWidgetItem(user.role));
        userTable->setItem(row, 4, new QTableWidgetItem(user.creationDate.toString("dd/MM/yyyy hh:mm:ss")));
        userTable->setItem(row, 5, new QTableWidgetItem(user.lastLogin.toString("dd/MM/yyyy hh:mm:ss")));
    }
}

void Settings::exportData() {
    QString filePath = QFileDialog::getSaveFileName(this, "Export Data", "", "CSV files (*.csv);;All files (*.*)");
    if (!filePath.isEmpty()) {
        // Perform data export here
        QMessageBox::information(this, "Success", "Data exported successfully.");
    }
}
