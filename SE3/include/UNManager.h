#ifndef UNMANAGER_H
#define UNMANAGER_H
#include <QString>

class UNManager{

public:
    UNManager() : m_UN(0){}
    QString getQUN() {  return (!m_UN) ? "" : QString::number(++m_UN);}
    int getUN() {return m_UN;}

private:
    int m_UN;
};

#endif // UNMANAGER_H
