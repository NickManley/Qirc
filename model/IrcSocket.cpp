#include "IrcSocket.h"

IrcSocket::IrcSocket(QObject *parent) :
    QObject(parent)
{
    m_socket = new QTcpSocket(this);
    m_identity = new IrcUser(this);
    m_port = 6667;
    m_document = new QTextDocument(this);
    m_document->setDefaultFont(QFont("Consolas", 11));
    m_document->setMaximumBlockCount(1024);

    QObject::connect(m_socket, SIGNAL(connected()),
		     this, SLOT(onConnect()));
    QObject::connect(m_socket, SIGNAL(readyRead()),
		     this, SLOT(onReadyRead()));
}

IrcSocket::~IrcSocket()
{
    delete m_socket;
    delete m_identity;
    delete m_document;
    foreach(IrcChannel *chan, m_channels)
	delete chan;
}

void IrcSocket::connect(const QString& name,
			const QString& host,
			const QString& nick)
{
    m_serverName = name;

    m_identity->setNick(nick);
    m_identity->setUser("Qirc");
    m_identity->setHost( QHostInfo::localHostName() );

    m_socket->connectToHost(host, m_port);
}

void IrcSocket::disconnect(QString message)
{
    if(message.isEmpty())
	    m_socket->write("QUIT\r\n");
    else
        m_socket->write("QUIT :" + message.toUtf8() + "\r\n");
    m_socket->disconnect();
    emit disconnected(m_serverName);
}

void IrcSocket::write(IrcMessage *message)
{
    if(message->command() == "PRIVMSG")
    {
	m_channels[message->argument(0)]->addText(
	    "\r\n<" + m_identity->nick() + "> " + message->suffix());
    }
    else if(message->command() == "PART" &&
	    (message->argument(0).startsWith("#") ||
	     message->argument(0).startsWith("&") == false))
    {
	emit channelDestroyed(m_serverName, message->argument(0));
	delete m_channels.take(message->argument(0));
    }
    else if(message->command() == "QUIT")
    {
	if(message->numArgs() > 0)
	    this->disconnect(message->argument(0));
	else
	    this->disconnect();
    }
    m_socket->write(message->toString().toUtf8() + "\r\n");
}

QTextDocument* IrcSocket::networkText()
{
    return m_document;
}

QTextDocument* IrcSocket::channelDocument(const QString& channel)
{
    return m_channels[channel]->document();
}

QStringList IrcSocket::channelNicks(const QString& channel)
{
    return m_channels[channel]->channelNicks();
}

void IrcSocket::createChannel(const QString& channel)
{
    m_channels.insert(channel, new IrcChannel(channel, this));
    emit this->channelCreated(m_serverName, channel);
}

void IrcSocket::destroyChannel(const QString& channel)
{
    emit this->channelDestroyed(m_serverName, channel);
    delete m_channels.take(channel);
}

void IrcSocket::setDocumentFont(const QFont& font)
{
    m_document->setDefaultFont(font);
    foreach(IrcChannel *chan, m_channels)
    {
	chan->setDocumentFont(font);
    }
}

IrcUser* IrcSocket::identity()
{
    return m_identity;
}

void IrcSocket::onConnect()
{
    m_socket->write("PASS QIRC\r\n");
    m_socket->write("NICK " + m_identity->nick().toUtf8() + "\r\n");
    m_socket->write("USER " + m_identity->nick().toUtf8() + " " +
            QHostInfo::localHostName().toUtf8() + " " +
		    " * " + ":Qirc User\r\n");
    m_socket->flush();
    emit connected();
}

void IrcSocket::onReadyRead()
{
    while(m_socket->canReadLine())
    {
	IrcMessage *message = new IrcMessage(m_socket->readLine(1024));
	// Who the message is from
	QString userNick = message->prefix().mid(0,
	    message->prefix().indexOf("!"));

	//qDebug() << message->toString();

	if(message->command() == "PRIVMSG")
	{
	    QString receiver = message->argument(0);
	    QTextCursor *textCursor = NULL;
	    QString c; // @+ character for displaying channel nick

	    // If the receiver of the message is ourself
	    // then it's a private message to us.
	    if(receiver == m_identity->nick())
	    {
		// Has the message been mapped yet?
		if(m_channels.contains(userNick) == false)
		    this->createChannel(userNick);
		textCursor = new QTextCursor(m_channels[userNick]->document());
		emit this->documentUpdated(m_serverName, userNick);
	    }
	    else // It's a message to a channel
	    {
		// Has the message been mapped yet?
		if(m_channels.contains(receiver) == false)
		    this->createChannel(receiver);
		textCursor = new QTextCursor(m_channels[receiver]->document());
		emit this->documentUpdated(m_serverName, receiver);

		if(m_channels[receiver]->user(userNick)->isOperator())
		    c = "@";
		else if(m_channels[receiver]->user(userNick)->isVoiced())
		    c = "+";
		else
		    c.clear();
	    }
	    textCursor->movePosition(QTextCursor::End);

	    // Handle message
	    if(message->argument(1).startsWith("\01") == false)
	    {
		textCursor->insertText("\r\n<" + c + userNick +
		    "> " + message->suffix());
	    }
	    if(message->argument(1).startsWith("\01ACTION ") &&
		    message->argument(1).endsWith("\01"))
	    {
		QString action = message->argument(1).mid(8);
		action.chop(1); // remove trailing '\01' char
		textCursor->insertText("\r\n* "+c+ userNick + " " + action);
	    }
	    else if(message->argument(1) == "\01VERSION\01")
	    {
        m_socket->write("NOTICE " + userNick.toUtf8() +
                ":\01VERSION Qirc:0.3.0:"
                + this->operatingSystem().toUtf8() +
				"\01\r\n");
		m_socket->flush();
	    }
	    else if(message->argument(1) == "\01FINGER\01")
	    {
        m_socket->write("NOTICE " + userNick.toUtf8() +
				":\01FINGER :Qirc User\01\r\n");
		m_socket->flush();
	    }
	    delete textCursor;
	}
	else if(message->command() == "NICK")
	{
	    foreach(IrcChannel *chan, m_channels)
	    {
		if(chan->renameUser(userNick, message->argument(0)))
		{
		    chan->addText("\r\n** " + userNick +
		       " is now known as " + message->argument(0));
		    emit this->userListChanged(m_serverName, chan->name());
		}
	    }
	}
	else if(message->command() == "PING")
	{
	    if(message->arguments().size() == 1)
	    {
		m_socket->write("PONG :" +
            message->suffix().toUtf8() + "\r\n");
	    }
	    else
		m_socket->write("PONG\r\n");
	}
	else if(message->command() == "QUIT")
	{
	    foreach(IrcChannel *chan, m_channels)
	    {
		if(chan->hasUser(userNick))
		    chan->addText("\r\n** " + userNick +
		       " has quit");
		if(message->arguments().size() == 1)
		    chan->addText(": " + message->suffix());
		emit this->userListChanged(m_serverName, chan->name());
	    }
	}
	else if(message->command() == "JOIN")
	{
	    IrcChannel *chan = NULL;
	    if(m_channels.contains(message->argument(0)) == false)
	    {
		m_channels.insert(message->argument(0),
		    new IrcChannel(message->argument(0), this));
		emit channelCreated(m_serverName, message->argument(0));
	    }
	    chan = m_channels[message->argument(0)];
	    chan->addUser(userNick);
	    chan->addText("\r\n** " + userNick +
		" has joined " + chan->name());
	    emit this->userListChanged(m_serverName, chan->name());
	}
	else if(message->command() == "PART")
	{
	    IrcChannel *chan = NULL;
	    if(m_identity->nick() == userNick) // remove self
		this->destroyChannel(message->argument(0));
	    else // remove another user
	    {
		chan = m_channels[message->argument(0)];
		chan->removeUser(userNick);
		chan->addText("\r\n** " + userNick +
		    " has left " + chan->name());
		emit userListChanged(m_serverName, chan->name());
	    }
	}
	else if(message->command() == "MODE")
	{
	    QString modeString = message->argument(1);

	    bool modeBool = false;
	    if(modeString.at(0) == '+')
		modeBool = true;

	    if(message->arguments().size() == 3) // channel-nick mode
	    {
		IrcChannel *chan = m_channels[message->argument(0)];
		IrcUser *user = chan->user(message->argument(2));
		for(int i=1; i<modeString.size(); i++)
		{
		    if(modeString.at(i) == 'v')
			user->setVoiced(modeBool);
		    else if(modeString.at(i) == 'o')
			user->setOperator(modeBool);
		    else if(modeString.at(i) == 'i')
			user->setVisibile(modeBool);
		}
		emit userListChanged(m_serverName, chan->name());
	    }
	    else if(message->argument(0).startsWith("#") ||
		    message->argument(0).startsWith("&")) // channel mode
	    {
		IrcChannel *chan = m_channels[message->argument(0)];
		for(int i=1; i<modeString.size(); i++)
		{
		    if(modeString.at(i) == 'p')
			chan->setPrivate(modeBool);
		    else if(modeString.at(i) == 's')
			chan->setSecret(modeBool);
		    else if(modeString.at(i) == 'i')
			chan->setInviteOnly(modeBool);
		}
	    }
	    else
	    {
		// set user mode
	    }
	}
	else if(message->command() == "TOPIC")
	{
	    IrcChannel *chan = m_channels[message->argument(0)];
	    chan->addText("\r\n** " + userNick +
		" has changed the topic to: " + message->suffix());
	}
	else if(message->command() == "KICK")
	{
	    if(message->argument(1) == m_identity->nick())
	    {
		QTextCursor *textCursor = new QTextCursor(m_document);
		textCursor->movePosition(QTextCursor::End);
		textCursor->insertText("\r\n");
		textCursor->insertHtml(
		    "<span style='color:#FF0000'>** " + userNick +
		    " has kicked " + message->argument(1) +
		    " from " + message->argument(0) + "</span>");
		if(message->numArgs() == 3)
		    textCursor->insertHtml("<span style='color:#FF0000'>"
			"&nbsp;| Reason: " + message->suffix() + "</span>");
		delete textCursor;
		this->destroyChannel(message->argument(0));
		emit this->documentUpdated(m_serverName);
	    }
	    else
	    {
		IrcChannel *chan = m_channels[message->argument(0)];
		chan->removeUser(message->argument(1));
		chan->addText("\r\n** " + userNick +
		    " has kicked " + message->argument(1));
		if(message->arguments().size() == 3)
		    chan->addText(" | Reason: " + message->suffix());
		emit this->userListChanged(m_serverName, chan->name());
	    }
	}
	else if(message->command() == "331") // RPL_NOTOPIC
	{
	    IrcChannel *chan = m_channels[message->argument(1)];
	    chan->addText("\r\n** No Topic Set");
	}
	else if(message->command() == "332") // RPL_TOPIC
	{
	    IrcChannel *chan = m_channels[message->argument(1)];
	    chan->setTopic(message->suffix());
	    chan->addText("\r\n** Topic: " + message->suffix());
	}
	else if(message->command() == "353") // RPL_NAMREPLY
	{
	    IrcChannel *chan = m_channels[message->argument(2)];
	    foreach(QString usr, message->suffix().split(" ",
		    QString::SkipEmptyParts))
	    {
		chan->addUser(usr);
	    }
	}
	else if(message->command() == "366") // RPL_ENDOFNAMES
	{
	    emit this->userListChanged(m_serverName, message->argument(1));
	}
	else if(message->command() == "376") // RPL_ENDOFMOTD
	{
	    // currently, do nothing
	}
	else if(message->command() == "004" || message->command() == "005")
	{
	    // currently, do nothing
	}
	else
	{
	    QTextCursor *textCursor = new QTextCursor(m_document);
	    textCursor->movePosition(QTextCursor::End);
	    textCursor->insertText("\r\n" + message->suffix());
	    delete textCursor;
	}
	delete message;
    }
}

QString IrcSocket::operatingSystem()
{
    #if defined Q_OS_WIN32
	return "Windows";
    #elif defined Q_OS_MAC
	return "Macintosh";
    #elif defined Q_OS_LINUX
	return "Linux";
    #else
	return "Unknown";
    #endif
}
