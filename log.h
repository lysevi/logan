#ifndef LOG_H
#define LOG_H

#include <QAbstractListModel>
#include "logline.h"


class Log : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QList<LogLine*> lines READ lines  NOTIFY linesChanged)
    Q_PROPERTY(int count READ count  NOTIFY countChanged)
    Q_PROPERTY(QString  name READ name NOTIFY nameChanged)
public:
    enum LogRoles {
        MessageRole = Qt::UserRole + 1,
    };

    explicit Log(QObject *parent = nullptr);
    Log(const QString&name,const QList<LogLine*>&lines, QObject *parent = nullptr);

    static Log* openFile(const QString&fname, QObject *parent = nullptr);
    QList<LogLine*> lines()const;
    int count()const;
    QString name()const;

    void update();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
signals:
    void linesChanged();
    void countChanged(int);
    void nameChanged();
public slots:
protected:
    QHash<int, QByteArray> roleNames() const;
protected:
    QList<LogLine*> m_lines;
    QString m_name;
};

#endif // LOG_H
