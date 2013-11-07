#include "IrcUser.h"
#include <QDebug>

IrcUser::IrcUser(QObject *parent) :
    QObject(parent)
{
    m_isVoiced = false;
    m_isOperator = false;
    m_isVisible = false;
}

IrcUser::IrcUser(const QString& theUser, QObject *parent) :
    QObject(parent)
{
    m_isVoiced = false;
    m_isOperator = false;
    m_isVisible = false;
    this->setWithString(theUser);
}

IrcUser& IrcUser::operator=(const IrcUser &rhs)
{
    if(this == &rhs)
	return *this;

    this->m_nick = rhs.m_nick;
    this->m_user = rhs.m_user;
    this->m_host = rhs.m_host;
    this->m_isVoiced = rhs.m_isVoiced;
    this->m_isOperator = rhs.m_isOperator;
    this->m_isVisible = rhs.m_isVisible;

    return *this;
}

IrcUser& IrcUser::operator=(const QString& rhs)
{
    this->setWithString(rhs);
    return *this;
}

bool IrcUser::operator==(const IrcUser &rhs) const
{
    if(this->m_nick == rhs.m_nick)
	return true;
    else
	return false;
}

bool IrcUser::operator!=(const IrcUser &rhs) const
{
    return !(*this == rhs);
}

bool IrcUser::operator<(const IrcUser &rhs) const
{
    if(rhs.m_isOperator && !this->m_isOperator)
	return true;
    if(this->m_isOperator && !rhs.m_isOperator)
	return false;
    if(rhs.m_isVoiced && !this->m_isVoiced)
	return true;
    if(this->m_isVoiced && !rhs.m_isVoiced)
	return false;
    return this->m_nick < rhs.m_nick;
}

bool IrcUser::operator<=(const IrcUser &rhs) const
{
    return ((*this < rhs) || (*this == rhs));
}

bool IrcUser::operator>(const IrcUser &rhs) const
{
    return !((*this < rhs) || (*this == rhs));
}

bool IrcUser::operator>=(const IrcUser &rhs) const
{
    return (*this > rhs) || (*this == rhs);
}

void IrcUser::setWithString(const QString& theData)
{
    int i;
    QString nick, user, host;
    for(i=0; i<theData.length(); i++)
    {
	if(theData.at(i) != '!')
	    nick.append(theData.at(i));
    }
    for(i=i+1; i<theData.length(); i++)
    {
	if(theData.at(i) != '@')
	    user.append(theData.at(i));
    }
    for(i=i+1; i<theData.length(); i++)
    {
	host.append(theData.at(i));
    }
    this->setNick(nick);
    this->setUser(user);
    this->setHost(host);
}

void IrcUser::setNick(const QString& theNick)
{
    //Q_ASSERT(theNick.length() <= 9);
    m_nick = theNick;
    if(m_nick.at(0) == '+')
    {
	m_isVoiced = true;
	m_nick.remove(0, 1);
    }
    else if(m_nick.at(0) == '@')
    {
	m_isOperator = true;
	m_nick.remove(0, 1);
    }
}

void IrcUser::setUser(const QString& theUser)
{
    m_user = theUser;
}

void IrcUser::setHost(const QString& theHost)
{
    m_host = theHost;
}

void IrcUser::setVoiced(bool hasVoice)
{
    m_isVoiced = hasVoice;
}

void IrcUser::setOperator(bool hasOps)
{
    m_isOperator = hasOps;
}

void IrcUser::setVisibile(bool canSee)
{
    m_isVisible = canSee;
}

QString IrcUser::nick() const
{
    return m_nick;
}

QString IrcUser::user() const
{
    return m_user;
}

QString IrcUser::host() const
{
    return m_host;
}

bool IrcUser::isVoiced() const
{
    return m_isVoiced;
}

bool IrcUser::isOperator() const
{
    return m_isOperator;
}

bool IrcUser::isVisible() const
{
    return m_isVisible;
}

QString IrcUser::toString() const
{
    return QString(m_nick + "!" + m_user + "@" + m_host);
}

void IrcUser::clear()
{
    m_nick.clear();
    m_user.clear();
    m_host.clear();
}

bool IrcUser::isEmpty() const
{
    return m_nick.isEmpty() &&
	   m_user.isEmpty() &&
	   m_host.isEmpty();
}
