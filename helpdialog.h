#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QTextBrowser>
#include <QPushButton>
#include <QVBoxLayout>

class HelpDialog : public QDialog {
    Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog();

private:
    QTextBrowser *helpTextBrowser;
    QPushButton *closeButton;

    void setupUi();
    void populateHelpText();
};

#endif // HELPDIALOG_H
