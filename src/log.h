#ifndef LOG_H
#define LOG_H

#include <QAbstractItemModel>
#include <QSet>
#include <QFileInfo>
#include <QDateTime>
#include <QListView>
#include <map>
#include <vector>
#include <future>
#include <memory>

struct HighlightPattern{
    QString pattern;
    QString rgb;
};

using HighlightPatterns=QMap<QString, HighlightPattern>;

struct LinePosition{
    int first;
    int second;
    int index;//TODO remove
};

//TODO hashset?
using LinePositionList = std::vector<LinePosition>;

struct CachedString{
    int index;
    std::shared_ptr<QString> originValue;
    std::shared_ptr<QString> Value;
};

const QString dateRe="\\d{2}:\\d{2}:\\d{2}\\.?\\d*";

enum class SearchDirection{
    Up, Down
};


class Log : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(QString  name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString  filename READ filename NOTIFY filenameChanged)
public:
    Log(const QFileInfo& fileInfo, const QString&filename, const HighlightPatterns *global_highlight,const QString&default_encoding, QObject *parent = nullptr);

    static Log* openFile(const QString&fname, const HighlightPatterns *global_highlight, const QString& defaul_encoding, QObject *parent = nullptr);

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
    int columnCount(const QModelIndex &parent = QModelIndex()) const override{
        Q_UNUSED(parent);
        return 1;
    }
    Qt::ItemFlags flags(const QModelIndex &index) const{
        Q_UNUSED(index)
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex())const override{
        if (!hasIndex(row, column, parent)) {
            return QModelIndex();
        }


        return createIndex(row, column);
    }

    QModelIndex parent(const QModelIndex &/*child*/) const override{
        return QModelIndex();
    }

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    Q_INVOKABLE void clearHightlight();
    Q_INVOKABLE void localHightlightPattern(const QString&pattern);
    void updateHeighlights(QVector<CachedString>::iterator begin,
                           QVector<CachedString>::iterator end,
                           const QString&pattern);
    void updateHeighlights(const QString&pattern);
    static bool heighlightStr(QString* str,const HighlightPattern&pattern );

    Q_INVOKABLE void setListVoxObject(QListView *object);

    QString plainText(const QModelIndex & index)const;


    QPair<int, QString> findFrom(const QString&pattern,int index, SearchDirection direction);
signals:
    void linesChanged();
    void countChanged(int);
    void nameChanged();
    void filenameChanged();
public slots:
protected:
    void loadFile();
    std::shared_ptr<QString> makeString(int row, bool isPlain=false)const;
protected:
    bool m_load_complete=false;
    QString m_name;
    QString m_fname;

    LinePositionList m_lines;
    QByteArray m_bts;
    mutable std::map<int, CachedString> m_cache;
    const HighlightPatterns *m_global_highlight;

    QTextCodec * m_codec;
    QListView  *m_lv_object;
    QFileInfo m_fileInfo;
    QDateTime m_lastModifed;
    QString m_default_encoding;
};

#endif // LOG_H
