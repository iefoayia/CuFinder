#include "mylineedit.h"
#include <QApplication>
#include "Windows.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//    a.setStyleSheet("QListWidget::item { height: 30px;}");
    MyLineEdit *w = new MyLineEdit();
    if (RegisterHotKey((HWND)w->winId(), MOD_ALT ^ VK_SPACE, MOD_ALT, VK_SPACE) == false) {
        QMessageBox msgbox;
        msgbox.setText("Register hotkey failed!");
    }
    app.installNativeEventFilter(w);
    w->show();
    return app.exec();
}
