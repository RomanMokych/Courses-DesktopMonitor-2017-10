#ifndef FRAME_H
#define FRAME_H

#include <QObject>
#include <QGraphicsScene>

class Frame : public QObject
{
    Q_OBJECT
public:
    explicit Frame(QObject *parent = nullptr);
    Frame(const Frame& frame);
    Frame& operator=(const Frame& frame);
    void SetTime(const QString time);
    void SetPmap(const QByteArray& buff);
    void SetPmap(const QPixmap& map);
    QPixmap GetPmap()const;
    QString GetTime()const;
signals:

public slots:

private:
    QString time_;
    QPixmap map_;
    const QString format_name_ ="PNG";
};

#endif // FRAME_H
