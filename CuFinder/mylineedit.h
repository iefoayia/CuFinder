#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QLineEdit>
#include <QListView>
#include <QStringListModel>


class MyCompletor : public QListView
{
    Q_OBJECT
public:
    MyCompletor(QWidget *parent=nullptr);
};

class MyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    MyLineEdit(QWidget *parent=nullptr);
private:
    void setMyCompletor(QString);
    QString getCuFolderFromDB(QString dbPath);
public:
    MyCompletor *m_completor;
    QStringListModel m_model;
    QString m_cuFolder;

};
#endif // MYLINEEDIT_H
