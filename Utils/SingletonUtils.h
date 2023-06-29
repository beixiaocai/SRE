#ifndef SINGLETONUTILS_H
#define SINGLETONUTILS_H

#include <QObject>

QT_BEGIN_NAMESPACE;
class QMutex;
QT_END_NAMESPACE;

class SingletonUtils : public QObject
{
    Q_OBJECT
private:
    explicit SingletonUtils(QObject *parent);
    ~SingletonUtils();

public:
    static SingletonUtils *getInstance();
    void setScreenSize(int width,int height);
    int getScreenWidth();//主屏幕分辨率宽
    int getScreenHeight();//主屏幕分辨率高

    void setFinger(const QString &finger);
    QString getFinger() const;

    void setRecordDir(const QString &recordDir);
    QString getRecordDir() const;

private:
    static QAtomicPointer<SingletonUtils> m_instance;
    static QMutex m_instanceMtx;

    int mScreenWidth;
    int mScreenHeight;
    QString m_finger;//机器指纹
    QString m_recordDir;//录制视频的输出文件夹路径
signals:

};

#endif // SINGLETONUTILS_H
