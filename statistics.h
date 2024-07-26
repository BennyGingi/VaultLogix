#ifndef STATISTICS_H
#define STATISTICS_H

#include <QObject>
#include <QChart>
#include <QPair>
#include <QDateTime>
#include <QList>
#include "itemmanager.h"

class Statistics : public QObject {
    Q_OBJECT

public:
    explicit Statistics(ItemManager* itemManager, QObject* parent = nullptr);
    ~Statistics();

    double calculateTotalRevenue();
    double calculateAverageSellingPrice();
    int calculateTotalItemsInInventory();
    int calculateTotalUnitsSold();
    double calculateTotalBuyingPrice();
    QList<QPair<QString, int>> calculateLowStockItems(int threshold);
    QList<QPair<QDateTime, double>> calculateSalesOverTime();
    QList<QPair<QString, QPair<double, int>>> calculateTop5BestSellers();

    QChart* createTop5BestSellersChart();
    QChart* createTotalRevenueChart();
    QChart* createAverageSellingPriceChart();
    QChart* createTotalItemsInInventoryChart();
    QChart* createTotalUnitsSoldChart();
    QChart* createLowStockItemsChart();
    QChart* createTotalBuyingPriceChart();
    QChart* createSalesOverTimeChart();

private:
    ItemManager* itemManager;
};

#endif // STATISTICS_H
