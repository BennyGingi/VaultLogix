#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QDateTime>

struct User {
    QString username;
    QString password;
    QString name;
    QString email;
    QString role; // "admin" or "user"
    QDateTime creationDate;
    QDateTime lastLogin;
    QString status; // "active" or "inactive"
};

class UserManager : public QObject {
    Q_OBJECT

public:
    explicit UserManager(QObject *parent = nullptr);
    ~UserManager();

    bool addUser(const User &user);
    bool editUser(const QString &username, const User &updatedUser);
    bool removeUser(const QString &username);
    QList<User> getUsers() const;
    QString getUserRole(const QString &username) const; // Retrieve the user role
    QString getUserName(const QString &username) const; // Retrieve the user name
    User getUserByUsername(const QString &username) const; // Retrieve the user by username

private:
    QList<User> users;
    QString userDataFilePath; // Path where the user data is saved

    void loadUsers();
    void saveUsers();
};

#endif // USERMANAGER_H
