#include "addlanguagedialog.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QHeaderView>
#include <QKeyEvent>

AddLanguageDialog::AddLanguageDialog(const QList<QStringList> &kbids, QWidget *parent)
    : QDialog(parent)
    , m_kbids(kbids)
{
    setWindowTitle(tr("Add Language"));
    setMinimumSize(360, 420);

    m_search = new QLineEdit(this);
    m_search->setPlaceholderText(tr("Search..."));
    m_search->setClearButtonEnabled(true);

    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(2);
    m_model->setHorizontalHeaderLabels({tr("Name"), tr("KBID")});

    m_tableView = new QTableView(this);
    m_tableView->setModel(m_model);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->verticalHeader()->hide();
    m_tableView->verticalHeader()->setDefaultSectionSize(22);

    m_addButton = new QPushButton(tr("Add"), this);
    m_addButton->setEnabled(false);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_search);
    layout->addWidget(m_tableView);
    layout->addWidget(m_addButton);

    fillTable({});

    connect(m_search, &QLineEdit::textChanged,
            this, &AddLanguageDialog::onSearchChanged);

    connect(m_tableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, [this](const QModelIndex &current, const QModelIndex &) {
                m_addButton->setEnabled(current.isValid());
            });

    connect(m_addButton, &QPushButton::clicked,
            this, &QDialog::accept);

    // Double-clicking a row also accepts
    connect(m_tableView, &QTableView::doubleClicked,
            this, &QDialog::accept);
}

QStringList AddLanguageDialog::selectedEntry() const
{
    const QModelIndex idx = m_tableView->currentIndex();
    if (!idx.isValid())
        return {};
    return {
        m_model->item(idx.row(), 0)->text(),
        m_model->item(idx.row(), 1)->text()
    };
}

void AddLanguageDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        if (!m_search->text().isEmpty()) {
            m_search->clear();
            return;
        }
    }
    QDialog::keyPressEvent(event);
}

void AddLanguageDialog::onSearchChanged(const QString &text)
{
    fillTable(text);
}

void AddLanguageDialog::fillTable(const QString &filter)
{
    m_model->removeRows(0, m_model->rowCount());

    const QString lower = filter.toLower();
    for (const auto &entry : m_kbids) {
        if (entry.size() < 2)
            continue;
        if (!lower.isEmpty() && !entry[0].toLower().contains(lower))
            continue;

        auto *nameItem = new QStandardItem(entry[0]);
        auto *kbidItem = new QStandardItem(entry[1]);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        kbidItem->setFlags(kbidItem->flags() & ~Qt::ItemIsEditable);
        m_model->appendRow({nameItem, kbidItem});
    }

    m_tableView->resizeColumnToContents(0);
    m_addButton->setEnabled(false);
}
