#include "loginwindow.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QPainter>
#include <QDebug>

LoginWindow::LoginWindow(QWidget *parent) : QDialog(parent), failedLoginAttempts(0) {
    setWindowIcon(QIcon(":/assets/loginwindow_logoPNG.png"));

    setupUi();
    attemptAutoLogin();
    loadFreezeState();
}

LoginWindow::~LoginWindow() {
    saveFreezeState();
}

void LoginWindow::setupUi() {
    setWindowTitle("Login");
    setAttribute(Qt::WA_StyledBackground, true);

    layout = new QVBoxLayout(this);
    usernameEdit = new QLineEdit(this);
    passwordEdit = new QLineEdit(this);
    loginButton = new QPushButton("Login", this);
    rememberMeCheckBox = new QCheckBox("Remember Me", this);
    statusLabel = new QLabel(this);

    usernameEdit->setPlaceholderText("Enter your username");
    passwordEdit->setPlaceholderText("Enter your password");
    passwordEdit->setEchoMode(QLineEdit::Password);

    layout->addWidget(usernameEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(rememberMeCheckBox);
    layout->addWidget(loginButton);
    layout->addWidget(statusLabel);

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::login);
}

void LoginWindow::attemptAutoLogin() {
    QSettings settings;
    if (settings.value("autoLogin", false).toBool()) {
        QString username = settings.value("autoLoginUsername").toString();
        QString name = settings.value("autoLoginName").toString();
        QString role = settings.value("autoLoginRole").toString();
        QDateTime lastLoginTime = settings.value("autoLoginTime").toDateTime();

        if (lastLoginTime.isValid() && lastLoginTime.secsTo(QDateTime::currentDateTime()) <= 3600) {
            emit loginSuccessful(username, name, role, true);
            accept();
        }
    }
}

void LoginWindow::login() {
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();

    QString userDataFilePath = QCoreApplication::applicationDirPath() + "/data/user.json";
    QFile file(userDataFilePath);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Failed to open user data file.");
        return;
    }

    QByteArray userData = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(userData);
    QJsonArray users = doc.array();

    bool loginSuccess = false;
    QString name;
    QString role;

    for (const QJsonValue &value : users) {
        QJsonObject obj = value.toObject();
        if (obj["username"].toString() == username) {
            if (obj["password"].toString() == QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex())) {
                loginSuccess = true;
                name = obj["name"].toString();
                role = obj["role"].toString();
                break;
            } else {
                QMessageBox::warning(this, "Login Failed", "Invalid password.");
                failedLoginAttempts++;
                freezeLoginWindow();
                return;
            }
        }
    }

    if (loginSuccess) {
        emit loginSuccessful(username, name, role, rememberMeCheckBox->isChecked());
        failedLoginAttempts = 0;
        saveFreezeState();
        accept();
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username.");
        failedLoginAttempts++;
        freezeLoginWindow();
    }
}

bool LoginWindow::isRememberMeChecked() const {
    return rememberMeCheckBox->isChecked();
}

void LoginWindow::paintEvent(QPaintEvent *event) {
    QDialog::paintEvent(event);
}

void LoginWindow::freezeLoginWindow() {
    int freezeDuration = 0;
    if (failedLoginAttempts == 10) {
        freezeDuration = 900; // 15 minutes
    } else if (failedLoginAttempts == 5) {
        freezeDuration = 600; // 10 minutes
    } else if (failedLoginAttempts == 3) {
        freezeDuration = 300; // 5 minutes
    }

    if (freezeDuration > 0) {
        loginButton->setEnabled(false);
        unfreezeTime = QDateTime::currentDateTime().addSecs(freezeDuration);
        saveFreezeState();

        // Start the countdown timer
        QTimer *countdownTimer = new QTimer(this);
        connect(countdownTimer, &QTimer::timeout, this, [this, countdownTimer, freezeDuration]() {
            int remainingTime = QDateTime::currentDateTime().secsTo(unfreezeTime);
            if (remainingTime > 0) {
                int minutes = remainingTime / 60;
                int seconds = remainingTime % 60;
                qDebug() << "Remaining time until unfreeze:" << minutes << "minutes" << seconds << "seconds";
            } else {
                countdownTimer->stop();
                unfreezeLoginWindow();
            }
        });
        countdownTimer->start(1000); // Update countdown every second
    }
}

void LoginWindow::unfreezeLoginWindow() {
    loginButton->setEnabled(true);
    unfreezeTime = QDateTime();
    saveFreezeState();

    qDebug() << "Login window unfrozen";
}

void LoginWindow::saveFreezeState() {
    QSettings settings;
    settings.setValue("failedLoginAttempts", failedLoginAttempts);
    settings.setValue("unfreezeTime", unfreezeTime);
}

void LoginWindow::loadFreezeState() {
    QSettings settings;
    failedLoginAttempts = settings.value("failedLoginAttempts", 0).toInt();
    unfreezeTime = settings.value("unfreezeTime").toDateTime();

    if (unfreezeTime.isValid() && QDateTime::currentDateTime() < unfreezeTime) {
        loginButton->setEnabled(false);
        QTimer::singleShot(QDateTime::currentDateTime().msecsTo(unfreezeTime), this, &LoginWindow::unfreezeLoginWindow);
    } else {
        loginButton->setEnabled(true);
    }
}
