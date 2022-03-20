#ifndef SETTING_H
#define SETTING_H
#include <QSettings>
#include <QString>
#include <QVariant>


class Setting
{
public:
    Setting(QString fileName="");
    ~Setting();
    void setKeyValue(QString node, QString key, QString v);
    QString getValue(QString node, QString key);

private:
    QSettings * mSettings;
    QString mSettingsFileName;
};

#endif // SETTING_H
