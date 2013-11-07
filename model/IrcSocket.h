#ifndef IRCSOCKET_H
#define IRCSOCKET_H

#include <QObject>
#include <QMap>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostInfo>
#include <QTextDocument>
#include <QTextCursor>

#include "IrcChannel.h"
#include "IrcUser.h"
#include "IrcMessage.h"

#define MAX_MSG_SIZE 1024 // bytes

class IrcSocket : public QObject
{
    Q_OBJECT
public:
    explicit IrcSocket(QObject *parent = 0);
    ~IrcSocket();

    void connect(const QString& name,
		 const QString& host,
		 const QString& nick);
    void disconnect(QString message = "");
    void write(IrcMessage *message);
    QTextDocument* networkText();
    QTextDocument* channelDocument(const QString& channel);
    QStringList channelNicks(const QString& channel);
    void createChannel(const QString& channel);
    void destroyChannel(const QString& channel);
    void setDocumentFont(const QFont& font);
    IrcUser* identity();

signals:
    void connected();
    void disconnected(QString network);
    void channelCreated(QString network, QString channel);
    void channelDestroyed(QString network, QString channel);
    void userListChanged(QString network, QString channel);
    void documentUpdated(QString network, QString channel);
    void documentUpdated(QString network);

private slots:
    void onConnect();
    void onReadyRead();

private:
    QString operatingSystem();

    QTcpSocket *m_socket;
    IrcUser *m_identity;
    QString m_serverName;
    int m_port;
    QTextDocument *m_document;
    QMap<QString, IrcChannel*> m_channels;
};

#endif // IRCSOCKET_H
