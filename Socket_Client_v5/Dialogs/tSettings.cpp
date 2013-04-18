#include "tSettings.h"

//-----------------------------------------------------------------
tSettings::tSettings()
{

}
//-----------------------------------------------------------------
tSettings::~tSettings()
{
    sync();
}
//-----------------------------------------------------------------
void tSettings::SetFilePath(const QString &_ini_file)
{
    ini_path=_ini_file;
    QSettings s(ini_path, QSettings::IniFormat);
    s.beginGroup("server");
    ip=s.value("ip", "127.0.0.1").toString();
    port=s.value("port", 1234).toInt();
    s.endGroup();

    s.beginGroup("client");
    root=s.value("root", "D:/Proba/").toString();
    temp=s.value("temp", "D:/Temp/").toString();
    s.endGroup();
}
//-----------------------------------------------------------------
void tSettings::sync()
{
    QSettings s(ini_path, QSettings::IniFormat);
    s.beginGroup("server");
    s.setValue("ip", ip);
    s.setValue("port", port);
    s.endGroup();

    s.beginGroup("client");
    s.setValue("root", root);
    s.setValue("temp", temp);
    s.endGroup();
}
//-----------------------------------------------------------------
