#ifndef IRCMAINWINDOW_H
#define IRCMAINWINDOW_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QHeaderView>
#include <QtGui/QFont>
#include <QtWidgets/QMessageBox>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QStatusBar>

#include "IrcController.h"

class IrcMainWindow : public QWidget
{
    Q_OBJECT

public:
    IrcMainWindow(QWidget *parent = 0);
    ~IrcMainWindow();
    static IrcMainWindow *instance();

    void setupStatusBar();

    void addNetwork(const QString& name);
    void removeNetwork(const QString& network);
    void addChannel(const QString& network, const QString& name);
    void removeChannel(const QString& network, const QString& channel);
    void selectNetwork(const QString& name);
    void selectChannel(const QString& network, const QString& channel);
    void setDocument(QTextDocument* document);
    void setUserList(QStringList users);
    void clearUserList();
    void clearInputText();
    void highlightNetwork(QString network);
    void highlightChannel(QString network, QString channel);

signals:
    void selectionChanged(const QString& network,
			     const QString& name = "");
    void inputTextEntered(const QString& text);
    void userDoubleClicked(const QString& network, const QString& nick);

private slots:
    void onSelectionChanged();
    void onInputTextEntered();
    void onDocumentUpdated();
    void onUserDoubleClicked(QListWidgetItem *item);
    void onActionCloseChannel();
    void onActionViewHelp();
    void onActionViewLicense();
    void onActionViewAbout();
    void onActionConnect();
    void onActionDisconnect();
    void onActionExit();
    void onActionJoinChannel();
    void onActionChangeNick();

protected:
     void closeEvent( QCloseEvent* );

private:
    QMenuBar *ircMenuBar;
    QTreeWidget *ircSessionTreeView;
    QTextBrowser *ircTextView;
    QLineEdit *ircTextInput;
    QListWidget *ircUserListView;
    // Single container for ircTextView and ircTextInput.
    // Used for layout purposes.
    QWidget *ircChatView;
    // Vertical layout for ircChatArea
    QVBoxLayout *vboxChat;
    QVBoxLayout *vbox;
    // Horizontal layout for the main window
    QSplitter *hbox;
    QBoxLayout *layout;
    // Actions
    QAction *actCloseChannel;
    QAction *actViewHelp;
    QAction *actViewLicense;
    QAction *actViewAbout;
    QAction *actConnect;
    QAction *actDisconnect;
    QAction *actExit;
    QAction *actJoinChannel;
    QAction *actChangeNick;
    // Menus
    QMenu *menuServer;
    QMenu *menuChannel;
    QMenu *menuUser;
    QMenu *menuBookmarks;
    QMenu *menuHelp;
    // status bar
    QStatusBar *ircStatusBar;

    static IrcMainWindow *m_instance;
};

extern IrcMainWindow *ircMainWindow;

#endif // IRCMAINWINDOW_H
