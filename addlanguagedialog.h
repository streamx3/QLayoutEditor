#ifndef ADDLANGUAGEDIALOG_H
#define ADDLANGUAGEDIALOG_H

#include <QDialog>
#include <QList>
#include <QStringList>

class QLineEdit;
class QTableView;
class QStandardItemModel;
class QPushButton;

class AddLanguageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddLanguageDialog(const QList<QStringList> &kbids, QWidget *parent = nullptr);

    // Returns {name, kbid} of the selected row, or an empty list if nothing is selected.
    QStringList selectedEntry() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onSearchChanged(const QString &text);

private:
    void fillTable(const QString &filter);

    QList<QStringList>  m_kbids;
    QLineEdit          *m_search;
    QTableView         *m_tableView;
    QStandardItemModel *m_model;
    QPushButton        *m_addButton;
};

#endif // ADDLANGUAGEDIALOG_H
