#ifndef LOG_H
#define LOG_H

#include <QAbstractListModel>
#include <QSet>

//TODO array!
using LinePositionList = QList<QPair<int,int>>;
struct CachedString{
    QString rawValue;
    QString Value;
    QModelIndex mi;
};

class Log : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString  name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString  filename READ filename NOTIFY filenameChanged)
public:
    enum LogRoles {
        MessageRole = Qt::UserRole + 1,
    };

    explicit Log(QObject *parent = nullptr);
    Log(const QString&name,const QString&filename,const QByteArray&bts,const LinePositionList&lines, QObject *parent = nullptr);

    static Log* openFile(const QString&fname, QObject *parent = nullptr);

    QString name()const;
    QString filename()const;

    void update();
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    void clearHeightlight();
    void heighlightWords(const QSet<QString>&sl);
    void updateHeighlights();
    static QString heighlightStr(const QString&str,const QSet<QString>&sl );
signals:
    void linesChanged();
    void countChanged(int);
    void nameChanged();
    void filenameChanged();
public slots:
protected:
    QHash<int, QByteArray> roleNames() const;
protected:
    QByteArray m_bts; //file as bytearray
    LinePositionList m_lines;
    QString m_name;
    QString m_fname;

    mutable QHash<int, CachedString> m_line_cache;
    QSet<QString> m_heighlight_patterns;
};

#endif // LOG_H
