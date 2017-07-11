#ifndef LOG_H
#define LOG_H

#include <QAbstractListModel>

//TODO array!
using LinePositionList = QList<QPair<int,int>>;

class Log : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString  name READ name NOTIFY nameChanged)
public:
    enum LogRoles {
        MessageRole = Qt::UserRole + 1,
    };

    explicit Log(QObject *parent = nullptr);
    Log(const QString&name,const QByteArray&bts,const LinePositionList&lines, QObject *parent = nullptr);

    static Log* openFile(const QString&fname, QObject *parent = nullptr);
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
    QByteArray m_bts; //file as bytearray
    LinePositionList m_lines;
    QString m_name;
};

#endif // LOG_H
