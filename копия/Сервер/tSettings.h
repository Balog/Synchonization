#ifndef TSETTINGS_H
#define TSETTINGS_H
#include <QSettings>

struct ServerSetting
{
    QString ip;
    int port;
};

class tSettings
{
public:
    tSettings();
    ~tSettings();
    void SetFilePath(const QString &_ini_file);
    void sync();

    inline const QString GetRoot() const  {return root;}
    inline const QString GetTemp() const  {return temp;}
    inline const QString GetServerAddr() {return ip;}
    inline const int GetServerPort() const  {return port;}
    inline const int GetMaxReaders() const  {return max_readers;}

    inline void SetRoot(const QString &_root) {root=_root;}
    inline void SetTemp(const QString &_temp) {temp=_temp;}
    inline void SetServerAddr(const QString &_ip) {ip=_ip;}
    inline void SetServerPort(const int _port) {port=_port;}
    inline void SetMaxReaders(const int _max_readers) {max_readers=_max_readers;}




private:
    QString root;
    QString temp;
    QString ip;
    int port;
    int max_readers;

    QString ini_path;
};

#endif // TSETTINGS_H
