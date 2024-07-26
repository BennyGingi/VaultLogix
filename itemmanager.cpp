#include "itemmanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>

ItemManager::ItemManager(QObject *parent) : QObject(parent), budget(50000.0) {
    budgetFilePath = QCoreApplication::applicationDirPath() + QDir::separator() + "budget.json";
    qDebug() << "Budget file path:" << budgetFilePath;
    loadItems();
    loadBudget(); // Load the budget when the application starts
}

ItemManager::~ItemManager() {
    saveItems();
    saveBudget(); // Save the budget when the application closes
}

void ItemManager::addItem(const Item &item) {
    items.append(item);
    saveItems();
    deductFromBudget(item.buyingPrice * item.unit); // Deduct from budget when adding item
    emit itemsUpdated();
}

bool ItemManager::editItem(int index, const Item &newItem) {
    if (index < 0 || index >= items.size()) return false;
    double difference = newItem.buyingPrice * newItem.unit - items[index].buyingPrice * items[index].unit;
    items[index] = newItem;
    saveItems();
    deductFromBudget(difference); // Adjust budget when editing item
    emit itemsUpdated();
    return true;
}

bool ItemManager::removeItem(int index) {
    if (index < 0 || index >= items.size()) return false;
    items.removeAt(index);
    saveItems();
    emit itemsUpdated();
    return true;
}

bool ItemManager::sellItem(int index, int quantity, double sellingPrice, const QString &user) {
    if (index < 0 || index >= items.size() || items[index].unit < quantity) return false;
    items[index].unitsSold += quantity;
    items[index].unit -= quantity;
    items[index].sellingPrice = sellingPrice;
    items[index].dateSold = QDateTime::currentDateTime();
    items[index].soldBy = user;
    saveItems();
    addToBudget(sellingPrice * quantity); // Add to budget when selling item
    emit itemsUpdated();
    if (items[index].unit <= 5) emit lowStockAlert(items[index].name, items[index].unit);
    return true;
}

void ItemManager::loadItems() {
    QFile file(dataFilePath);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray jsonArray = doc.array();
        items.clear();
        for (const QJsonValue &value : jsonArray) {
            QJsonObject obj = value.toObject();
            Item item;
            item.name = obj["name"].toString();
            item.unit = obj["unit"].toInt();
            item.buyingPrice = obj["buyingPrice"].toDouble();
            item.barcode = obj["barcode"].toString();
            item.dateAdded = QDateTime::fromString(obj["dateAdded"].toString(), Qt::ISODate);
            item.sellingPrice = obj["sellingPrice"].toDouble();
            item.unitsSold = obj["unitsSold"].toInt();
            item.dateSold = QDateTime::fromString(obj["dateSold"].toString(), Qt::ISODate);
            item.soldBy = obj["soldBy"].toString();
            items.append(item);
        }
        file.close();
    }
}

void ItemManager::saveItems() {
    QJsonArray jsonArray;
    for (const Item &item : items) {
        QJsonObject obj;
        obj["name"] = item.name;
        obj["unit"] = item.unit;
        obj["buyingPrice"] = item.buyingPrice;
        obj["barcode"] = item.barcode;
        obj["dateAdded"] = item.dateAdded.toString(Qt::ISODate);
        obj["sellingPrice"] = item.sellingPrice;
        obj["unitsSold"] = item.unitsSold;
        obj["dateSold"] = item.dateSold.toString(Qt::ISODate);
        obj["soldBy"] = item.soldBy;
        jsonArray.append(obj);
    }
    QJsonDocument doc(jsonArray);
    QFile file(dataFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

QList<Item> ItemManager::getItems() const {
    return items;
}

Item ItemManager::getItemByName(const QString &name) const {
    for (const Item &item : items) {
        if (item.name == name) {
            return item;
        }
    }
    return Item();
}

int ItemManager::findItemIndexByName(const QString &name) const {
    for (int i = 0; i < items.size(); ++i) {
        if (items[i].name == name) {
            return i;
        }
    }
    return -1;
}

void ItemManager::addToBudget(double amount) {
    budget += amount;
    saveBudget(); // Ensure budget is saved whenever it's updated
}

void ItemManager::deductFromBudget(double amount) {
    budget -= amount;
    saveBudget(); // Ensure budget is saved whenever it's updated
}

double ItemManager::getBudget() const {
    return budget;
}

void ItemManager::resetBudget() {
    budget = 50000.0; // Reset budget to initial value
    saveBudget();
}

void ItemManager::saveBudget() const {
    QFile file(budgetFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject obj;
        obj["budget"] = budget;
        QJsonDocument doc(obj);
        file.write(doc.toJson());
        file.close();
        qDebug() << "Budget saved:" << budget;
    } else {
        qWarning() << "Failed to save budget.";
    }
}

void ItemManager::loadBudget() {
    QFile file(budgetFilePath);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc(QJsonDocument::fromJson(data));
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            budget = obj["budget"].toDouble();
            qDebug() << "Budget loaded:" << budget;
        }
        file.close();
    } else {
        qWarning() << "Failed to load budget or file does not exist.";
    }
}
