#include "IrcConnectionWindow.h"
#include <QtWidgets/QApplication>

IrcConnectionWindow* IrcConnectionWindow::m_instance = NULL;
IrcConnectionWindow* ircConnectionWindow = NULL;

IrcConnectionWindow::IrcConnectionWindow(QWidget *parent) :
    QWidget(parent)
{
    layout = new QBoxLayout(QBoxLayout::LeftToRight, this);

    hbox = new QSplitter(this);

    ircNetworkTreeView = new QTreeWidget(this);
    ircConnectionForm = new QWidget(this);
    ircConnectionFormLayout = new QGridLayout(ircConnectionForm);

    ircServerNameLabel = new QLabel(ircConnectionForm);
    ircServerNameInput = new QLineEdit(ircConnectionForm);
    ircServerAddressLabel = new QLabel(ircConnectionForm);
    ircServerAddressInput = new QLineEdit(ircConnectionForm);
    ircNicknameLabel = new QLabel(ircConnectionForm);
    ircNicknameInput = new QLineEdit(ircConnectionForm);
    ircConnectButton = new QPushButton(ircConnectionForm);
    ircCancelButton = new QPushButton(ircConnectionForm);

    QObject::connect(ircConnectButton, SIGNAL(clicked()),
                     this, SLOT(onConnectButtonPressed()));
    QObject::connect(ircCancelButton, SIGNAL(clicked()),
                     this, SLOT(onCancelButtonPressed()));
    QObject::connect(ircServerNameInput, SIGNAL(textChanged(QString)),
                     this, SLOT(onInformationChanged()));
    QObject::connect(ircServerAddressInput, SIGNAL(textChanged(QString)),
                     this, SLOT(onInformationChanged()));
    QObject::connect(ircNicknameInput, SIGNAL(textChanged(QString)),
                     this, SLOT(onInformationChanged()));

    ircServerNameLabel->setText("Server Name: ");
    ircServerAddressLabel->setText("Server Address: ");
    ircNicknameLabel->setText("Nickname: ");
    ircConnectButton->setText("Connect");
    ircConnectButton->setFixedWidth(80);
    ircConnectButton->setEnabled(false);
    ircCancelButton->setText("Cancel");
    ircCancelButton->setFixedWidth(80);

    ircConnectionFormLayout->addWidget(ircServerNameLabel, 0, 0, 1, 4);
    ircConnectionFormLayout->addWidget(ircServerNameInput, 1, 0, 1, 4);
    ircConnectionFormLayout->addWidget(ircServerAddressLabel, 2, 0, 1, 4);
    ircConnectionFormLayout->addWidget(ircServerAddressInput, 3, 0, 1, 4);
    ircConnectionFormLayout->addWidget(ircNicknameLabel, 4, 0, 1, 4);
    ircConnectionFormLayout->addWidget(ircNicknameInput, 5, 0, 1, 4);
    ircConnectionFormLayout->addWidget(ircConnectButton, 7, 1);
    ircConnectionFormLayout->addWidget(ircCancelButton, 7, 2);

    ircConnectionFormLayout->setHorizontalSpacing(80);
    ircConnectionFormLayout->setAlignment(Qt::AlignTop);
    ircConnectionForm->setLayout(ircConnectionFormLayout);

    ircNetworkTreeView->setColumnCount(1);
    ircNetworkTreeView->setHeaderLabel("IRC Servers");

    this->generateServerList();

    QObject::connect(ircNetworkTreeView,
                     SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                     this, SLOT(onServerSelected(QTreeWidgetItem*)));

    ircNicknameInput->setText("Qirc");

    hbox->addWidget(ircNetworkTreeView);
    hbox->addWidget(ircConnectionForm);
    hbox->setStretchFactor(0, 3);
    hbox->setStretchFactor(1, 2);

    layout->addWidget(hbox);
    layout->setMargin(8);
    this->setLayout(layout);
    this->resize(800, 500);
    this->setWindowTitle("New Connection");
    this->setWindowIcon(QIcon("Qirc32.ico"));
}

IrcConnectionWindow* IrcConnectionWindow::instance()
{
    if(m_instance == NULL)
        m_instance = new IrcConnectionWindow();
    return m_instance;
}

void IrcConnectionWindow::onConnectButtonPressed()
{
    emit this->connectButtonPressed(ircServerNameInput->text(),
                                    ircServerAddressInput->text(),
                                    ircNicknameInput->text());
    ircServerNameInput->clear();
    ircServerAddressInput->clear();
}

void IrcConnectionWindow::onCancelButtonPressed()
{
    emit this->cancelButtonPressed();
    ircServerNameInput->clear();
    ircServerAddressInput->clear();
}

void IrcConnectionWindow::onServerSelected(QTreeWidgetItem* item)
{
    if(item->parent() == NULL)
    {
        ircServerNameInput->setText(item->text(0));
        ircServerAddressInput->setText(item->text(1));
    }
    else
    {
        ircServerNameInput->setText(item->parent()->text(0));
        ircServerAddressInput->setText(item->text(1));
    }
}

void IrcConnectionWindow::onInformationChanged()
{
    if(ircNicknameInput->text().isEmpty() ||
            ircServerNameInput->text().isEmpty() ||
            ircServerAddressInput->text().isEmpty())
    {
        ircConnectButton->setEnabled(false);
    }
    else
        ircConnectButton->setEnabled(true);
}

void IrcConnectionWindow::generateServerList()
{
    ircNetworkTreeView->addTopLevelItem(
                new QTreeWidgetItem(ircNetworkTreeView,
                                    QStringList() << "QircNet" << "127.0.0.1"));

    QTreeWidgetItem *freenode = new QTreeWidgetItem(ircNetworkTreeView,
            QStringList() << "FreeNode" << "asimov.freenode.net");
    freenode->addChild(new QTreeWidgetItem(freenode,
            QStringList() << "Austin, TX, US" << "asimov.freenode.net"));
    freenode->addChild(new QTreeWidgetItem(freenode,
            QStringList() << "London, UK, EU" << "holmes.freenode.net"));
    freenode->addChild(new QTreeWidgetItem(freenode,
            QStringList() << "Brisbane, AU" << "roddenberry.freenode.net"));
    freenode->addChild(new QTreeWidgetItem(freenode,
            QStringList() << "Paris, FR, EU" << "barjavel.freenode.net"));
    freenode->addChild(new QTreeWidgetItem(freenode,
            QStringList() << "Luxembourg, LU, EU" << "bartol.freenode.net"));
    ircNetworkTreeView->addTopLevelItem(freenode);

    QTreeWidgetItem *efnet = new QTreeWidgetItem(ircNetworkTreeView,
                                                 QStringList() << "EFNet" << "irc.prison.net");
    efnet->addChild(new QTreeWidgetItem(efnet,
            QStringList() << "San Francisco, CA, US" << "irc.prison.net"));
    efnet->addChild(new QTreeWidgetItem(efnet,
            QStringList() << "Reston, VA, US" << "irc.wh.verio.net"));
    efnet->addChild(new QTreeWidgetItem(efnet,
            QStringList() << "Orlando, FL, US" << "irc.colosolutions.net"));
    efnet->addChild(new QTreeWidgetItem(efnet,
            QStringList() << "Calgary, AB, CA" << "irc.arcti.ca"));
    efnet->addChild(new QTreeWidgetItem(efnet,
            QStringList() << "Toronto, ON, CA" << "irc.teksavvy.ca"));
    efnet->addChild(new QTreeWidgetItem(efnet,
            QStringList() << "Helsinki, FI, EU" << "efnet.cs.hut.fi"));
    efnet->addChild(new QTreeWidgetItem(efnet,
            QStringList() << "Oslo, Norway, EU" << "irc.efnet.no"));
    efnet->addChild(new QTreeWidgetItem(efnet,
            QStringList() << "Warsaw, Poland, EU" << "irc.efnet.pl"));
    efnet->addChild(new QTreeWidgetItem(efnet,
            QStringList() << "Paris, France, EU" << "irc.efnet.fr"));
    efnet->addChild(new QTreeWidgetItem(efnet,
            QStringList() << "Stockholm, Sweden, EU" << "irc.swepipe.se"));
    ircNetworkTreeView->addTopLevelItem(efnet);

    ircNetworkTreeView->addTopLevelItem(
            new QTreeWidgetItem(ircNetworkTreeView,
            QStringList() << "Undernet" << "mesa.az.us.undernet.org"));
    ircNetworkTreeView->addTopLevelItem(
            new QTreeWidgetItem(ircNetworkTreeView,
            QStringList() << "QuakeNet" << "xs4all.nl.quakenet.org"));
    ircNetworkTreeView->addTopLevelItem(
            new QTreeWidgetItem(ircNetworkTreeView,
            QStringList() << "DALnet" << "hash.ix.nl.dal.net"));
}
