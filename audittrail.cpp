#include "audittrail.h"

AuditTrail::AuditTrail(QObject *parent) : QObject(parent) {
    loadAuditTrail();
}

AuditTrail::~AuditTrail() {
    saveAuditTrail();
}

void AuditTrail::logAction(const QString &action, const QString &username, const QString &itemName,
                           double buyingPrice, double sellingPrice, int quantitySold, double totalRevenue,
                           const QDateTime &dateAdded, const QDateTime &dateSold) {
    AuditEntry entry = {action, username, itemName, buyingPrice, sellingPrice, quantitySold, totalRevenue, dateAdded, dateSold, QDateTime::currentDateTime()};
    auditEntries.append(entry);
    (void)QtConcurrent::run([this]() { saveAuditTrail(); }); // To handle the warning for ignoring the result
}

QList<AuditEntry> AuditTrail::getAuditEntries() const {
    return auditEntries;
}

void AuditTrail::exportToPDF(const QString &fileName) {
    QTextDocument document;
    QTextCursor cursor(&document);
    QTextTable *table = cursor.insertTable(auditEntries.size() + 1, 10);

    QStringList headers = {"Action", "Username", "Item", "Buying Price", "Selling Price", "Quantity Sold", "Total Revenue", "Date Added", "Date Sold", "Timestamp"};
    for (int i = 0; i < headers.size(); ++i) {
        table->cellAt(0, i).firstCursorPosition().insertText(headers[i]);
    }

    for (int row = 0; row < auditEntries.size(); ++row) {
        const AuditEntry &entry = auditEntries[row];
        table->cellAt(row + 1, 0).firstCursorPosition().insertText(entry.action);
        table->cellAt(row + 1, 1).firstCursorPosition().insertText(entry.username);
        table->cellAt(row + 1, 2).firstCursorPosition().insertText(entry.itemName);
        table->cellAt(row + 1, 3).firstCursorPosition().insertText(QString::number(entry.buyingPrice));
        table->cellAt(row + 1, 4).firstCursorPosition().insertText(QString::number(entry.sellingPrice));
        table->cellAt(row + 1, 5).firstCursorPosition().insertText(QString::number(entry.quantitySold));
        table->cellAt(row + 1, 6).firstCursorPosition().insertText(QString::number(entry.totalRevenue));
        table->cellAt(row + 1, 7).firstCursorPosition().insertText(entry.dateAdded.toString(Qt::ISODate));
        table->cellAt(row + 1, 8).firstCursorPosition().insertText(entry.dateSold.toString(Qt::ISODate));
        table->cellAt(row + 1, 9).firstCursorPosition().insertText(entry.timestamp.toString(Qt::ISODate));
    }

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    document.print(&printer);
}

void AuditTrail::exportToExcel(const QString &fileName) {
    QAxObject *excel = new QAxObject("Excel.Application", nullptr);
    excel->setProperty("Visible", false);
    QAxObject *workbooks = excel->querySubObject("Workbooks");
    QAxObject *workbook = workbooks->querySubObject("Add");
    QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);

    QStringList headers = {"Action", "Username", "Item", "Buying Price", "Selling Price", "Quantity Sold", "Total Revenue", "Date Added", "Date Sold", "Timestamp"};
    for (int i = 0; i < headers.size(); ++i) {
        QAxObject *cell = worksheet->querySubObject("Cells(int,int)", 1, i + 1);
        cell->setProperty("Value", headers[i]);
    }

    for (int row = 0; row < auditEntries.size(); ++row) {
        const AuditEntry &entry = auditEntries[row];
        worksheet->querySubObject("Cells(int,int)", row + 2, 1)->setProperty("Value", entry.action);
        worksheet->querySubObject("Cells(int,int)", row + 2, 2)->setProperty("Value", entry.username);
        worksheet->querySubObject("Cells(int,int)", row + 2, 3)->setProperty("Value", entry.itemName);
        worksheet->querySubObject("Cells(int,int)", row + 2, 4)->setProperty("Value", entry.buyingPrice);
        worksheet->querySubObject("Cells(int,int)", row + 2, 5)->setProperty("Value", entry.sellingPrice);
        worksheet->querySubObject("Cells(int,int)", row + 2, 6)->setProperty("Value", entry.quantitySold);
        worksheet->querySubObject("Cells(int,int)", row + 2, 7)->setProperty("Value", entry.totalRevenue);
        worksheet->querySubObject("Cells(int,int)", row + 2, 8)->setProperty("Value", entry.dateAdded.toString(Qt::ISODate));
        worksheet->querySubObject("Cells(int,int)", row + 2, 9)->setProperty("Value", entry.dateSold.toString(Qt::ISODate));
        worksheet->querySubObject("Cells(int,int)", row + 2, 10)->setProperty("Value", entry.timestamp.toString(Qt::ISODate));
    }

    workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(fileName));
    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");
    delete excel;
}

void AuditTrail::exportToCSV(const QString &fileName) {
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Action,Username,Item,Buying Price,Selling Price,Quantity Sold,Total Revenue,Date Added,Date Sold,Timestamp\n";
        for (const AuditEntry &entry : auditEntries) {
            out << entry.action << "," << entry.username << "," << entry.itemName << ","
                << entry.buyingPrice << "," << entry.sellingPrice << "," << entry.quantitySold << ","
                << entry.totalRevenue << "," << entry.dateAdded.toString(Qt::ISODate) << ","
                << entry.dateSold.toString(Qt::ISODate) << "," << entry.timestamp.toString(Qt::ISODate) << "\n";
        }
        file.close();
    }
}

void AuditTrail::printLogsToCommandWindow() const {
    for (const AuditEntry &entry : auditEntries) {
        qDebug() << entry.action << entry.username << entry.itemName << entry.buyingPrice << entry.sellingPrice
                 << entry.quantitySold << entry.totalRevenue << entry.dateAdded << entry.dateSold << entry.timestamp;
    }
}

void AuditTrail::saveAuditTrail() const {
    QJsonArray jsonArray;
    for (const AuditEntry &entry : auditEntries) {
        QJsonObject jsonObject;
        jsonObject["action"] = entry.action;
        jsonObject["username"] = entry.username;
        jsonObject["itemName"] = entry.itemName;
        jsonObject["buyingPrice"] = entry.buyingPrice;
        jsonObject["sellingPrice"] = entry.sellingPrice;
        jsonObject["quantitySold"] = entry.quantitySold;
        jsonObject["totalRevenue"] = entry.totalRevenue;
        jsonObject["dateAdded"] = entry.dateAdded.toString(Qt::ISODate);
        jsonObject["dateSold"] = entry.dateSold.toString(Qt::ISODate);
        jsonObject["timestamp"] = entry.timestamp.toString(Qt::ISODate);
        jsonArray.append(jsonObject);
    }
    QJsonDocument jsonDoc(jsonArray);
    QFile file(auditFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonDoc.toJson());
        file.close();
    }
}

void AuditTrail::loadAuditTrail() {
    QFile file(auditFilePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        QJsonDocument jsonDoc(QJsonDocument::fromJson(data));
        QJsonArray jsonArray = jsonDoc.array();

        auditEntries.clear();
        for (const QJsonValue &value : jsonArray) {
            QJsonObject jsonObject = value.toObject();
            AuditEntry entry;
            entry.action = jsonObject["action"].toString();
            entry.username = jsonObject["username"].toString();
            entry.itemName = jsonObject["itemName"].toString();
            entry.buyingPrice = jsonObject["buyingPrice"].toDouble();
            entry.sellingPrice = jsonObject["sellingPrice"].toDouble();
            entry.quantitySold = jsonObject["quantitySold"].toInt();
            entry.totalRevenue = jsonObject["totalRevenue"].toDouble();
            entry.dateAdded = QDateTime::fromString(jsonObject["dateAdded"].toString(), Qt::ISODate);
            entry.dateSold = QDateTime::fromString(jsonObject["dateSold"].toString(), Qt::ISODate);
            entry.timestamp = QDateTime::fromString(jsonObject["timestamp"].toString(), Qt::ISODate);
            auditEntries.append(entry);
        }
    }
}
