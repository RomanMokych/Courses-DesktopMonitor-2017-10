#ifndef FRAME_H
#define FRAME_H

#include <QObject>
#include <QGraphicsScene>

class Frame : public QObject
{
    Q_OBJECT
public:
    explicit Frame(QObject *parent = nullptr);
    Frame(const Frame& f);
    Frame& operator=(const Frame& f);
    void SetTime(const QString time);
    void SetPmap(const QByteArray& buff);
    void SetPmap(const QPixmap& map);
    QPixmap GetPmap()const;
    QString GetTime()const;
    Frame BuildFrame();
signals:

public slots:

private:
    QString time_;
    QPixmap pmap_;
    const QString formatName_ ="PNG";
};

#endif // FRAME_H
