#ifndef LOG_H
#define LOG_H

#include <QAbstractListModel>
#include <QSet>
#include <QFileInfo>
#include <QDateTime>
#include <QListView>
#include <map>
#include <future>
#include <memory>

using HighlightPatterns=QSet<QString>;

struct LinePosition{
    int first;
    int second;
    int index;//TODO remove
};

//TODO hashset?
using LinePositionList = QVector<LinePosition>;

struct CachedString{
    int index;
    std::shared_ptr<QString> originValue;
    std::shared_ptr<QString> Value;
};

const QString dateRe="\\d{2}:\\d{2}:\\d{2}\\.?\\d*";

class Log : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString  name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString  filename READ filename NOTIFY filenameChanged)
public:

    explicit Log(QObject *parent = nullptr);
    Log(const QFileInfo& fileInfo,
        const QString&filename,
        const HighlightPatterns *global_highlight,
        QObject *parent = nullptr);

    static Log* openFile(const QString&fname, const HighlightPatterns *global_highlight, QObject *parent = nullptr);

    QString name()const;
    QString filename()const;

    void update();
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role){
        Q_UNUSED(index)
        Q_UNUSED(value)
        Q_UNUSED(role)
        return true;
    }
    Qt::ItemFlags flags(const QModelIndex &index) const{
        Q_UNUSED(index)
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    Q_INVOKABLE void clearHightlight();
    Q_INVOKABLE void localHightlightPattern(const QString&pattern);
    void updateHeighlights(QVector<CachedString>::iterator begin,
                           QVector<CachedString>::iterator end,
                           const QString&pattern);
    void updateHeighlights(const QString&pattern);
    static bool heighlightStr(QString* str,const QString&pattern );

    Q_INVOKABLE void setListVoxObject(QListView *object);
signals:
    void linesChanged();
    void countChanged(int);
    void nameChanged();
    void filenameChanged();
public slots:
protected:
    void loadFile();
    QString makeString(int row)const;
protected:
    bool m_load_complete=false;
    QString m_name;
    QString m_fname;

    LinePositionList m_lines;
    QByteArray m_bts;
    //std::map<int, CachedString> m_cache;
    const HighlightPatterns *m_global_highlight;

    QListView  *m_lv_object;
    QFileInfo m_fileInfo;
    QDateTime m_lastModifed;
};

#endif // LOG_H
