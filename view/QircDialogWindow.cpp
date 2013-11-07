#include "QircDialogWindow.h"

QircDialogWindow::QircDialogWindow(QWidget *parent) :
    QWidget(parent)
{
    textBrowser = new QTextBrowser(this);
    textBrowser->setFont(QFont("Courier", 10));
    textBrowser->setReadOnly(true);

    lineEdit = new QLineEdit(this);

    button = new QPushButton(this);
    button->setText("OK");
    button->setMaximumWidth(200);
    QObject::connect(button, SIGNAL(clicked()),
		     this, SLOT(onButtonPressed()));

    vbox = new QVBoxLayout(this);
    vbox->addWidget(textBrowser,8);
    vbox->addWidget(lineEdit,1);
    vbox->addWidget(button,1);
    vbox->setAlignment(button, Qt::AlignHCenter);

    QObject::connect(lineEdit, SIGNAL(returnPressed()),
		     this, SLOT(onButtonPressed()));

    this->setLayout(vbox);
    this->resize(680, 500);
    this->setWindowIcon(QIcon("Qirc.ico"));
}

void QircDialogWindow::showAbout()
{
    QTextCursor cursor = QTextCursor(textBrowser->document());
    this->setWindowTitle("About Qirc");
    textBrowser->clear();
    cursor.insertHtml(
"<table align='center'><tr><td>"
"<img width='64' height='64' src='Qirclogo.png'/></td><td>"
"<div align='center'>"

"<h2 align='center' style='font-family: Arial,sans-serif'>"
"Qirc IRC Client</h2>"
"<br/><span style='font-weight: bold'>Version:</span> 0.3.0<br/>"
"<span style='font-weight: bold'>Author:</span> Nick Manley"
"<br/>"
"</div></td></tr></table>"
);
    cursor.setPosition(0, QTextCursor::MoveAnchor);
    textBrowser->setTextCursor(cursor);
    textBrowser->show();
    lineEdit->hide();
    button->setText("OK");
    this->setFixedSize(300,200);
    this->centerDialog();
    this->show();
}

void QircDialogWindow::showLicense()
{
    QTextCursor cursor = textBrowser->textCursor();
    this->setWindowTitle("License");
    textBrowser->clear();
    cursor.insertText(
"Copyright (c) 2013, Nick Manley\n"
"All rights reserved.\n\n"
"Redistribution and use in source and binary forms, with or without modification, "
"are permitted provided that the following conditions are met:\n\n"

"1. Redistributions of source code must retain the above copyright notice, "
"this list of conditions and the following disclaimer.\n\n"

"2. Redistributions in binary form must reproduce the above copyright notice, "
"this list of conditions and the following disclaimer in the documentation and/or "
"other materials provided with the distribution.\n\n"

"3. Neither the name of Qirc nor the names of its contributors "
"may be used to endorse or promote products derived from this software "
"without specific prior written permission.\n\n"

"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "
"\"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, "
"THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE "
"ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE "
"FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL "
"DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR "
"SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER "
"CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, "
"OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE "
"OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.");
    cursor.setPosition(0, QTextCursor::MoveAnchor);
    textBrowser->setTextCursor(cursor);
    lineEdit->hide();
    textBrowser->show();
    button->setText("OK");
    this->setFixedSize(680,500);
    this->centerDialog();
    this->show();
}

void QircDialogWindow::showHelp()
{
    QTextCursor cursor = QTextCursor(textBrowser->document());
    this->setWindowTitle("Qirc Help");
    textBrowser->clear();
    cursor.insertHtml(
"<h2 align='center' style='font-family: Arial,sans-serif'>Qirc Commands</h2>"
"<br/><span style='font-weight: bold'>/help</span> - Display this dailog<br/>"
"<span style='font-weight: bold'>/license</span> - Display the Qirc License"
"<br/><span style='font-weight: bold'>/about</span> - Obtain version and "
"author information about Qirc<br/><span style='font-weight: bold'>/font "
"[name] [size]</span> - Change the font of the chat dialog."
);
    cursor.setPosition(0, QTextCursor::MoveAnchor);
    textBrowser->setTextCursor(cursor);
    textBrowser->show();
    lineEdit->hide();
    this->setFixedSize(500,300);
    this->centerDialog();
    this->show();
}

void QircDialogWindow::showJoinChannel()
{
    textBrowser->hide();
    lineEdit->setText("#");
    lineEdit->setFocus(Qt::ActiveWindowFocusReason);
    lineEdit->show();
    button->setText("Join");
    this->setFixedSize(250,80);
    this->setWindowTitle("Join Channel");
    this->centerDialog();
    this->show();
}

void QircDialogWindow::showChangeNick()
{
    textBrowser->hide();
    lineEdit->clear();
    lineEdit->setFocus(Qt::ActiveWindowFocusReason);
    lineEdit->show();
    button->setText("Change Nick");
    this->setFixedSize(250,80);
    this->setWindowTitle("Change Nick");
    this->centerDialog();
    this->show();
}

void QircDialogWindow::centerDialog()
{
    QRect available_geom = QDesktopWidget().availableGeometry();
    QRect current_geom = frameGeometry();
    setGeometry(available_geom.width() / 2 - current_geom.width() / 2,
		available_geom.height() / 2 - current_geom.height() / 2,
		current_geom.width(),
		current_geom.height());
}

QircDialogWindow::~QircDialogWindow()
{
    delete textBrowser;
    delete button;
    delete vbox;
}

void QircDialogWindow::onButtonPressed()
{
    if(button->text() == "Join")
    {
	emit this->inputTextEntered("/join " + lineEdit->text());
    }
    else if(button->text() == "Change Nick")
    {
	emit this->inputTextEntered("/nick " + lineEdit->text());
    }
    this->close();
}
