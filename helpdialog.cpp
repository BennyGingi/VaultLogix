#include "helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) : QDialog(parent) {
    setupUi();
    populateHelpText();
}

HelpDialog::~HelpDialog() {
}

void HelpDialog::setupUi() {
    setWindowTitle("Help");
    setFixedSize(600, 400);

    helpTextBrowser = new QTextBrowser(this);
    closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &HelpDialog::accept);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(helpTextBrowser);
    layout->addWidget(closeButton);
    setLayout(layout);
}

void HelpDialog::populateHelpText() {
    QString helpText = "<h1>VaultLogix Inventory Manager</h1>";
    helpText += "<p>Welcome to VaultLogix Inventory Manager! This application helps you manage your inventory efficiently.</p>";
    helpText += "<h2>Main Window</h2>";
    helpText += "<p>The main window displays the inventory items in a table view. You can perform various actions using the buttons provided:</p>";
    helpText += "<ul>";
    helpText += "<li><strong>Add Item:</strong> Allows you to add a new item to the inventory. Click the button and fill in the required details such as item name, unit, buying price, and selling price.</li>";
    helpText += "<li><strong>Edit Item:</strong> Enables you to modify the details of an existing item. Select an item from the table and click the button to update its information.</li>";
    helpText += "<li><strong>Remove Item:</strong> Removes an item from the inventory. Select an item from the table and click the button to delete it.</li>";
    helpText += "<li><strong>Sell Item:</strong> Records a sale transaction for an item. Select an item, enter the quantity sold and the selling price, and click the button to complete the sale.</li>";
    helpText += "<li><strong>Settings:</strong> Opens the settings dialog where you can configure various aspects of the application, such as generating reports and managing user accounts.</li>";
    helpText += "</ul>";
    helpText += "<h2>Search Bar</h2>";
    helpText += "<p>Use the search bar to quickly find specific items in the inventory. Enter a keyword or phrase, and the table will dynamically filter the items based on your search query.</p>";
    helpText += "<h2>Statistics</h2>";
    helpText += "<p>The right side of the main window displays statistical information about your inventory. It includes pie charts and graphs that visualize key metrics such as sales trends, top-selling items, revenue comparison, profit margins, low stock items, and category-wise revenue.</p>";
    helpText += "<h2>Settings</h2>";
    helpText += "<p>The settings dialog provides additional features and configurations:</p>";
    helpText += "<ul>";
    helpText += "<li><strong>Sales Report:</strong> Generates a detailed report of sales transactions, including item quantities and total revenue.</li>";
    helpText += "<li><strong>Inventory Report:</strong> Creates a report of the current inventory status, showing item quantities and their buying prices.</li>";
    helpText += "<li><strong>Financial Report:</strong> Provides an overview of the financial aspects, including total revenue, total buying price, and gross profit.</li>";
    helpText += "<li><strong>User Manager:</strong> Allows administrators to manage user accounts. You can add new users, edit existing user details, and remove user accounts.</li>";
    helpText += "<li><strong>Theme Switcher:</strong> Enables you to switch between light and dark themes for the application interface.</li>";
    helpText += "<li><strong>Export Data:</strong> Lets you export the inventory data in PDF or Excel format for further analysis or record-keeping purposes.</li>";
    helpText += "</ul>";
    helpText += "<p>For any further assistance or inquiries, please contact our support team at support@vaultlogix.com.</p>";

    helpTextBrowser->setHtml(helpText);
}
