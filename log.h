#ifndef LOG_H
#define LOG_H

#include <QAbstractListModel>
#include <QSet>
#include <future>
#include <memory>

using HighlightPatterns=QSet<QString>;

struct LinePosition{
    int first;
    int second;
    int index;
};

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
    enum LogRoles {
        MessageRole = Qt::UserRole + 1,
    };

    explicit Log(QObject *parent = nullptr);
    Log(const QString&name,
        const QString&filename,
        const HighlightPatterns *global_highlight,
        QObject *parent = nullptr);

    static Log* openFile(const QString&fname, const HighlightPatterns *global_highlight, QObject *parent = nullptr);

    QString name()const;
    QString filename()const;

    void update();
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE void clearHightlight();
    Q_INVOKABLE void localHightlightPattern(const QString&pattern);
    void updateHeighlights(const QString&pattern);
    static bool heighlightStr(QString* str,const QString&pattern );

    Q_INVOKABLE void setQmlObject(QObject *object);
signals:
    void linesChanged();
    void countChanged(int);
    void nameChanged();
    void filenameChanged();
public slots:
protected:
    QHash<int, QByteArray> roleNames() const;
    void loadFile();
    void initBuffer(const uchar*bts, const LinePositionList&lines);
protected:
    bool m_load_complete=false;
    QString m_name;
    QString m_fname;

    QVector<CachedString> m_buffer;
    const HighlightPatterns *m_global_highlight;

    QObject *m_qml_object;
};

#endif // LOG_H
