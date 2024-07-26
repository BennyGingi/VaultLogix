#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QSettings>
#include <QTimer>
#include <QDateTime>

class LoginWindow : public QDialog {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

    bool isRememberMeChecked() const;

signals:
    void loginSuccessful(const QString &username, const QString &name, const QString &role, bool rememberMe);

private slots:
    void login();
    void freezeLoginWindow();
    void unfreezeLoginWindow();

private:
    QVBoxLayout *layout;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QCheckBox *rememberMeCheckBox;
    QLabel *statusLabel;

    int failedLoginAttempts;
    QDateTime unfreezeTime;

    void setupUi();
    void attemptAutoLogin();
    void saveFreezeState();
    void loadFreezeState();

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // LOGINWINDOW_H
