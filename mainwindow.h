#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSettings>
#include <QStringList>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


enum class RegType {
    Unknown = 0,
    CurUser = 1,
    System
};

struct Language {
    Language();
    ~Language();

    QString id;
    QString kbid;
    QString name;
    bool invalid;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_pushButtonSave_clicked();

    void on_pushButtonAdd_clicked();

    void on_pushButtonUp_clicked();

    void on_pushButtonDown_clicked();

    void on_pushButtonRemove_clicked();

    void on_pushButtonInfo_clicked();

private:
    void loadLanguages(RegType type);
    RegType Index2RegType(int index);
    QString kbid2Name(QString kbid);
    QString kbid2NameFuzzy(QString kbid);

private:
    QList<QStringList> m_kbids;
    Ui::MainWindow *ui;
    QSettings m_settings;
    RegType m_type;
};
#endif // MAINWINDOW_H
