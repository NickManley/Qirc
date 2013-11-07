#include "IrcChannel.h"
#include <QDebug>

IrcChannel::IrcChannel(const QString& name, QObject *parent) :
    QObject(parent)
{
    m_userLimit = -1;
    m_isPrivate = false;
    m_isSecret = false;
    m_isInviteOnly = false;
    m_isExternal = false;
    m_isModerated = false;
    m_hasUserLimit = false;
    m_hasKey = false;
    m_document = new QTextDocument(this);
    m_textCursor = new QTextCursor(m_document);
    m_document->setDefaultFont(QFont("Consolas", 11));
    m_document->setMaximumBlockCount(1024);
    m_name = name;
}

IrcChannel::~IrcChannel()
{
    foreach(IrcUser* user, m_users)
    {
	if(user != NULL)
	    delete user;
    }
    delete m_textCursor;
}

IrcChannel& IrcChannel::operator=(const IrcChannel &rhs)
{
    if(this == &rhs)
	return *this;

    this->m_name = rhs.m_name;
    this->m_users = rhs.m_users;
    this->m_topic = rhs.m_topic;
    this->m_key = rhs.m_key;
    this->m_userLimit = rhs.m_userLimit;
    this->m_document = rhs.m_document;
    this->m_isPrivate = rhs.m_isPrivate;
    this->m_isSecret = rhs.m_isSecret;
    this->m_isInviteOnly = rhs.m_isInviteOnly;
    this->m_isExternal = rhs.m_isExternal;
    this->m_isModerated = rhs.m_isModerated;
    this->m_hasUserLimit = rhs.m_hasUserLimit;
    this->m_hasKey = rhs.m_hasKey;

    return *this;
}

bool IrcChannel::operator==(const IrcChannel &rhs) const
{
    if(this->m_name == rhs.m_name)
	return true;
    else
	return false;
}

bool IrcChannel::operator!=(const IrcChannel &rhs) const
{
    return !(*this == rhs);
}

bool IrcChannel::operator<(const IrcChannel &rhs) const
{
    if(this->m_name.at(0) == '&' &&
       rhs.m_name.at(0) == '#')
	return true;
    if(this->m_name.at(0) == '#' &&
       rhs.m_name.at(0) == '&')
	return false;
    return (this->m_name < rhs.m_name);
}

bool IrcChannel::operator<=(const IrcChannel &rhs) const
{
    if(this->m_name.at(0) == '&' &&
       rhs.m_name.at(0) == '#')
	return true;
    if(this->m_name.at(0) == '#' &&
       rhs.m_name.at(0) == '&')
	return false;
    return (this->m_name <= rhs.m_name);
}

bool IrcChannel::operator>(const IrcChannel &rhs) const
{
    return !((*this < rhs) || (*this == rhs));
}

bool IrcChannel::operator>=(const IrcChannel &rhs) const
{
    return (*this > rhs) || (*this == rhs);
}

void IrcChannel::setName(QString& theName)
{
    m_name = theName;
}

void IrcChannel::addUser(QString& user)
{
    IrcUser *usr = new IrcUser(user, this);
    m_users.insert(usr->nick(), usr);
}

void IrcChannel::removeUser(const QString& nick)
{
    Q_ASSERT(m_users.count(nick) == 1);
    delete m_users.take(nick);
}

bool IrcChannel::renameUser(QString oldNick, QString newNick)
{
    if(m_users.contains(oldNick))
    {
	m_users.insert(newNick, m_users.take(oldNick));
	m_users[newNick]->setNick(newNick);
	return true;
    }
    else
	return false;
}

bool IrcChannel::hasUser(QString nick)
{
    return m_users.contains(nick);
}

void IrcChannel::addText(QString theText)
{
    m_textCursor->movePosition(QTextCursor::End);
    m_textCursor->insertText(theText);
}

void IrcChannel::setTopic(QString theTopic)
{
    m_topic = theTopic;
}

void IrcChannel::setKey(QString& theKey)
{
    m_key = theKey;
    if(m_key.isEmpty())
	m_hasKey = false;
    else
	m_hasKey = true;
}

void IrcChannel::setUserLimit(int theLimit)
{
    if(m_userLimit > 0)
    {
	m_userLimit = theLimit;
	m_hasUserLimit = true;
    }
    else
    {
	m_userLimit = -1;
	m_hasUserLimit = false;
    }
}

void IrcChannel::setPrivate(bool theBool)
{
    m_isPrivate = theBool;
}

void IrcChannel::setSecret(bool theBool)
{
    m_isSecret = theBool;
}

void IrcChannel::setInviteOnly(bool theBool)
{
    m_isInviteOnly = theBool;
}

void IrcChannel::setExternal(bool theBool)
{
    m_isExternal = theBool;
}

void IrcChannel::setModerated(bool theBool)
{
    m_isModerated = theBool;
}

void IrcChannel::setDocumentFont(const QFont& font)
{
    m_document->setDefaultFont(font);
}

QString IrcChannel::name()
{
    return m_name;
}

QString IrcChannel::topic()
{
    return m_topic;
}

QString IrcChannel::key()
{
    return m_key;
}

QTextDocument* IrcChannel::document()
{
    return m_document;
}

IrcUser* IrcChannel::user(QString nick)
{
    if(m_users.contains(nick))
	return m_users[nick];
    else
	return NULL;
}

QStringList IrcChannel::channelNicks()
{
    QStringList ops, vos, reg;
    foreach(IrcUser *usr, m_users)
    {
	if(usr->isOperator())
	    ops.append("@"+usr->nick());
	else if(usr->isVoiced())
	    vos.append("+"+usr->nick());
	else
	    reg.append(usr->nick());
    }
    return ops+vos+reg;
}

int IrcChannel::userLimit()
{
    return m_userLimit;
}

bool IrcChannel::isPrivate()
{
    return m_isPrivate;
}

bool IrcChannel::isSecret()
{
    return m_isSecret;
}

bool IrcChannel::isInviteOnly()
{
    return m_isInviteOnly;
}

bool IrcChannel::isExternal()
{
    return m_isExternal;
}

bool IrcChannel::isModerated()
{
    return m_isModerated;
}

bool IrcChannel::isPrivateMessage()
{
    if(m_name.startsWith("#") ||
       m_name.startsWith("&"))
	return true;
    else
	return false;
}
