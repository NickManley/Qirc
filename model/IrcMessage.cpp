#include "IrcMessage.h"
#include <QDebug>

IrcMessage::IrcMessage(QObject *parent) :
    QObject(parent)
{
}

IrcMessage::IrcMessage(const QString& rhs, QObject *parent) :
    QObject(parent)
{
    this->parseServerMessage(rhs);
}

IrcMessage& IrcMessage::operator=(const IrcMessage &rhs)
{
    if(this == &rhs)
	return *this;
    this->m_prefix = rhs.m_prefix;
    this->m_command = rhs.m_command;
    this->m_arguments = rhs.m_arguments;
    return *this;
}

IrcMessage& IrcMessage::operator=(QString& rhs)
{
    this->parseServerMessage(rhs);
    return *this;
}

IrcMessage::~IrcMessage()
{
}

void IrcMessage::setMessage(const QString& theMessage)
{
    this->parseServerMessage(theMessage);
}

void IrcMessage::setPrefix(QString& thePrefix)
{
    m_prefix = thePrefix;
}

void IrcMessage::setCommand(const QString& cmd)
{
    m_command = cmd.toUpper();
}

void IrcMessage::appendArgument(QString arg)
{
    m_arguments.append(arg);
}

void IrcMessage::appendArguments(QStringList args)
{
    m_arguments.append(args);
}

bool IrcMessage::hasPrefix() const
{
    if(m_prefix == NULL)
	return false;
    if(!m_prefix.isEmpty())
	return true;
    else
	return false;
}

QString IrcMessage::command() const
{
    return m_command;
}

QStringList IrcMessage::arguments() const
{
    return m_arguments;
}

QString IrcMessage::argument(int index) const
{
    return m_arguments.at(index);
}

QString IrcMessage::prefix() const
{
    return m_prefix;
}

QString IrcMessage::suffix() const
{
    if(m_arguments.isEmpty() == false)
	return m_arguments.last();
    else
	return "";
}

/*QString IrcMessage::commandString() const
{
   if(m_command == RPL_NOTOPIC)
	return "331";
    else if(m_command == RPL_TOPIC)
	return "332";
    else if(m_command == RPL_NAMREPLY)
	return "353";
    else if(m_command == RPL_ENDOFNAMES)
	return "366";
    else if(m_command == RPL_MOTD)
	return "372";
    else if(m_command == RPL_MOTDSTART)
	return "375";
    else if(m_command == RPL_ENDOFMOTD)
	return "376";
    else if(m_command == ERR_NICKNAMEINUSE)
	return "433";
    else
	return "ERROR";
}*/

QString IrcMessage::toString() const
{
    QString str;

    // format prefix
    if(this->hasPrefix())
	str.append(":" + m_prefix + " ");

    // format command
    str.append(m_command);

    // format arguments
    if(this->numArgs() > 0)
    {
	for(int i=0; i<this->numArgs()-1; i++)
	    str.append(" " + m_arguments.at(i));

	// format suffix
	if(this->suffix().contains(" "))
	    str.append(" :" + m_arguments.last());
	else
	    str.append(" " + m_arguments.last());
    }
    return str;
}

int IrcMessage::numArgs() const
{
    return m_arguments.length();
}

void IrcMessage::clear()
{
    m_prefix.clear();
    m_command.clear();
    m_arguments.clear();
}

void IrcMessage::parseServerMessage(const QString& theMessage)
{
    //qDebug() << theMessage;

    int i=0, suffixPosition=0;
    QString command, prefix;

    // parse prefix
    if(theMessage.at(0) == ':')
    {
	for(i=1; i<theMessage.length() &&
	    !theMessage.at(i).isSpace(); i++)
	{
		prefix.append(theMessage.at(i));
	}
	this->setPrefix(prefix);
	i++;
    }

    // parse command
    for(; i<theMessage.length() &&
	    !theMessage.at(i).isSpace(); i++)
    {
        command.append(theMessage.at(i));
    }
    //i++;
    this->setCommand(command);

    // parse arguments
    suffixPosition = theMessage.indexOf(" :");
    this->appendArguments(theMessage.mid(i, suffixPosition-i)
			  .split(" ", QString::SkipEmptyParts));
    if(suffixPosition != -1)
	this->appendArgument(theMessage.mid(suffixPosition+2));

    m_arguments.last() = m_arguments.last().simplified();
}
