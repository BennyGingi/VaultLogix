#ifndef AUDITTRAIL_H
#define AUDITTRAIL_H

#include <QObject>
#include <QList>
#include <QString>
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>
#include <QTextDocument>
#include <QPrinter>
#include <QTextTable>
#include <QDir>
#include <ActiveQt/QAxObject>
#include <QFileDialog>
#include <QPageSize>
#include <QTextStream>

struct AuditEntry {
    QString action;
    QString username;
    QString itemName;
    double buyingPrice;
    double sellingPrice;
    int quantitySold;
    double totalRevenue;
    QDateTime dateAdded;
    QDateTime dateSold;
    QDateTime timestamp;
};

class AuditTrail : public QObject {
    Q_OBJECT

public:
    explicit AuditTrail(QObject *parent = nullptr);
    ~AuditTrail();

    void logAction(const QString &action, const QString &username, const QString &itemName,
                   double buyingPrice, double sellingPrice, int quantitySold, double totalRevenue,
                   const QDateTime &dateAdded, const QDateTime &dateSold);
    QList<AuditEntry> getAuditEntries() const;
    void exportToPDF(const QString &fileName);
    void exportToExcel(const QString &fileName);
    void exportToCSV(const QString &fileName);
    void printLogsToCommandWindow() const;

private:
    QList<AuditEntry> auditEntries;
    QString auditFilePath = "audittrail.json";
    void saveAuditTrail() const;
    void loadAuditTrail();
};

#endif // AUDITTRAIL_H
