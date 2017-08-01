#include "logviewer.h"
#include "log.h"
#include "ui_logviewer.h"
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QKeyEvent>
#include <QMenu>
#include <QScrollBar>

LogViewer::LogViewer(const QFont &font, QWidget *parent)
    : QWidget(parent), ui(new Ui::LogViewer) {
  m_default_font = font;
  qDebug() << "font: " << m_default_font.toString();
  ui->setupUi(this);
  auto lb = ui->listView;

  m_delegate.m_default_font = m_default_font;

  lb->setAlternatingRowColors(true);
  lb->setItemDelegate(&m_delegate);
  lb->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
  lb->installEventFilter(this);
  lb->setUniformItemSizes(true);
  lb->setLayoutMode(QListView::LayoutMode::Batched);
  lb->setSpacing(0);

  lb->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
  connect(lb, SIGNAL(customContextMenuRequested(const QPoint &)), this,
          SLOT(customContextMenu(const QPoint &)));
  connect(lb->verticalScrollBar(), &QScrollBar::rangeChanged, this,
          &LogViewer::onScrollRangeChanged);
  connect(ui->actioncopy, &QAction::triggered, this, &LogViewer::copySelectedSlot);
  connect(ui->listView, &QListView::clicked, this, &LogViewer::onSelectionChangedSlot);
  m_progress_dlg = nullptr;
}

LogViewer::~LogViewer() {
  delete ui;
}

void LogViewer::setModel(Log *model_) {
  this->ui->listView->setModel(model_);
  model_->setListVoxObject(this);
  m_model = model_;
  connect(model_, SIGNAL(rowsInserted(QModelIndex, int, int)), this->ui->listView,
          SLOT(scrollToBottom()));
}

void LogViewer::setAutoScroll(bool value) {
  m_autoscroll = value;
  if (value) {
    ui->listView->scrollToBottom();
  }
}

void LogViewer::onScrollRangeChanged(int min, int max) {
  Q_UNUSED(min);
  Q_UNUSED(max);
  if (m_autoscroll) {
    ui->listView->scrollToBottom();
  }
}

void LogViewer::copySelectedSlot() {
  qDebug() << "copySelectedSlot";
  auto selectedIndexes = this->ui->listView->selectionModel()->selectedIndexes();
  QClipboard *clipboard = QApplication::clipboard();
  QStringList sl;
  for (auto &i : selectedIndexes) {
    auto plainText = m_model->plainText(i);
    plainText.remove("\u0000");
    plainText.remove("\0");
    plainText.remove('\r');
    plainText.remove('\n');
    sl.append(plainText);
  }
  auto cs = sl.join("\n");
  qDebug() << cs;
  clipboard->setText(cs);
}

void LogViewer::customContextMenu(const QPoint &) {
  qDebug() << "customContextMenu";
  QMenu menu;
  menu.addAction(this->ui->actioncopy);
  menu.exec(QCursor::pos());
}

bool LogViewer::eventFilter(QObject *object, QEvent *event) {
  if (object == ui->listView && event->type() == QEvent::KeyPress) {
    QKeyEvent *ke = static_cast<QKeyEvent *>(event);
    if (ke->key() == Qt::Key_C &&
        ke->modifiers() == Qt::KeyboardModifier::ControlModifier) {
      this->copySelectedSlot();
      return true;
    }
  }
  return false;
}

void LogViewer::selectRow(int row) {
  ui->listView->selectionModel()->clearSelection();

  auto index = m_model->index(row, 0);

  ui->listView->selectionModel()->select(index,
                                         QItemSelectionModel::SelectionFlag::Select);
  ui->listView->scrollTo(index);
}

int LogViewer::selectedRow() {
  auto si = ui->listView->selectionModel()->selectedIndexes();
  if (si.empty()) {
    return 0;
  } else {
    return si.first().row();
  }
}

void LogViewer::onSelectionChangedSlot() {
  qDebug() << "onSelectionChangedSlot";
  emit selectNewRow();
}

void LogViewer::longOperationStart(const QString &title) {
  qDebug() << "LogViewer::longOperationStartSlot() " << title;
  m_progress_dlg =
      std::shared_ptr<QProgressDialog>{new QProgressDialog(title, "", 0, 100, this)};
  m_progress_dlg->setWindowModality(Qt::WindowModal);
  m_progress_dlg->showNormal();
}

void LogViewer::longOperationStop() {
  qDebug() << "LogViewer::longOperationStopSlot()";
  m_progress_dlg->close();
  m_progress_dlg = nullptr;
}

void LogViewer::progress(int percent) {
  qDebug() << "progressSlot:" << percent;
  if (m_progress_dlg != nullptr) {
    m_progress_dlg->setValue(percent);
  }
}
