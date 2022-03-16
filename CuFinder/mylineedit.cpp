#include "mylineedit.h"
#include <QStringListModel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QtDebug>

MyLineEdit::MyLineEdit(QWidget *parent):QLineEdit(parent), m_completor(new MyCompletor(this)),
    m_model(this)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setClearButtonEnabled(true);
    m_cuFolder = getCuFolderFromDB("E:/2-python project/3.9/IDE/ide_data.db");
//    qDebug() << m_cuFolder;
    connect(this, &MyLineEdit::textEdited, this, &MyLineEdit::setMyCompletor);
    setFont(QFont("Times", 14));
    resize(500, 40);
}

void MyLineEdit::setMyCompletor(QString text)
{

}

QString MyLineEdit::getCuFolderFromDB(QString dbPath)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    if (!db.open()){
        QMessageBox msg;
        msg.setText("Database Path is not correct");
        msg.exec();
        return "";
    }
    QSqlQuery qsQuery(db);
    QString queryStr("SELECT * FROM cu_folders ORDER BY ID DESC LIMIT 1");
    qsQuery.prepare(queryStr);
    qsQuery.exec();
    QString res;
    if (qsQuery.next()){
        res = qsQuery.value(1).toString();
    }
    else {
        res = "";
    }
    db.close();
    return res;
}

MyCompletor::MyCompletor(QWidget *parent):QListView(parent)
{
    setWindowFlags(Qt::ToolTip);
    setFocusProxy(parent);
}
