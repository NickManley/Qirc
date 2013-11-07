#ifndef IRCMESSAGE_H
#define IRCMESSAGE_H

#include <QObject>
#include <QStringList>
#include "IrcUser.h"

class IrcMessage : public QObject
{
    Q_OBJECT
public:
    explicit IrcMessage(QObject *parent = 0);
    explicit IrcMessage(const QString& rhs,
			QObject *parent = 0);
    IrcMessage& operator=(const IrcMessage &rhs);
    IrcMessage& operator=(QString& rhs);
    ~IrcMessage();

    void setMessage(const QString& theMessage);
    void setPrefix(QString& thePrefix);
    void setCommand(const QString& cmd);
    void appendArgument(QString arg);
    void appendArguments(QStringList args);
    bool hasPrefix() const;
    QString command() const;
    QStringList arguments() const;
    QString argument(int index) const;
    QString prefix() const;
    QString suffix() const;
    QString toString() const;
    int numArgs() const;
    void clear();

signals:

public slots:

private:
    void parseServerMessage(const QString& theMessage);

    QString m_prefix;
    QString m_command;
    QStringList m_arguments;
};

#endif // IRCMESSAGE_H
