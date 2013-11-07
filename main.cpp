#include <QtWidgets/QApplication>
#include "IrcController.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ircController = IrcController::instance();

    return app.exec();
}

/*
 TODO

 - BAN command
 - KICKBAN command
 - INVITE command
 - STATS command
 - ADMIN command
 - INFO command
 - WHOIS command
 - WHOWAS command
 - Add @/+ marker for own messages
 - status bar: Qirc | #Reddit +iwx | 400 users
 - only ask for quit confirmation if connected to a server
 - clickable URLs
 - settings UI
 - CTCP Quoting
 - DCC
 */

// Approximate Total Lines of Code: 2069
// Version changes at
//	IrcSocket.cpp  174
//	QircDialogWindow.cpp  38
