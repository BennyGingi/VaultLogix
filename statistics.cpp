#include "statistics.h"
#include <QtCharts/QChart>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>
#include <QMap>
#include <algorithm>

Statistics::Statistics(ItemManager* itemManager, QObject* parent)
    : QObject(parent), itemManager(itemManager) {}

Statistics::~Statistics() {}

double Statistics::calculateTotalRevenue() {
    double totalRevenue = 0.0;
    for (const Item &item : itemManager->getItems()) {
        totalRevenue += item.sellingPrice * item.unitsSold;
    }
    return totalRevenue;
}

double Statistics::calculateAverageSellingPrice() {
    double totalRevenue = calculateTotalRevenue();
    int totalUnitsSold = calculateTotalUnitsSold();
    return totalUnitsSold ? totalRevenue / totalUnitsSold : 0.0;
}

int Statistics::calculateTotalItemsInInventory() {
    int totalItems = 0;
    for (const Item &item : itemManager->getItems()) {
        totalItems += item.unit;
    }
    return totalItems;
}

int Statistics::calculateTotalUnitsSold() {
    int totalUnitsSold = 0;
    for (const Item &item : itemManager->getItems()) {
        totalUnitsSold += item.unitsSold;
    }
    return totalUnitsSold;
}

double Statistics::calculateTotalBuyingPrice() {
    double totalBuyingPrice = 0.0;
    for (const Item &item : itemManager->getItems()) {
        totalBuyingPrice += item.buyingPrice * item.unit;
    }
    return totalBuyingPrice;
}

QList<QPair<QString, int>> Statistics::calculateLowStockItems(int threshold) {
    QList<QPair<QString, int>> lowStockItems;
    for (const Item &item : itemManager->getItems()) {
        if (item.unit <= threshold) {
            lowStockItems.append(qMakePair(item.name, item.unit));
        }
    }
    return lowStockItems;
}

QList<QPair<QDateTime, double>> Statistics::calculateSalesOverTime() {
    QList<QPair<QDateTime, double>> salesOverTime;
    for (const Item &item : itemManager->getItems()) {
        if (item.unitsSold > 0) {
            salesOverTime.append(qMakePair(item.dateSold, item.sellingPrice * item.unitsSold));
        }
    }
    std::sort(salesOverTime.begin(), salesOverTime.end(), [](const QPair<QDateTime, double> &a, const QPair<QDateTime, double> &b) {
        return a.first < b.first;
    });
    return salesOverTime;
}

QList<QPair<QString, QPair<double, int>>> Statistics::calculateTop5BestSellers() {
    QList<QPair<QString, QPair<double, int>>> bestSellers;
    for (const Item &item : itemManager->getItems()) {
        bestSellers.append(qMakePair(item.name, qMakePair(item.sellingPrice, item.unitsSold)));
    }
    std::sort(bestSellers.begin(), bestSellers.end(), [](const QPair<QString, QPair<double, int>> &a, const QPair<QString, QPair<double, int>> &b) {
        return a.second.second > b.second.second; // Sort by units sold
    });
    return bestSellers.mid(0, 5);
}

QChart* Statistics::createTop5BestSellersChart() {
    QList<QPair<QString, QPair<double, int>>> bestSellers = calculateTop5BestSellers();
    QBarSet *setPrice = new QBarSet("Selling Price");
    QBarSet *setUnits = new QBarSet("Units Sold");

    QStringList categories;
    for (const auto &seller : bestSellers) {
        categories << seller.first;
        *setPrice << seller.second.first;
        *setUnits << seller.second.second;
    }

    QBarSeries *series = new QBarSeries();
    series->append(setPrice);
    series->append(setUnits);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Top 5 Best Seller Items by Selling Price and Units Sold");

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Value");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    return chart;
}

QChart* Statistics::createTotalRevenueChart() {
    double totalRevenue = calculateTotalRevenue();
    QPieSeries *series = new QPieSeries();
    series->append("Total Revenue", totalRevenue);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Total Revenue");

    return chart;
}

QChart* Statistics::createAverageSellingPriceChart() {
    double averageSellingPrice = calculateAverageSellingPrice();
    QPieSeries *series = new QPieSeries();
    series->append("Average Selling Price", averageSellingPrice);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Average Selling Price");

    return chart;
}

QChart* Statistics::createTotalItemsInInventoryChart() {
    int totalItemsInInventory = calculateTotalItemsInInventory();
    QPieSeries *series = new QPieSeries();
    series->append("Total Items in Inventory", totalItemsInInventory);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Total Items in Inventory");

    return chart;
}

QChart* Statistics::createTotalUnitsSoldChart() {
    int totalUnitsSold = calculateTotalUnitsSold();
    QPieSeries *series = new QPieSeries();
    series->append("Total Units Sold", totalUnitsSold);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Total Units Sold");

    return chart;
}

QChart* Statistics::createLowStockItemsChart() {
    QList<QPair<QString, int>> lowStockItems = calculateLowStockItems(5); // Threshold can be adjusted
    QBarSet *set = new QBarSet("Units");

    QStringList categories;
    for (const auto &item : lowStockItems) {
        categories << item.first;
        *set << item.second;
    }

    QBarSeries *series = new QBarSeries();
    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Low Stock Items");

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Units");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    return chart;
}

QChart* Statistics::createTotalBuyingPriceChart() {
    double totalBuyingPrice = calculateTotalBuyingPrice();
    QPieSeries *series = new QPieSeries();
    series->append("Total Buying Price", totalBuyingPrice);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Total Buying Price");

    return chart;
}

QChart* Statistics::createSalesOverTimeChart() {
    QList<QPair<QDateTime, double>> salesOverTime = calculateSalesOverTime();
    QLineSeries *series = new QLineSeries();

    for (const auto &data : salesOverTime) {
        series->append(data.first.toMSecsSinceEpoch(), data.second);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Sales Over Time");

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setFormat("dd-MM-yyyy");
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Revenue");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    return chart;
}
