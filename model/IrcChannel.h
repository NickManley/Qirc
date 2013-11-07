#ifndef IRCCHANNEL_H
#define IRCCHANNEL_H

#include <QObject>
#include <QMap>
#include <QTextDocument>
#include <QTextCursor>
#include "IrcUser.h"
#include "IrcMessage.h"

class IrcChannel : public QObject
{
    Q_OBJECT
public:
    explicit IrcChannel(const QString& name, QObject *parent = 0);
    ~IrcChannel();
    IrcChannel& operator=(const IrcChannel &rhs);
    bool operator==(const IrcChannel &rhs) const;
    bool operator!=(const IrcChannel &rhs) const;
    bool operator<(const IrcChannel &rhs) const;
    bool operator<=(const IrcChannel &rhs) const;
    bool operator>(const IrcChannel &rhs) const;
    bool operator>=(const IrcChannel &rhs) const;
    void setName(QString& theName);
    void addUser(QString& user);
    void removeUser(const QString& nick);
    bool renameUser(QString oldNick, QString newNick);
    bool hasUser(QString nick);
    void addText(QString theText);
    void setTopic(QString theTopic);
    void setKey(QString& theKey); // empty string removes key
    void setUserLimit(int theLimit); // negative value removes user limit
    void setPrivate(bool theBool);
    void setSecret(bool theBool);
    void setInviteOnly(bool theBool);
    void setExternal(bool theBool);
    void setModerated(bool theBool);
    void setDocumentFont(const QFont& font);

    QString name();
    QString topic();
    QString key();
    QTextDocument* document();
    IrcUser* user(QString nick);
    QStringList channelNicks();
    int userLimit();
    bool isPrivate();
    bool isSecret();
    bool isInviteOnly();
    bool isExternal();
    bool isModerated();
    bool isPrivateMessage();

signals:

public slots:

private:
    QString m_name;
    QMap<QString,IrcUser*> m_users;
    QString m_topic;
    QString m_key;
    int m_userLimit;
    QTextDocument* m_document;
    QTextCursor* m_textCursor;

    bool m_isPrivate;
    bool m_isSecret;
    bool m_isInviteOnly;
    bool m_isExternal;
    bool m_isModerated;
    bool m_hasUserLimit;
    bool m_hasKey;
};

#endif // IRCCHANNEL_H
