#ifndef ITEMMANAGER_H
#define ITEMMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QDateTime>

struct Item {
    QString name;
    int unit;
    double buyingPrice;
    QString barcode;
    QDateTime dateAdded;
    double sellingPrice; // To be added when the item is sold
    int unitsSold;
    QDateTime dateSold;
    QString soldBy; // To track who sold the item
};

class ItemManager : public QObject {
    Q_OBJECT

public:
    explicit ItemManager(QObject *parent = nullptr);
    ~ItemManager();

    void addItem(const Item &item);
    bool editItem(int index, const Item &newItem);
    bool removeItem(int index);
    bool sellItem(int index, int quantity, double sellingPrice, const QString &user);
    void loadItems();
    void saveItems();
    QList<Item> getItems() const;
    Item getItemByName(const QString &name) const;
    int findItemIndexByName(const QString &name) const;
    void addToBudget(double amount);
    void deductFromBudget(double amount);
    double getBudget() const;
    void resetBudget();

signals:
    void itemsUpdated();
    void lowStockAlert(const QString &itemName, int quantity);

private:
    QString dataFilePath = "item.json";
    QString budgetFilePath;
    QList<Item> items;
    double budget; // Budget tracking

    void saveBudget() const;
    void loadBudget();
};

#endif // ITEMMANAGER_H
