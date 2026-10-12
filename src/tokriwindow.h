#ifndef TOKRIWINDOW_H
#define TOKRIWINDOW_H

#include <QListView>
#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QCloseEvent>
#include <QList>
#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui {
class TokriWindow;
}
QT_END_NAMESPACE

class TokriWindow : public QMainWindow
{
    Q_OBJECT

public:
    TokriWindow(QWidget *parent = nullptr);
    ~TokriWindow();
    Ui::TokriWindow* uiHandle();
    void sleep();
    void wakeUp();

public slots:
    void applyTheme();
    void onShakeDetect();
    void deleteSelection();
    void renameSelection();
    void copySelection();
    void openSelection();
    void selectAll();

signals:
    void pasteRequested();
    void themeChanged(bool isDark);

protected:
    void showEvent(QShowEvent *e) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void closeEvent(QCloseEvent *e) override;

private:
    Ui::TokriWindow *ui;

    void init();
    void applyColumnSizing();
    void moveNearCursor();
    void openItem(QString filePath);
    QList<QUrl> selectedUrls() const;
#ifdef Q_OS_WIN
    void updateWindowsTitleBarTheme(bool dark);
#endif
};
#endif // TOKRIWINDOW_H
