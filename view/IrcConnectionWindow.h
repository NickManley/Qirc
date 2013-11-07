#ifndef IRCCONNECTIONWINDOW_H
#define IRCCONNECTIONWINDOW_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QGridLayout>

class IrcConnectionWindow : public QWidget
{
    Q_OBJECT

public:
    static IrcConnectionWindow *instance();

signals:
    void cancelButtonPressed();
    void connectButtonPressed(const QString& serverName,
	    const QString& serverAddress, const QString& nick);

private slots:
    void onConnectButtonPressed();
    void onCancelButtonPressed();
    void onServerSelected(QTreeWidgetItem* item);
    void onInformationChanged();

protected:
    explicit IrcConnectionWindow(QWidget *parent = 0);

private:
    void generateServerList();

    QBoxLayout *layout;
    QSplitter *hbox;
    QTreeWidget *ircNetworkTreeView;

    QLabel *ircServerNameLabel;
    QLineEdit *ircServerNameInput;
    QLabel *ircServerAddressLabel;
    QLineEdit *ircServerAddressInput;
    QLabel *ircNicknameLabel;
    QLineEdit *ircNicknameInput;
    QPushButton *ircConnectButton;
    QPushButton *ircCancelButton;

    QWidget *ircConnectionForm;
    QGridLayout *ircConnectionFormLayout;

    static IrcConnectionWindow *m_instance;
};

extern IrcConnectionWindow *ircConnectionWindow;

#endif // IRCCONNECTIONWINDOW_H
