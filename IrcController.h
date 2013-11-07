#ifndef IRCCONTROLLER_H
#define IRCCONTROLLER_H

#include <QObject>
#include <QMap>
#include "model/IrcSocket.h"

#include "view/IrcConnectionWindow.h"
#include "view/IrcMainWindow.h"
#include "view/QircDialogWindow.h"

// Singleton
class IrcController : public QObject
{
    Q_OBJECT
public:
    static IrcController *instance();
    ~IrcController();
signals:

public slots:
    // Connection Window slots
    void onCancelButtonPressed();
    void onConnectButtonPressed(QString serverName,
	    QString serverAddress, QString nick);

    // Main Window slots
    void onSelectionChanged(const QString& network,
			     const QString& name = "");
    void onInputTextEntered(const QString& text);
    void onUserDoubleClicked(const QString& network, const QString& nick);

    // Socket slots
    void onChannelCreated(QString network, QString channel);
    void onChannelDestroyed(QString network, QString channel);
    void onUserListChanged(QString network, QString channel);
    void onDocumentUpdated(QString network, QString channel);
    void onDocumentUpdated(QString network);
    void onDisconnected(QString network);

protected:
    explicit IrcController(QObject *parent = 0);
    QTextDocument* getDocument(QString networkName);
    QTextDocument* getDocument(QString networkName, QString name);
    void createChannel(QString network, QString nick);
    void setDocumentFont(const QFont& font);

private:
    static IrcController *m_instance;
    QircDialogWindow *dw;

    QMap<QString, IrcSocket*> m_sockets;
    QString m_network;
    QString m_channel;
};

extern IrcController *ircController;

#endif // IRCCONTROLLER_H
