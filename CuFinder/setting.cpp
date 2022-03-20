#include "setting.h"
#include <QCoreApplication>
#include <QDir>
#include <QDebug>


Setting::Setting(QString fileName)
{
    if (fileName.isEmpty()) {
        mSettingsFileName = "/setting.ini";
    }
    else {
        mSettingsFileName = fileName;
    }
    auto mSettingsFilePath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + mSettingsFileName);
//    qDebug() << mSettingsFilePath;



    mSettings = new QSettings(mSettingsFilePath, QSettings::IniFormat);
    if (!QDir(mSettingsFilePath).exists()) {
        QString chrome("C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe");
        QString explore("C:\\Windows\\explorer.exe");
        QString python("E:\\3-Program\\python39\\python.exe");

        QString argKey("arg");
        QString exeKey("exe");
        setKeyValue("baidu", argKey, "https://www.baidu.com/s?ie=utf-8&f=3&rsv_bp=1&rsv_idx=1&tn=baidu&wd=");
        setKeyValue("baidu", exeKey, chrome);
        setKeyValue("qt", argKey, "doc.qt.io/qt-5/classes.html#");
        setKeyValue("qt", exeKey, chrome);
        setKeyValue("net", argKey, "https://netcare.huawei.com/p/netcare/index.html#/home");
        setKeyValue("net", exeKey, chrome);
        setKeyValue("container", argKey, "https://doc.qt.io/qt-5/containers.html");
        setKeyValue("container", exeKey, chrome);
        setKeyValue("hed", argKey, "http://127.0.0.1:7890/newhdx.cgi?fe=0&lib=AZK1025J&v=02&homepage=resources/hedex-homepage.html");
        setKeyValue("hed", exeKey, chrome);

        setKeyValue("sdn", argKey, "E:\\3-Desktop\\Aiyaofei\\IPMetro\\SDN");
        setKeyValue("sdn", exeKey, explore);
        setKeyValue("cu", argKey, "E:\\3-Desktop\\configration");
        setKeyValue("cu", exeKey, explore);
        setKeyValue("ayf", argKey, "E:\\3-Desktop\\Aiyaofei");
        setKeyValue("ayf", exeKey, explore);
        setKeyValue("c", argKey, "E:\\3-Desktop\\Book\\C&C++");
        setKeyValue("c", exeKey, explore);
        setKeyValue("book", argKey, "E:\\3-Desktop\\Book");
        setKeyValue("book", exeKey, explore);
        setKeyValue("top", argKey, "E:\\2-python project\\3.9\\routerApp\\topoWiget.py");
        setKeyValue("top", exeKey, python);
    }
}

Setting::~Setting()
{
    delete mSettings;
    mSettings = NULL;
}

void Setting::setKeyValue(QString node, QString key, QString v)
{
    QString k = QString("/%1/%2").arg(node).arg(key);
    mSettings->setValue(k, QVariant(v));
}

QString Setting::getValue(QString node, QString key)
{
    QString k = QString("/%1/%2").arg(node).arg(key);
    auto v = mSettings->value(k, QVariant()).toString();
    return v;
}
