#include "tSettings.h"
#include <QTextCodec>

//-----------------------------------------------------------------
tSettings::tSettings()
{
    QTextCodec *codec =QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);
}
//-----------------------------------------------------------------
tSettings::~tSettings()
{
    sync();
    l="----------------------------------------------------";
    log.Write(l, false);
}
//-----------------------------------------------------------------
void tSettings::SetFilePath(const QString &_ini_file)
{
    l="----------------------------------------------------";
    log.Write(l, false);
    l="tSettings \tSetFilePath\tRead parameters ";
    log.Write(l);

    ini_path=_ini_file;
    QSettings s(ini_path, QSettings::IniFormat);
    s.beginGroup("server");
    ip=s.value("ip", "127.0.0.1").toString();
    port=s.value("port", 1234).toInt();
    s.endGroup();

}
//-----------------------------------------------------------------
void tSettings::sync()
{
    l="tSettings \tsync\tSave parameters ";
    log.Write(l);

    QSettings s(ini_path, QSettings::IniFormat);
    s.beginGroup("server");
    s.setValue("ip", ip);
    s.setValue("port", port);
    s.endGroup();

}
//-----------------------------------------------------------------
