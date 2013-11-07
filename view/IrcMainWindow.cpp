#include "IrcMainWindow.h"

IrcMainWindow* IrcMainWindow::m_instance = NULL;
IrcMainWindow* ircMainWindow = NULL;

IrcMainWindow::IrcMainWindow(QWidget *parent) :
    QWidget(parent)
{
    ircSessionTreeView = new QTreeWidget(this);
    ircTextView = new QTextBrowser(this);
    ircUserListView = new QListWidget(this);
    ircTextInput = new QLineEdit(this);
    ircMenuBar = new QMenuBar(this);

    ircChatView = new QWidget(this);
    vboxChat = new QVBoxLayout(ircChatView);
    hbox = new QSplitter(this);
    layout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    ircTextInput->setFocus(Qt::ActiveWindowFocusReason);
    ircTextInput->setFocus(Qt::PopupFocusReason);
    ircSessionTreeView->setFont(QFont("", 10));
    ircSessionTreeView->setColumnCount(1);
    ircSessionTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ircSessionTreeView->setSelectionMode(QAbstractItemView::SingleSelection);
    ircSessionTreeView->setHeaderHidden(true);
    ircSessionTreeView->setIndentation(10);
    ircTextView->setReadOnly(true);
    ircUserListView->setSortingEnabled(false);

    QObject::connect(ircUserListView,
		     SIGNAL(itemDoubleClicked(QListWidgetItem*)),
		     this, SLOT(onUserDoubleClicked(QListWidgetItem*)));
    QObject::connect(ircTextInput, SIGNAL(returnPressed()),
		     this, SLOT(onInputTextEntered()));
    QObject::connect(ircSessionTreeView, SIGNAL(itemSelectionChanged()),
		     this, SLOT(onSelectionChanged()));
    QObject::connect(ircTextView, SIGNAL(textChanged()),
		     this, SLOT(onDocumentUpdated()));

    // Setup actions
    actCloseChannel = new QAction(QIcon("closeicon.png"),"Close",this);
	ircSessionTreeView->setContextMenuPolicy(Qt::ActionsContextMenu);
	ircSessionTreeView->addAction(actCloseChannel);
	QObject::connect(actCloseChannel, SIGNAL(triggered()),
		    this, SLOT(onActionCloseChannel()));
    actViewHelp = new QAction("Documentation", this);
	QObject::connect(actViewHelp, SIGNAL(triggered()),
		    this, SLOT(onActionViewHelp()));
    actViewLicense = new QAction("License", this);
	QObject::connect(actViewLicense, SIGNAL(triggered()),
		    this, SLOT(onActionViewLicense()));
    actViewAbout = new QAction("About", this);
	QObject::connect(actViewAbout, SIGNAL(triggered()),
		    this, SLOT(onActionViewAbout()));
    actConnect = new QAction("Connect", this);
	QObject::connect(actConnect, SIGNAL(triggered()),
		     this, SLOT(onActionConnect()));
    actDisconnect = new QAction("Disconnect", this);
	QObject::connect(actDisconnect, SIGNAL(triggered()),
		     this, SLOT(onActionDisconnect()));
    actExit = new QAction("Exit", this);
	QObject::connect(actExit, SIGNAL(triggered()),
		     this, SLOT(onActionExit()));
    actJoinChannel = new QAction("Join Channel", this);
	QObject::connect(actJoinChannel, SIGNAL(triggered()),
		     this, SLOT(onActionJoinChannel()));
    actChangeNick = new QAction("Change Nick", this);
	QObject::connect(actChangeNick, SIGNAL(triggered()),
		     this, SLOT(onActionChangeNick()));
    // Setup menus
    menuServer = new QMenu("Server", this);
	menuServer->addAction(actConnect);
	menuServer->addAction(actJoinChannel);
	menuServer->addAction(actChangeNick);
	menuServer->addAction(actDisconnect);
	menuServer->addAction(actExit);
    menuChannel = new QMenu("Channel", this);
	menuChannel->setEnabled(false);
    menuUser = new QMenu("User", this);
	menuUser->setEnabled(false);
    menuBookmarks = new QMenu("Bookmarks", this);
	menuBookmarks->setEnabled(false);
    menuHelp = new QMenu("Help", this);
	menuHelp->addAction(actViewHelp);
	menuHelp->addAction(actViewLicense);
	menuHelp->addAction(actViewAbout);
    // Setup menu bar
    ircMenuBar->addMenu(menuServer);
    //ircMenuBar->addMenu(menuChannel);
    //ircMenuBar->addMenu(menuUser);
    //ircMenuBar->addMenu(menuBookmarks);
    ircMenuBar->addMenu(menuHelp);

    // Setup ircChatArea
    vboxChat->addWidget(ircTextView);
    vboxChat->addWidget(ircTextInput);
    vboxChat->setMargin(0);
    ircChatView->setLayout(vboxChat);

    // Setup horizontal display for the main window
    hbox->addWidget(ircSessionTreeView);
    hbox->addWidget(ircChatView);
    hbox->addWidget(ircUserListView);
    hbox->setStretchFactor(0, 2);
    hbox->setStretchFactor(1, 10);
    hbox->setStretchFactor(2, 2);

    layout->setDirection(QBoxLayout::TopToBottom);
    layout->setMenuBar(ircMenuBar);
    layout->addWidget(hbox);
    //setupStatusBar();

    layout->setMargin(8);
    this->setMinimumWidth(300);
    this->setLayout(layout);
    this->resize(800, 500);
    this->setWindowTitle("Qirc IRC Client");
    this->setWindowIcon(QIcon("Qirc32.ico"));
}

IrcMainWindow* IrcMainWindow::instance() {
    if(m_instance == NULL)
	m_instance = new IrcMainWindow();
    return m_instance;
}

void IrcMainWindow::setupStatusBar()
{
    ircStatusBar = new QStatusBar(this);
    ircStatusBar->setFixedHeight(12);
    ircStatusBar->showMessage("Connected as: Qirc");
    layout->addWidget(ircStatusBar);
}

void IrcMainWindow::addNetwork(const QString& name)
{
    ircSessionTreeView->addTopLevelItem(
	    new QTreeWidgetItem(ircSessionTreeView, QStringList(name)));
    this->selectNetwork(name);
}

/*
When this method is called, the socket has already
removed the network from its tree.  Trying to access
it will result in a segmentation fault.  This is why
we have to set the current item to NULL to prevent
the session tree from autoselecting the next item
which doesn't exist.
*/
void IrcMainWindow::removeNetwork(const QString& network)
{
    ircSessionTreeView->setCurrentItem(NULL);
    ircTextView->setDocument(NULL);
    ircTextView->clear();
    ircUserListView->clear();

    QTreeWidgetItem *item = ircSessionTreeView->findItems(network,
				Qt::MatchExactly).at(0);
    item->takeChildren();
    for(int i=0; i<ircSessionTreeView->topLevelItemCount(); i++)
    {
	if(ircSessionTreeView->topLevelItem(i)->text(0) == network)
	    ircSessionTreeView->takeTopLevelItem(i);
    }
}

void IrcMainWindow::addChannel(const QString& network, const QString& name)
{
    QTreeWidgetItem *item = ircSessionTreeView->findItems(network,
			    Qt::MatchExactly).at(0);
    item->addChild(new QTreeWidgetItem(item, QStringList(name)));
}

void IrcMainWindow::removeChannel(const QString& network,
				  const QString& channel)
{
    QList<QTreeWidgetItem*> items = ircSessionTreeView->findItems(channel,
			Qt::MatchExactly | Qt::MatchRecursive, 0);
    foreach(QTreeWidgetItem *item, items)
    {
	if(item->parent() != NULL &&
	   item->parent()->text(0) == network)
	{
	    item->parent()->removeChild(item);
	    break;
	}
    }
}

void IrcMainWindow::selectNetwork(const QString& name)
{
    QTreeWidgetItem *item = ircSessionTreeView->findItems(name,
			    Qt::MatchExactly).at(0);
    item->setSelected(false);
    ircSessionTreeView->setCurrentItem(item);
    ircTextInput->setFocus(Qt::OtherFocusReason);
}

void IrcMainWindow::selectChannel(const QString& network,
				  const QString& channel)
{
    QList<QTreeWidgetItem*> items = ircSessionTreeView->findItems(channel,
			Qt::MatchExactly | Qt::MatchRecursive, 0);
    foreach(QTreeWidgetItem *item, items)
    {
	if(item->parent() != NULL &&
	   item->parent()->text(0) == network)
	{
	    item->parent()->setExpanded(true);
	    ircSessionTreeView->setCurrentItem(item);
	    break;
	}
    }
    ircTextInput->setFocus(Qt::OtherFocusReason);
}

void IrcMainWindow::setDocument(QTextDocument* document)
{
    if(document == NULL)
	ircTextView->clear();
    ircTextView->setDocument(document);
}

void IrcMainWindow::setUserList(QStringList users)
{
    ircUserListView->clear();
    ircUserListView->addItems(users);
}

void IrcMainWindow::clearUserList()
{
    ircUserListView->clear();
}

void IrcMainWindow::clearInputText()
{
    ircTextInput->clear();
}

void IrcMainWindow::highlightNetwork(QString network)
{
    QTreeWidgetItem *item = ircSessionTreeView->findItems(network,
			    Qt::MatchExactly).at(0);
    if(item != ircSessionTreeView->currentItem())
	item->setTextColor(0, QColor(255,0,0));
}

void IrcMainWindow::highlightChannel(QString network, QString channel)
{
    QTreeWidgetItem *item = ircSessionTreeView->findItems(network,
			    Qt::MatchExactly).at(0);
    QTreeWidgetItemIterator it(item);
    while(*it)
    {
	if ((*it)->text(0) == channel &&
	    (*it) != ircSessionTreeView->currentItem())
	{
	    (*it)->setTextColor(0, QColor(255,0,0));
	    break;
	}
	++it;
    }
}

void IrcMainWindow::onSelectionChanged()
{
    QTreeWidgetItem *item = ircSessionTreeView->currentItem();
    if(item == NULL) return;
    item->setTextColor(0, QColor(0,0,0));
    if(item->parent() == NULL)
	emit this->selectionChanged(item->text(0));
    else
	emit this->selectionChanged(item->parent()->text(0),
				    item->text(0));
    ircTextInput->setFocus(Qt::OtherFocusReason);
    ircTextView->verticalScrollBar()->setValue(
	    ircTextView->verticalScrollBar()->maximum());
}

void IrcMainWindow::onInputTextEntered()
{
    emit this->inputTextEntered(ircTextInput->text());
    ircTextInput->clear();
}

void IrcMainWindow::onDocumentUpdated()
{
    static QScrollBar *scrollbar = ircTextView->verticalScrollBar();
    if(scrollbar->isSliderDown() == false)
	scrollbar->setValue(scrollbar->maximum());
}

void IrcMainWindow::onUserDoubleClicked(QListWidgetItem *item)
{
    QString userNick = item->text();
    if(userNick.startsWith("@") || userNick.startsWith("+"))
	userNick = userNick.right(userNick.size()-1);
    QString network = ircSessionTreeView->currentItem()->parent()->text(0);
    emit this->userDoubleClicked(network, userNick);
}

void IrcMainWindow::onActionCloseChannel()
{
    if(ircSessionTreeView->currentItem() == NULL)
	return;
    else if(ircSessionTreeView->currentItem()->parent())
	emit this->inputTextEntered("/part");
    else
	emit this->inputTextEntered("/quit");
}

void IrcMainWindow::onActionViewHelp()
{
    emit this->inputTextEntered("/help");
}

void IrcMainWindow::onActionViewLicense()
{
    emit this->inputTextEntered("/license");
}

void IrcMainWindow::onActionViewAbout()
{
    emit this->inputTextEntered("/about");
}

void IrcMainWindow::onActionConnect()
{
    emit this->inputTextEntered("/connect");
}

void IrcMainWindow::onActionDisconnect()
{
    emit this->inputTextEntered("/quit");
}

void IrcMainWindow::onActionExit()
{
    this->close();
}

void IrcMainWindow::onActionJoinChannel()
{
    emit this->inputTextEntered("/join");
}

void IrcMainWindow::onActionChangeNick()
{
    emit this->inputTextEntered("/nick");
}

void IrcMainWindow::closeEvent(QCloseEvent* ce)
{
    switch( QMessageBox::question( this, "Exit Qirc",
	"Are you sure you wish to exit Qirc?", "Exit", "Cancel", 0, 1 ) )
    {
	case 0:
	    ce->accept(); break;
	case 1:
	default: // just for sanity
	    ce->ignore(); break;
    }
}

IrcMainWindow::~IrcMainWindow()
{

}
