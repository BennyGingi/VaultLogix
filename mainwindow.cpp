#include "mainwindow.h"
#include "settings.h"
#include "helpdialog.h"
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QChartView>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QDir>
#include <QApplication> // Include this header
#include <QDateTimeEdit> // Add this line

MainWindow::MainWindow(QWidget *parent, const QString& userRole, const QString& userName)
    : QMainWindow(parent), userRole(userRole), userName(userName), budget(50000.0) {
    itemManager = new ItemManager(this);
    statistics = new Statistics(itemManager, this);
    auditTrail = new AuditTrail(this);
    userManager = new UserManager(this);

    setupUserInterface();
    setupConnections();

    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &MainWindow::updateDateTime);
    dateTimeTimer->start(1000); // Update every second

    updateTable();
    updateStatisticsCharts();
}

MainWindow::~MainWindow() {
    // Add any necessary cleanup here
}

void MainWindow::setupUserInterface() {
    setWindowTitle("VaultLogix");
    setWindowIcon(QIcon(":/assets/mainwindow.png")); // Set the main window's icon
    resize(800, 600);

    tabWidget = new QTabWidget(this);
    QWidget *inventoryTab = new QWidget(this);
    QWidget *statisticsTab = new QWidget(this);

    // Inventory Tab Layout
    QVBoxLayout *inventoryLayout = new QVBoxLayout(inventoryTab);
    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("Search...");

    itemTable = new QTableWidget(this);
    itemTable->setColumnCount(5);
    itemTable->setHorizontalHeaderLabels({"Name", "Units", "Buying Price", "Selling Price", "Date Added"});
    itemTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    addButton = new QPushButton("Add Item", this);
    editButton = new QPushButton("Edit Item", this);
    removeButton = new QPushButton("Remove Item", this);
    sellButton = new QPushButton("Sell Item", this);
    settingsButton = new QPushButton("Settings", this);
    helpButton = new QPushButton("Help", this);
    budgetButton = new QPushButton("Budget Overview", this);
    themeToggleButton = new QPushButton("Toggle Theme", this);

    // Set icons for buttons
    setButtonIcon(addButton, ":/assets/add.png", 32);
    setButtonIcon(editButton, ":/assets/edit.png", 32);
    setButtonIcon(removeButton, ":/assets/remove.png", 32);
    setButtonIcon(sellButton, ":/assets/sell.png", 32);
    setButtonIcon(settingsButton, ":/assets/setting.png", 32);
    setButtonIcon(helpButton, ":/assets/help.svg", 32);
    setButtonIcon(budgetButton, ":/assets/budget.png", 32);
    setButtonIcon(themeToggleButton, ":/assets/theme.png", 32);

    buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addWidget(sellButton);
    buttonLayout->addWidget(settingsButton);
    buttonLayout->addWidget(helpButton);
    buttonLayout->addWidget(budgetButton);
    buttonLayout->addWidget(themeToggleButton);

    inventoryLayout->addWidget(searchBar);
    inventoryLayout->addWidget(itemTable);
    inventoryLayout->addLayout(buttonLayout);

    // Statistics Tab Layout
    statisticsLayout = new QVBoxLayout(statisticsTab);

    tabWidget->addTab(inventoryTab, "Inventory");
    tabWidget->addTab(statisticsTab, "Statistics");

    setCentralWidget(tabWidget);

    // Status bar setup
    dateTimeLabel = new QLabel(this);
    userGreetingLabel = new QLabel(QString("Welcome, %1!").arg(userName), this);
    statusBar()->addWidget(dateTimeLabel);
    statusBar()->addWidget(userGreetingLabel);

    // Load the saved theme
    QSettings settings;
    QString theme = settings.value("theme", "light").toString();
    loadStyle(theme);
}

void MainWindow::setupConnections() {
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addItem);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::editItem);
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeItem);
    connect(sellButton, &QPushButton::clicked, this, &MainWindow::sellItem);
    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(helpButton, &QPushButton::clicked, this, &MainWindow::openHelpDialog);
    connect(budgetButton, &QPushButton::clicked, this, &MainWindow::openBudgetOverview);
    connect(themeToggleButton, &QPushButton::clicked, this, &MainWindow::toggleTheme);
    connect(searchBar, &QLineEdit::textChanged, this, &MainWindow::searchItems);
    connect(itemManager, &ItemManager::itemsUpdated, this, &MainWindow::updateTable);
    connect(itemManager, &ItemManager::lowStockAlert, this, &MainWindow::showLowStockAlert);
}

void MainWindow::setButtonIcon(QPushButton* button, const QString& iconPath, int iconSize) {
    QIcon icon(iconPath);
    button->setIcon(icon);
    button->setIconSize(QSize(iconSize, iconSize));
}

void MainWindow::addItem() {
    QDialog dialog(this);
    dialog.setWindowTitle("Add Item");

    QFormLayout formLayout(&dialog);
    QLineEdit nameEdit;
    QSpinBox quantityEdit;
    quantityEdit.setRange(1, 9999);
    QDoubleSpinBox buyingPriceEdit;
    buyingPriceEdit.setRange(0, 99999);
    QDateTimeEdit dateAddedEdit;
    dateAddedEdit.setDateTime(QDateTime::currentDateTime());

    formLayout.addRow("Name:", &nameEdit);
    formLayout.addRow("Quantity:", &quantityEdit);
    formLayout.addRow("Buying Price:", &buyingPriceEdit);
    formLayout.addRow("Date Added:", &dateAddedEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    formLayout.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        Item newItem;
        newItem.name = nameEdit.text();
        newItem.unit = quantityEdit.value();
        newItem.buyingPrice = buyingPriceEdit.value();
        newItem.dateAdded = dateAddedEdit.dateTime();

        if (itemManager->getBudget() >= newItem.buyingPrice * newItem.unit) {
            itemManager->addItem(newItem);
            logUserAction("Added Item", newItem);
        } else {
            QMessageBox::warning(this, "Budget Exceeded", "Not enough budget to add this item.");
        }
    }
}

void MainWindow::editItem() {
    QDialog dialog(this);
    dialog.setWindowTitle("Edit Item");

    QFormLayout formLayout(&dialog);
    QComboBox itemSelect;
    QLineEdit nameEdit;
    QSpinBox quantityEdit;
    quantityEdit.setRange(1, 9999);
    QDoubleSpinBox buyingPriceEdit;
    buyingPriceEdit.setRange(0, 99999);
    QDateTimeEdit dateAddedEdit;

    formLayout.addRow("Select Item:", &itemSelect);
    formLayout.addRow("Name:", &nameEdit);
    formLayout.addRow("Quantity:", &quantityEdit);
    formLayout.addRow("Buying Price:", &buyingPriceEdit);
    formLayout.addRow("Date Added:", &dateAddedEdit);

    for (const Item &item : itemManager->getItems()) {
        itemSelect.addItem(item.name);
    }

    connect(&itemSelect, &QComboBox::currentTextChanged, this, [&](const QString &selectedItem) {
        const Item item = itemManager->getItemByName(selectedItem);
        nameEdit.setText(item.name);
        quantityEdit.setValue(item.unit);
        buyingPriceEdit.setValue(item.buyingPrice);
        dateAddedEdit.setDateTime(item.dateAdded);
    });

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    formLayout.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString selectedItem = itemSelect.currentText();
        int itemIndex = itemManager->findItemIndexByName(selectedItem);

        if (itemIndex != -1) {
            Item newItem;
            newItem.name = nameEdit.text();
            newItem.unit = quantityEdit.value();
            newItem.buyingPrice = buyingPriceEdit.value();
            newItem.dateAdded = dateAddedEdit.dateTime();

            itemManager->editItem(itemIndex, newItem);
            logUserAction("Edited Item", newItem);
        }
    }
}

void MainWindow::removeItem() {
    QDialog dialog(this);
    dialog.setWindowTitle("Remove Item");

    QFormLayout formLayout(&dialog);
    QComboBox itemSelect;

    formLayout.addRow("Select Item:", &itemSelect);

    for (const Item &item : itemManager->getItems()) {
        itemSelect.addItem(item.name);
    }

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    formLayout.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString selectedItem = itemSelect.currentText();
        int itemIndex = itemManager->findItemIndexByName(selectedItem);

        if (itemIndex != -1) {
            Item removedItem = itemManager->getItemByName(selectedItem);
            itemManager->removeItem(itemIndex);
            logUserAction("Removed Item", removedItem);
        }
    }
}

void MainWindow::sellItem() {
    QDialog dialog(this);
    dialog.setWindowTitle("Sell Item");

    QFormLayout formLayout(&dialog);
    QComboBox itemSelect;
    QSpinBox quantityEdit;
    quantityEdit.setRange(1, 9999);
    QDoubleSpinBox sellingPriceEdit;
    sellingPriceEdit.setRange(0, 99999);

    formLayout.addRow("Select Item:", &itemSelect);
    formLayout.addRow("Quantity:", &quantityEdit);
    formLayout.addRow("Selling Price:", &sellingPriceEdit);

    for (const Item &item : itemManager->getItems()) {
        itemSelect.addItem(item.name);
    }

    connect(&itemSelect, &QComboBox::currentTextChanged, this, [&](const QString &selectedItem) {
        const Item item = itemManager->getItemByName(selectedItem);
        quantityEdit.setMaximum(item.unit); // Set the maximum quantity to the item's available quantity
    });

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    formLayout.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString selectedItem = itemSelect.currentText();
        int itemIndex = itemManager->findItemIndexByName(selectedItem);
        int quantity = quantityEdit.value();
        double sellingPrice = sellingPriceEdit.value();

        if (itemIndex != -1 && itemManager->sellItem(itemIndex, quantity, sellingPrice, userName)) {
            Item soldItem = itemManager->getItemByName(selectedItem);
            logUserAction("Sold Item", soldItem);
        } else {
            QMessageBox::warning(this, "Sell Item", "Failed to sell item.");
        }
    }
}

void MainWindow::setupStatisticsCharts() {
    updateStatisticsCharts();
}

void MainWindow::updateStatisticsCharts() {
    // Clear the current charts
    QLayoutItem *item;
    while ((item = statisticsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Add updated charts
    statisticsLayout->addWidget(new QChartView(statistics->createTop5BestSellersChart()));
    statisticsLayout->addWidget(new QChartView(statistics->createTotalRevenueChart()));
    statisticsLayout->addWidget(new QChartView(statistics->createAverageSellingPriceChart()));
    statisticsLayout->addWidget(new QChartView(statistics->createTotalItemsInInventoryChart()));
    statisticsLayout->addWidget(new QChartView(statistics->createTotalUnitsSoldChart()));
}

void MainWindow::updateDateTime() {
    dateTimeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

void MainWindow::logUserAction(const QString &action, const Item &item) {
    auditTrail->logAction(action, userName, item.name, item.buyingPrice, item.sellingPrice, item.unit, item.sellingPrice * item.unit, item.dateAdded, item.dateSold);
}

void MainWindow::showLowStockAlert(const QString &itemName, int quantity) {
    QMessageBox::warning(this, "Low Stock Alert", QString("The stock for item '%1' is low (%2 units remaining).").arg(itemName).arg(quantity));
}

void MainWindow::searchItems(const QString &searchText) {
    QList<Item> items = itemManager->getItems();
    QList<Item> filteredItems;

    for (const Item &item : items) {
        if (item.name.contains(searchText, Qt::CaseInsensitive)) {
            filteredItems.append(item);
        }
    }

    itemTable->clearContents();
    itemTable->setRowCount(filteredItems.size());

    for (int row = 0; row < filteredItems.size(); ++row) {
        const Item &item = filteredItems[row];
        itemTable->setItem(row, 0, new QTableWidgetItem(item.name));
        itemTable->setItem(row, 1, new QTableWidgetItem(QString::number(item.unit)));
        itemTable->setItem(row, 2, new QTableWidgetItem(QString::number(item.buyingPrice)));
        itemTable->setItem(row, 3, new QTableWidgetItem(QString::number(item.sellingPrice)));
        itemTable->setItem(row, 4, new QTableWidgetItem(item.dateAdded.toString("yyyy-MM-dd hh:mm:ss")));
    }
}

void MainWindow::updateTable() {
    QList<Item> items = itemManager->getItems();
    itemTable->clearContents();
    itemTable->setRowCount(items.size());

    for (int row = 0; row < items.size(); ++row) {
        const Item &item = items[row];
        QTableWidgetItem *nameItem = new QTableWidgetItem(item.name);
        QTableWidgetItem *quantityItem = new QTableWidgetItem(QString::number(item.unit));
        QTableWidgetItem *buyingPriceItem = new QTableWidgetItem(QString::number(item.buyingPrice));
        QTableWidgetItem *sellingPriceItem = new QTableWidgetItem(QString::number(item.sellingPrice));
        QTableWidgetItem *dateAddedItem = new QTableWidgetItem(item.dateAdded.toString("yyyy-MM-dd hh:mm:ss"));

        if (item.unit < 5) {
            nameItem->setBackground(Qt::red);
            quantityItem->setBackground(Qt::red);
            buyingPriceItem->setBackground(Qt::red);
            sellingPriceItem->setBackground(Qt::red);
            dateAddedItem->setBackground(Qt::red);
        }

        itemTable->setItem(row, 0, nameItem);
        itemTable->setItem(row, 1, quantityItem);
        itemTable->setItem(row, 2, buyingPriceItem);
        itemTable->setItem(row, 3, sellingPriceItem);
        itemTable->setItem(row, 4, dateAddedItem);
    }
}

void MainWindow::openSettings() {
    Settings settingsDialog(itemManager, statistics, auditTrail, userManager, this);
    settingsDialog.exec();
}

void MainWindow::openHelpDialog() {
    HelpDialog helpDialog(this);
    helpDialog.exec();
}

void MainWindow::openBudgetOverview() {
    QMessageBox::information(this, "Budget Overview", QString("Current budget: $%1").arg(itemManager->getBudget()));
}

void MainWindow::loadStyle(const QString &styleName) {
    QFile file(QString(":/styles/") + styleName + QString(".qss"));
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        qApp->setStyleSheet(stream.readAll());
        file.close();
    } else {
        qWarning("Could not open the QSS file");
    }
}

void MainWindow::toggleTheme() {
    QSettings settings;
    QString currentTheme = settings.value("theme", "light").toString();
    QString newTheme = (currentTheme == "light") ? "dark" : "light";
    loadStyle(newTheme);
    settings.setValue("theme", newTheme);
}
