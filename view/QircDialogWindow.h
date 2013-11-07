#ifndef QIRCDIALOGWINDOW_H
#define QIRCDIALOGWINDOW_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtCore/QDebug>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QLineEdit>

class QircDialogWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QircDialogWindow(QWidget *parent = 0);
    void showAbout();
    void showLicense();
    void showHelp();
    void showJoinChannel();
    void showChangeNick();
    void centerDialog();
    ~QircDialogWindow();

signals:
    void inputTextEntered(const QString& text);

public slots:
    void onButtonPressed();

private:
    QLineEdit *lineEdit;
    QTextBrowser *textBrowser;
    QPushButton *button;
    QVBoxLayout *vbox;
};

#endif // QIRCDIALOGWINDOW_H
