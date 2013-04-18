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
    s.beginGroup("net");
    ip=s.value("ip", "127.0.0.1").toString();
    port=s.value("port", 1234).toInt();
    s.endGroup();

    s.beginGroup("files");
    root=s.value("root", "C:/Users/Public/Pictures/Sample Pictures/").toString();
    temp=s.value("temp", "D:/ServerTemp/").toString();
    max_readers=s.value("max_readers", 20).toInt();
    s.endGroup();
}
//-----------------------------------------------------------------
void tSettings::sync()
{
    QSettings s(ini_path, QSettings::IniFormat);
    s.beginGroup("net");
    s.setValue("ip", ip);
    s.setValue("port", port);
    s.endGroup();

    s.beginGroup("files");
    s.setValue("root", root);
    s.setValue("temp", temp);
    s.setValue("max_readers", max_readers);
    s.endGroup();
}
//-----------------------------------------------------------------
