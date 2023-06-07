#include "SingletonUtils.h"
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <QsLog.h>

QAtomicPointer<SingletonUtils> SingletonUtils::m_instance = 0;
QMutex SingletonUtils::m_instanceMtx;


SingletonUtils::SingletonUtils(QObject *parent): QObject(parent){
}
SingletonUtils::~SingletonUtils(){
}


SingletonUtils * SingletonUtils::getInstance(){
    //! testAndSetOrders操作保证在原子操作前和后的的内存访问 不会被重新排序。
    if(m_instance.testAndSetOrdered(0,0)){
        QMutexLocker locker(&m_instanceMtx);
        m_instance.testAndSetOrdered(0,new SingletonUtils(nullptr));
    }
    return m_instance;

}


void SingletonUtils::setFinger(const QString &finger){
    m_finger = finger;
}
QString SingletonUtils::getFinger() const{
    return m_finger;
}

void SingletonUtils::setRecordDir(const QString &recordDir){
    m_recordDir = recordDir;
}
QString SingletonUtils::getRecordDir() const{
    return m_recordDir;
}
