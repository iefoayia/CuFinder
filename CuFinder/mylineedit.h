#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QLineEdit>
#include <QListWidget>
#include <QStringListModel>
#include <QList>
#include <QListWidgetItem>
#include <QMap>
#include <QKeyEvent>
#include <QIcon>
#include <QAbstractNativeEventFilter>
#include "setting.h"


class MyCompletor : public QListWidget
{
    Q_OBJECT
public:
    MyCompletor(QWidget *parent=nullptr);
    QRect rectForIndex(QModelIndex);
    QSize sizeHint() const override;
    virtual void keyPressEvent(QKeyEvent *e) override;
};

class MyLineEdit : public QLineEdit, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    MyLineEdit(QWidget *parent=nullptr);
    ~MyLineEdit();
private:
    void setMyCompletor(QString);
    QString getCuFolderFromDB(QString dbPath);
    void intialAppList();

public:
    // members
    MyCompletor *m_completor;
    QList<QListWidgetItem> itemList;
    QString m_cuFolder;
    QStringList exePathList;
    QMap<QString, QString> exePath_icon_map;
    QList<QListWidgetItem *> findRes;
    QHash<QString, QPair<QString, QString>> shortcutHash;
    Setting mSetting;


    // functions
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);


};
#endif // MYLINEEDIT_H
