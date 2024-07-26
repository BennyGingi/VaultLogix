#include "usermanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QCryptographicHash>
#include <QDebug>
#include <QCoreApplication>

UserManager::UserManager(QObject *parent) : QObject(parent) {
    // Define the path to the user.json file based on the application directory
    QString appDir = QCoreApplication::applicationDirPath();
    userDataFilePath = appDir + "/data/user.json";

    qDebug() << "UserManager using user data file path:" << userDataFilePath;
    loadUsers(); // Load users at initialization
}

UserManager::~UserManager() {
    saveUsers(); // Save users upon destruction
}

bool UserManager::addUser(const User &user) {
    for (const User &existingUser : users) {
        if (existingUser.username == user.username) {
            return false; // Username already exists
        }
    }
    User newUser = user;
    newUser.password = QString(QCryptographicHash::hash(user.password.toUtf8(), QCryptographicHash::Sha256).toHex());
    newUser.creationDate = QDateTime::currentDateTime();
    newUser.status = "active";
    users.append(newUser);
    saveUsers();
    return true;
}

bool UserManager::editUser(const QString &username, const User &updatedUser) {
    for (User &user : users) {
        if (user.username == username) {
            user.name = updatedUser.name;
            user.email = updatedUser.email;
            user.role = updatedUser.role;
            if (!updatedUser.password.isEmpty()) {
                user.password = QString(QCryptographicHash::hash(updatedUser.password.toUtf8(), QCryptographicHash::Sha256).toHex());
            }
            user.status = updatedUser.status;
            saveUsers();
            return true;
        }
    }
    return false;
}

bool UserManager::removeUser(const QString &username) {
    auto it = std::find_if(users.begin(), users.end(), [&username](const User& user) {
        return user.username == username;
    });
    if (it != users.end()) {
        users.erase(it);
        saveUsers();
        return true;
    }
    return false;
}

QList<User> UserManager::getUsers() const {
    return users;
}

void UserManager::loadUsers() {
    QFile file(userDataFilePath);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray jsonArray = doc.array();
        users.clear();
        for (const QJsonValue &value : jsonArray) {
            QJsonObject obj = value.toObject();
            User user;
            user.username = obj["username"].toString();
            user.password = obj["password"].toString();
            user.name = obj["name"].toString();
            user.email = obj["email"].toString();
            user.role = obj["role"].toString();
            user.creationDate = QDateTime::fromString(obj["creationDate"].toString(), Qt::ISODate);
            user.lastLogin = QDateTime::fromString(obj["lastLogin"].toString(), Qt::ISODate);
            user.status = obj["status"].toString();
            users.append(user);
        }
        file.close();
    }
}

void UserManager::saveUsers() {
    QFile file(userDataFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonArray jsonArray;
        for (const User &user : users) {
            QJsonObject obj;
            obj["username"] = user.username;
            obj["password"] = user.password;
            obj["name"] = user.name;
            obj["email"] = user.email;
            obj["role"] = user.role;
            obj["creationDate"] = user.creationDate.toString(Qt::ISODate);
            obj["lastLogin"] = user.lastLogin.isValid() ? user.lastLogin.toString(Qt::ISODate) : "";
            obj["status"] = user.status;
            jsonArray.append(obj);
        }
        QJsonDocument doc(jsonArray);
        file.write(doc.toJson());
        file.close();
    }
}

QString UserManager::getUserRole(const QString &username) const {
    for (const User &user : users) {
        if (user.username == username) {
            return user.role;
        }
    }
    return QString(); // Return empty string if not found
}

QString UserManager::getUserName(const QString &username) const {
    for (const User &user : users) {
        if (user.username == username) {
            return user.name;
        }
    }
    return QString(); // Return empty string if not found
}

User UserManager::getUserByUsername(const QString &username) const {
    for (const User &user : users) {
        if (user.username == username) {
            return user;
        }
    }
    return User(); // Return an empty User object if not found
}
