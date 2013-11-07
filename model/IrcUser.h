#ifndef IRCUSER_H
#define IRCUSER_H

#include <QObject>
#include <QString>

class IrcUser : public QObject
{
    Q_OBJECT
public:
    explicit IrcUser(QObject *parent = 0);
    IrcUser(const QString& theUser, QObject *parent = 0);
    IrcUser& operator=(const IrcUser& rhs);
    IrcUser& operator=(const QString& rhs);
    bool operator==(const IrcUser &rhs) const;
    bool operator!=(const IrcUser &rhs) const;
    bool operator<(const IrcUser &rhs) const;
    bool operator<=(const IrcUser &rhs) const;
    bool operator>(const IrcUser &rhs) const;
    bool operator>=(const IrcUser &rhs) const;

    void setWithString(const QString& theData);
    void setNick(const QString& theNick);
    void setUser(const QString& theUser);
    void setHost(const QString& theHost);

    void setVoiced(bool hasVoice);
    void setOperator(bool hasOps);
    void setVisibile(bool canSee);

    QString nick() const;
    QString user() const;
    QString host() const;
    bool isVoiced() const;
    bool isOperator() const;
    bool isVisible() const;

    QString toString() const;
    void clear();
    bool isEmpty() const;

signals:

public slots:

private:
    QString m_nick;
    QString m_user;
    QString m_host;
    bool m_isVoiced;
    bool m_isOperator;
    bool m_isVisible;
};

#endif // IRCUSER_H
