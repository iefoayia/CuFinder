#include "mylineedit.h"
#include "windows.h"
//#include "stdio.h"
//#include "shellapi.h"
//#include "objbase.h"
#include "ShlGuid.h"  // IID_IShellLink
#include "ShObjIdl.h" // CLSID_ShellLink
#include <string>
#include <QStringListModel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QDirIterator>
#include <QIcon>
#include <QPixmap>
#include <QPoint>
#include <QGuiApplication>
#include <QScreen>
#include <QProcess>

#include <QRegularExpression>
#include <QtDebug>

using namespace::std;


MyLineEdit::MyLineEdit(QWidget *parent):QLineEdit(parent), mSetting()
{
    m_completor = new MyCompletor(this);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setClearButtonEnabled(true);

    m_cuFolder = getCuFolderFromDB("E:/2-python project/3.9/IDE/ide_data.db");
//    qDebug() << m_cuFolder;
    connect(this, &MyLineEdit::textEdited, this, &MyLineEdit::setMyCompletor);
    setFont(QFont("Times", 14));
    resize(500, 40);  
    auto screen = QGuiApplication::primaryScreen();
    auto size = screen->availableSize();
    auto x = (size.width() - width()) / 2;
    auto y = (size.height() - height()) / 3;
    move(x, y);
    intialAppList();

}

MyLineEdit::~MyLineEdit()
{
    UnregisterHotKey((HWND)this->winId(), MOD_ALT ^ VK_SPACE);
}

void MyLineEdit::setMyCompletor(QString text)
{
    if (text.startsWith(" ")) {
        m_completor->hide();
        return;
    }else if (text.size() < 3) {
        m_completor->hide();
        return;
    }
//    m_completor->hide();
    if (!findRes.isEmpty()) {
        QListIterator<QListWidgetItem *> findItemIter(findRes);
        while (findItemIter.hasNext()) {
            auto item = findItemIter.next();
            auto row = m_completor->row(item);
            m_completor->setRowHidden(row, true);
        }
    }

    findRes.clear();
    QStringList reStrList;
    reStrList << ".*";
    auto textList = text.split(' ');
    for (int i= 0; i < textList.size(); i++){
        reStrList << textList.at(i) << ".*";
    }

    auto items = m_completor->findItems(reStrList.join(""), Qt::MatchRegularExpression);
    if(items.isEmpty()) {
        m_completor->hide();
        return;
    }
    QListIterator<QListWidgetItem *> findItemIter(items);
    while (findItemIter.hasNext()) {
        auto item = findItemIter.next();
        auto row = m_completor->row(item);
        m_completor->setRowHidden(row, false);
        findRes.append(item);
    }
    if (m_completor->isHidden()) {
        auto p = QPoint(0, height()+1);
        auto completor_pos = p + pos();
        m_completor->setIconSize(QSize(30, 30));
        m_completor->setSpacing(2);
        m_completor->setContentsMargins(4, 1, 1, 2);
        m_completor->resize(width(), m_completor->sizeHint().height());
        m_completor->move(completor_pos);
        m_completor->setCurrentItem(items.at(0));
        m_completor->show();
    }
    else {
        m_completor->update();
    }
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

void MyLineEdit::intialAppList()
{
     //Create COM Object
    CoInitialize(NULL);
    IPersistFile* pPF = NULL;
    HRESULT hr = CoCreateInstance(
                CLSID_ShellLink,
                NULL,
                CLSCTX_INPROC_SERVER,
                IID_IPersistFile,
                (void **)(&pPF)
                );
    if (FAILED(hr)) { qDebug() << " CoCreateInstance failed!";}

    // Query interface
    IShellLink * pSL = NULL;
    hr = pPF->QueryInterface(
                IID_IShellLink,
                (void**)(&pSL)
                );
    if (FAILED(hr)) { qDebug() << " QueryInterface failed!";}

    QString appPath("C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs");
    QDirIterator appDirIter(appPath, {"*.lnk"}, QDir::NoFilter, QDirIterator::Subdirectories);
    while (appDirIter.hasNext()){
        auto lnkPath = appDirIter.next();
//        qDebug() << lnkPath;
        auto wLnkPath = (wchar_t *)lnkPath.utf16();
        wchar_t wRet[MAX_PATH];
        //Open file
        hr = pPF->Load(
                    wLnkPath,
                    STGM_READ);
        if (FAILED(hr)) { qDebug() << " Load failed: " << GetLastError();}
        hr = pSL->GetPath(wRet, MAX_PATH, NULL, 0);
        if (SUCCEEDED(hr)) {
            auto exePath = QString::fromStdWString(wRet);
            exePathList.append(exePath);
            QFileInfo exeLnkInfo(lnkPath);
            QFileInfo exeFileInfo(exePath);
            if (exeLnkInfo.fileName().toLower().contains("netcare")
                    || exeLnkInfo.fileName().toLower().contains("uninstall")
                    || exeLnkInfo.fileName().toLower().contains("ufad")
                    || exeLnkInfo.fileName().toLower().contains("-read")) {
                continue;
            }
            QFileIconProvider fileIconProvider;
            auto icon = fileIconProvider.icon(exeFileInfo);
            if(icon.isNull()) continue;
            exePath_icon_map.insert(exeLnkInfo.fileName().remove(".lnk"), exePath);
            auto *item = new QListWidgetItem(icon, exeLnkInfo.fileName().remove(".lnk"));
            item->setSizeHint(QSize(item->sizeHint().width(), 30));
            QFont font;
            font.setBold(true);
            font.setPixelSize(14);
            item->setFont(font);
            m_completor->addItem(item);
            auto row = m_completor->row(item);
            m_completor->setRowHidden(row, true);
        }
    }
    appPath = "C:\\Users\\a84137140\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu";
    QDirIterator userAppDirIter(appPath, {"*.lnk"}, QDir::NoFilter, QDirIterator::Subdirectories);
    while (userAppDirIter.hasNext()){
        auto lnkPath = userAppDirIter.next();
        auto wLnkPath = (wchar_t *)lnkPath.utf16();
        wchar_t wRet[MAX_PATH];
        //Open file
        hr = pPF->Load(
                    wLnkPath,
                    STGM_READ);
        if (FAILED(hr)) { qDebug() << " Load failed: " << GetLastError();}
        hr = pSL->GetPath(wRet, MAX_PATH, NULL, 0);
        if (SUCCEEDED(hr)) {
            auto exePath = QString::fromStdWString(wRet);
            exePathList.append(exePath);
            QFileInfo exeLnkInfo(lnkPath);
            QFileInfo exeFileInfo(exePath);
            if (exeLnkInfo.fileName().toLower().contains("netcare")
                    || exeLnkInfo.fileName().toLower().contains("uninstall")
                    || exeLnkInfo.fileName().toLower().contains("ufad")
                    || exeLnkInfo.fileName().toLower().contains("-read")) {
                continue;
            }
            QFileIconProvider fileIconProvider;
            auto icon = fileIconProvider.icon(exeFileInfo);
            if(icon.isNull()) continue;
            exePath_icon_map.insert(exeLnkInfo.fileName().remove(".lnk"), exePath);
            auto *item = new QListWidgetItem(icon, exeLnkInfo.fileName().remove(".lnk"));
            item->setSizeHint(QSize(item->sizeHint().width(), 30));
            QFont font;
            font.setBold(true);
            font.setPixelSize(14);
            item->setFont(font);

            m_completor->addItem(item);
            auto row = m_completor->row(item);
            m_completor->setRowHidden(row, true);
        }
    }
    pPF->Release();
    CoUninitialize();

    QDirIterator cuDirIter(m_cuFolder, {"*.txt"});
    while (cuDirIter.hasNext()) {
        auto cuPath = cuDirIter.next();
        QFileInfo cuFile(cuPath);
        auto cuFileName = cuFile.fileName().remove(".txt");
        exePath_icon_map.insert(cuFileName, cuPath);
        QFileIconProvider fileIconProvider;
        auto icon = fileIconProvider.icon(cuFile);
        auto *item = new QListWidgetItem(icon, cuFileName);
        QFont font;
        font.setBold(true);
        font.setPixelSize(14);
        item->setFont(font);

        m_completor->addItem(item);
        auto row = m_completor->row(item);
        m_completor->setRowHidden(row, true);
    }
}

void MyLineEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_Q) {
        close();
    }else if (e->key() == Qt::Key_Escape) {
        hide();
        m_completor->hide();
    }else if(e->key() == Qt::Key_Up || e->key() == Qt::Key_Down){
        m_completor->keyPressEvent(e);
    }
    else if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        auto txt = text();
        if (txt.startsWith(" ")) {
            hide();
            m_completor->hide();

            auto keywordList = txt.split(' ');
            auto key = keywordList.at(1);
            keywordList.takeFirst();
            keywordList.takeFirst();
            auto keyArg = keywordList.join("");
            QString argKey("arg");
            QString exeKey("exe");
            auto arg = mSetting.getValue(key, argKey);
            if (arg.isEmpty()) return;
            arg = arg + keyArg;
            auto exePath = mSetting.getValue(key, exeKey);
            if (exePath.isEmpty()) return;
            QProcess *p = new QProcess(this);
            p->start(exePath, {arg});

        } else {
            if (findRes.isEmpty()) {
                return;
            }
            auto selected = findRes.at(0);
            auto selectedPath = exePath_icon_map.find(selected->data(Qt::DisplayRole).toString()).value();
            if (selectedPath.contains(".txt")) {
                QString ue("E:\\DBackup\\2-Program\\UltraEdit\\UltraEdit\\UltraEdit\\Uedit32.exe ");
                QProcess *p = new QProcess(this);
                p->start(ue, {QDir::toNativeSeparators(selectedPath)});
            } else {
                QProcess *p = new QProcess(this);
                QStringList args;
                p->start(selectedPath, {args.join("")});
            }
        }


    }
    else {
        QLineEdit::keyPressEvent(e);
    }
}

void MyLineEdit::focusOutEvent(QFocusEvent *e)
{
    clear();
    hide();
    m_completor->hide();
}

bool MyLineEdit::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    if (eventType == "windows_generic_MSG"){
        MSG *msg = static_cast<MSG *>(message);
        if(msg->message == WM_HOTKEY){
            const quint32 keycode = HIWORD(msg->lParam);
            const quint32 modifier = LOWORD(msg->lParam);
            if (modifier == MOD_ALT && keycode == VK_SPACE) {
                show();
                activateWindow();
                return true;
            }
        }
    }
    return false;
}

MyCompletor::MyCompletor(QWidget *parent):QListWidget(parent)
{
    setViewMode(QListWidget::ListMode);
    setWindowFlags(Qt::ToolTip);
    setFocusProxy(parent);


}

QRect MyCompletor::rectForIndex(QModelIndex index)
{
    return QListWidget::rectForIndex(index);

}

QSize MyCompletor::sizeHint() const
{
    QSize size;
    size.setHeight(QListWidget::sizeHint().height());
    size.setWidth(static_cast<QWidget *>(parent())->size().width());
    return size;
}

void MyCompletor::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Up || e->key() == Qt::Key_Down){
        QListWidget::keyPressEvent(e);
    }
    else {
        static_cast<MyLineEdit *>(parent())->keyPressEvent(e);
    }
}
