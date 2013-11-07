#ifndef IRCSESSIONTREE_H
#define IRCSESSIONTREE_H

#include <QObject>
#include <QAbstractItemModel>

class IrcSessionTree : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit IrcSessionTree(QObject *parent = 0);

signals:

public slots:

};

#endif // IRCSESSIONTREE_H
