#ifndef LOG_H
#define LOG_H

#include <QAbstractListModel>
#include <QSet>
#include <memory>
using HighlightPatterns= QSet<QString>;

//TODO array!
using LinePositionList = QList<QPair<int,int>>;
struct CachedString{
    std::shared_ptr<QString> rawValue;
    std::shared_ptr<QString> Value; //TODO pointer to string!
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
        HighlightPatterns *global_highlight,
        QObject *parent = nullptr);

    static Log* openFile(HighlightPatterns *global_highlight, const QString&fname, QObject *parent = nullptr);

    QString name()const;
    QString filename()const;

    void update();
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE void clearHeightlight();
    Q_INVOKABLE void addHeighlightPatter(const QString&sl);
    void updateHeighlights();
    static bool heighlightStr(QString* str,const HighlightPatterns&sl );
signals:
    void linesChanged();
    void countChanged(int);
    void nameChanged();
    void filenameChanged();
public slots:
protected:
    QHash<int, QByteArray> roleNames() const;
    void loadFile();
protected:
    QByteArray m_bts; //file as bytearray
    LinePositionList m_lines;
    QString m_name;
    QString m_fname;

    mutable QHash<int, CachedString> m_line_cache;
    HighlightPatterns m_heighlight_patterns;
    HighlightPatterns *m_global_highlight;
};

#endif // LOG_H
