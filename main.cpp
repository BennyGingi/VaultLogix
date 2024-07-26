#include <QApplication>
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include "loginwindow.h"
#include "mainwindow.h"

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    Q_UNUSED(type);
    Q_UNUSED(context);
    QFile logFile("app.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream logStream(&logFile);
        logStream << msg << Qt::endl;
        logStream.flush();
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Define settings scope
    QCoreApplication::setOrganizationName("MyOrganization");
    QCoreApplication::setApplicationName("VaultLogix");

    // Redirect console output to a log file
    qInstallMessageHandler(customMessageHandler);

    QSettings settings;
    bool autoLogin = settings.value("autoLogin", false).toBool();
    QDateTime lastLoginTime = settings.value("autoLoginTime").toDateTime();
    int secsSinceLastLogin = lastLoginTime.secsTo(QDateTime::currentDateTime());
    qDebug() << "Auto-login check:" << autoLogin << "Seconds since last login:" << secsSinceLastLogin;

    if (autoLogin && secsSinceLastLogin <= 3600) { // 3600 seconds = 1 hour
        QString username = settings.value("autoLoginUsername", "").toString();
        QString name = settings.value("autoLoginName", "").toString();
        QString role = settings.value("autoLoginRole", "user").toString();
        qDebug() << "Logging in automatically for user:" << username;
        MainWindow *mainWindow = new MainWindow(nullptr, role, username);
        mainWindow->show();
    } else {
        qDebug() << "Showing login window...";
        LoginWindow login;
        QObject::connect(&login, &LoginWindow::loginSuccessful, [&](const QString &username, const QString &name, const QString &role, bool rememberMe) {
            settings.setValue("autoLogin", rememberMe);
            if (rememberMe) {
                settings.setValue("autoLoginUsername", username);
                settings.setValue("autoLoginName", name);
                settings.setValue("autoLoginRole", role);
                settings.setValue("autoLoginTime", QDateTime::currentDateTime());
            } else {
                settings.remove("autoLoginUsername");
                settings.remove("autoLoginName");
                settings.remove("autoLoginRole");
                settings.remove("autoLoginTime");
            }
            MainWindow *mainWindow = new MainWindow(nullptr, role, username);
            mainWindow->show();
        });
        login.exec();
    }

    return app.exec();
}
