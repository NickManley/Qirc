#include "IrcController.h"
#include <QtWidgets/QApplication>

IrcController* IrcController::m_instance = NULL;
IrcController* ircController = NULL;

IrcController::IrcController(QObject *parent) :
    QObject(parent)
{
    ircMainWindow = IrcMainWindow::instance();
    dw = new QircDialogWindow();
    ircConnectionWindow = IrcConnectionWindow::instance();
    ircConnectionWindow->show();

    // Connection Window
    QObject::connect(ircConnectionWindow, SIGNAL(cancelButtonPressed()),
	this, SLOT(onCancelButtonPressed()));
    QObject::connect(ircConnectionWindow,
	SIGNAL(connectButtonPressed(QString,QString,QString)), this,
	SLOT(onConnectButtonPressed(QString,QString,QString)));
    // Main Window
    QObject::connect(ircMainWindow, SIGNAL(selectionChanged(QString)),
		     this, SLOT(onSelectionChanged(QString)));
    QObject::connect(ircMainWindow, SIGNAL(selectionChanged(QString,QString)),
		     this, SLOT(onSelectionChanged(QString,QString)));
    QObject::connect(ircMainWindow, SIGNAL(inputTextEntered(QString)),
		     this, SLOT(onInputTextEntered(QString)));
    QObject::connect(ircMainWindow, SIGNAL(userDoubleClicked(QString,QString)),
		     this, SLOT(onUserDoubleClicked(QString,QString)));
    // Dialog Window
    QObject::connect(dw, SIGNAL(inputTextEntered(QString)),
		     this, SLOT(onInputTextEntered(QString)));
}

IrcController::~IrcController()
{
    delete dw;
}

IrcController* IrcController::instance() {
    if(m_instance == NULL)
	m_instance = new IrcController();
    return m_instance;
}

// Connection Window slots

 void IrcController::onCancelButtonPressed()
 {
     if(ircMainWindow->isHidden())
	 qApp->quit();
     else
	 ircConnectionWindow->hide();
 }

void IrcController::onConnectButtonPressed(QString serverName,
	QString serverAddress, QString nick)
{
    ircConnectionWindow->hide();
    IrcSocket *socket = new IrcSocket(this);
    serverName.remove(QChar('!'), Qt::CaseInsensitive);
    serverName.remove(QChar('@'), Qt::CaseInsensitive);
    serverName.remove(QChar('#'), Qt::CaseInsensitive);
    serverName.remove(QChar('$'), Qt::CaseInsensitive);
    serverName.remove(QChar('%'), Qt::CaseInsensitive);
    serverName.remove(QChar('^'), Qt::CaseInsensitive);
    serverName.remove(QChar('&'), Qt::CaseInsensitive);
    serverName.remove(QChar('*'), Qt::CaseInsensitive);
    serverName.remove(QChar('+'), Qt::CaseInsensitive);
    if(m_sockets.contains(serverName))
    {
	int i = 1;
	while(m_sockets.contains(serverName + "-" + QString::number(++i)));
	serverName.append("-" + QString::number(i));
    }

    QObject::connect(socket, SIGNAL(channelCreated(QString,QString)),
		     this, SLOT(onChannelCreated(QString,QString)));
    QObject::connect(socket, SIGNAL(channelDestroyed(QString,QString)),
		     this, SLOT(onChannelDestroyed(QString,QString)));
    QObject::connect(socket, SIGNAL(userListChanged(QString,QString)),
		     this, SLOT(onUserListChanged(QString,QString)));
    QObject::connect(socket, SIGNAL(documentUpdated(QString)),
		     this, SLOT(onDocumentUpdated(QString)));
    QObject::connect(socket, SIGNAL(documentUpdated(QString,QString)),
		     this, SLOT(onDocumentUpdated(QString,QString)));
    QObject::connect(socket, SIGNAL(disconnected(QString)),
		     this, SLOT(onDisconnected(QString)));

    socket->connect(serverName, serverAddress, nick);
    m_sockets.insert(serverName, socket);
    ircMainWindow->addNetwork(serverName);
    ircMainWindow->show();
}

// Main Window slots

void IrcController::onSelectionChanged(const QString& network,
			 const QString& name)
{
    m_network = network;
    m_channel = name;

    if(name.isEmpty())
    {
	ircMainWindow->setDocument(this->getDocument(network));
	ircMainWindow->clearUserList();
    }
    else
    {
	ircMainWindow->setDocument(this->getDocument(network, name));
	ircMainWindow->setUserList(m_sockets[network]->channelNicks(name));
    }
    ircMainWindow->clearInputText();
}

void IrcController::onInputTextEntered(const QString& text)
{
    IrcMessage *message = new IrcMessage(this);
    if(text.at(0) != '/' && m_channel.isEmpty() == false)
    {
	message->setCommand("PRIVMSG");
	message->appendArgument(m_channel);
	message->appendArgument(text);
	m_sockets[m_network]->write(message);
    }
    else if(text.toLower().startsWith("/join ") ||
	    text.toLower().startsWith("/j "))
    {
	message->setCommand("JOIN");
	message->appendArguments(text.section(" ", 1).split(",",
				    QString::SkipEmptyParts));
	m_sockets[m_network]->write(message);
    }
    else if(text.toLower() == "/join" || text.toLower() == "/j")
    {
	dw->showJoinChannel();
    }
    else if(text.toLower() == "/part" ||
	    text.toLower() == "/p")
    {
	message->setCommand("PART");
	message->appendArgument(m_channel);
	m_sockets[m_network]->write(message);
    }
    else if(text.toLower().startsWith("/message ") ||
	    text.toLower().startsWith("/msg ") ||
	    text.toLower().startsWith("/m "))
    {
	message->setCommand("PRIVMSG");
	message->appendArgument(text.section(" ", 1, 1));
	message->appendArgument(text.section(" ", 2));
	m_sockets[m_network]->write(message);
    }
    else if(text.toLower().startsWith("/nick "))
    {
	message->setCommand("NICK");
	message->appendArguments(text.section(" ", 1).split(",",
				    QString::SkipEmptyParts));
	m_sockets[m_network]->write(message);
    }
    else if(text.toLower() == "/nick")
    {
	dw->showChangeNick();
    }
    else if(text.toLower().startsWith("/quit "))
    {
	m_sockets[m_network]->disconnect(text.section(" ", 1,1));
    }
    else if(text.toLower() == "/quit")
    {
	if(m_network.isEmpty() == false)
	    m_sockets[m_network]->disconnect();
    }
    else if(text.toLower().startsWith("/font "))
    {
	QString fontName = text.toLower().section(" ", 1,1);
	int fontSize = text.toLower().section(" ", 2, 2).toInt();
	this->setDocumentFont(QFont(fontName, fontSize));
    }
    else if(text.toLower().startsWith("/kick "))
    {
	qDebug() << "Kick";
	QString nick = text.section(" ", 1, 1);
	QString reason = text.section(" ", 2);
	message->setCommand("KICK");
	message->appendArgument(m_channel);
	message->appendArgument(nick);
	if(reason.isEmpty() == false)
	    message->appendArgument(reason);
	qDebug() << message->toString();
	m_sockets[m_network]->write(message);
    }
    else if(text.toLower() == "/time")
    {
	message->setCommand("TIME");
	m_sockets[m_network]->write(message);
    }
    else if(text.toLower() == "/version")
    {
	message->setCommand("VERSION");
	m_sockets[m_network]->write(message);
    }
    else if(text.toLower() == "/connect" ||
	    text.toLower() == "/c")
    {
	ircConnectionWindow->show();
    }
    else if(text.toLower() == "/about")
    {
	dw->showAbout();
    }
    else if(text.toLower() == "/license")
    {
	dw->showLicense();
    }
    else if(text.toLower() == "/help")
    {
	dw->showHelp();
    }
    delete message;
}

void IrcController::onUserDoubleClicked(const QString& network,
					const QString& nick)
{
    this->createChannel(network, nick);
    ircMainWindow->selectChannel(network, nick);
}

// Socket slots

void IrcController::onChannelCreated(QString network, QString channel)
{
    ircMainWindow->addChannel(network, channel);
    if(channel.startsWith("#") || channel.startsWith("&"))
	ircMainWindow->selectChannel(network, channel);
}

void IrcController::onChannelDestroyed(QString network, QString channel)
{
    ircMainWindow->removeChannel(network, channel);
}

void IrcController::onUserListChanged(QString network, QString channel)
{
    if(m_network == network && m_channel == channel)
	ircMainWindow->setUserList(m_sockets[network]->channelNicks(channel));
}

void IrcController::onDocumentUpdated(QString network, QString channel)
{
    ircMainWindow->highlightChannel(network, channel);
}

void IrcController::onDocumentUpdated(QString network)
{
    ircMainWindow->highlightNetwork(network);
}

QTextDocument* IrcController::getDocument(QString networkName)
{
    return m_sockets[networkName]->networkText();
}

QTextDocument* IrcController::getDocument(QString networkName, QString name)
{
    return m_sockets[networkName]->channelDocument(name);
}

void IrcController::createChannel(QString network, QString nick)
{
    m_sockets[network]->createChannel(nick);
}

void IrcController::setDocumentFont(const QFont &font)
{
    foreach(IrcSocket *socket, m_sockets)
    {
	socket->setDocumentFont(font);
    }
}

void IrcController::onDisconnected(QString network)
{
    if(m_network == network)
    {
	m_network.clear();
	m_channel.clear();
    }
    ircMainWindow->removeNetwork(network);
    delete m_sockets.take(network);
}

