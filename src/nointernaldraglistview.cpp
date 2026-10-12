#include "nointernaldraglistview.h"

#include "loghelpers.h"
#include "themeprovider.h"

#include <QDragEnterEvent>
#include <QGuiApplication>
#include <QMouseEvent>

namespace {

void preferCopy(QDropEvent *e)
{
    if (e->isAccepted()) {
        e->setDropAction(Qt::CopyAction);
        e->accept();
    }
}

}

NoInternalDragListView::NoInternalDragListView() {}

void NoInternalDragListView::startDrag(Qt::DropActions supportedActions)
{
    const Qt::KeyboardModifiers mods = QGuiApplication::keyboardModifiers();
#ifdef Q_OS_MACOS
    const bool copy = mods.testFlag(Qt::AltModifier);
    if (copy) {
        supportedActions = Qt::CopyAction;
    }
#else
    const bool copy = mods.testFlag(Qt::ControlModifier);
#endif
    setDefaultDropAction(copy ? Qt::CopyAction : Qt::MoveAction);
    QListView::startDrag(supportedActions);
}

QItemSelectionModel::SelectionFlags
NoInternalDragListView::selectionCommand(const QModelIndex &index,
                                         const QEvent *event) const
{
    if (event && event->type() == QEvent::MouseButtonPress) {
        const auto *mouseEvent = static_cast<const QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::RightButton
            && selectionModel()
            && selectionModel()->hasSelection())
            return QItemSelectionModel::NoUpdate;
    }

    return QListView::selectionCommand(index, event);
}

void NoInternalDragListView::dragEnterEvent(QDragEnterEvent *e)
{
    const bool isSelf = e->source() == this;
    if (isSelf)
        e->ignore();
    else {
        emit dropping(true);
        QListView::dragEnterEvent(e);
        preferCopy(e);
    }
}

void NoInternalDragListView::dragMoveEvent(QDragMoveEvent *e)
{
    const bool isSelf = e->source() == this;
    if (isSelf)
        e->ignore();
    else {
        QListView::dragMoveEvent(e);
        preferCopy(e);
    }
}

void NoInternalDragListView::dragLeaveEvent(QDragLeaveEvent *e)
{
    Logger &log = Logger::instance();
    log.push("dragLeave");
    log.log() << "leave";

    emit dropping(false);

    QListView::dragLeaveEvent(e);

    log.pop();
}

void NoInternalDragListView::dropEvent(QDropEvent *e)
{
    Logger &log = Logger::instance();
    log.push("dropEvent");

    log.log() << "source==this=" << (e->source() == this)
              << "dropAction=" << e->dropAction()
              << "possibleActions=" << e->possibleActions()
              << "formats=" << e->mimeData()->formats();
    emit dropping(false);

    e->setDropAction(Qt::CopyAction);

    QListView::dropEvent(e);

    log.log() << "accepted=" << e->isAccepted()
              << "finalDropAction=" << e->dropAction();

    log.pop();
}

void NoInternalDragListView::emitVisibleCount()
{
    const QSize grid = gridSize();
    if (grid.isEmpty())
        return;

    const QSize vp = viewport()->size();
    const int cols = vp.width() / grid.width() + 1;
    const int rows = vp.height() / grid.height() + 1;
    const int count = cols * rows;

    if (count == mLastVisibleCount)
        return;

    mLastVisibleCount = count;
    emit visibleCountChanged(count);
}

void NoInternalDragListView::paintEvent(QPaintEvent *e) {
    emitVisibleCount();

    QListView::paintEvent(e);

    auto *m = model();
    if (!m || m->rowCount(rootIndex()) > 0)
        return;

    QPainter p(viewport());
    p.setRenderHint(QPainter::Antialiasing, true);

    QPixmap rawPm(":/background.png");
    if (rawPm.isNull())
        return;

    const bool isDark = ThemeProvider::isDark();
    const QColor iconColor = isDark ? QColor(160, 150, 180, 140) : QColor(117, 55, 241, 140);

    const int iconSize = 96;
    QPixmap pm = rawPm.scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Tint icon to match theme
    QPixmap tinted(pm.size());
    tinted.fill(Qt::transparent);
    {
        QPainter tp(&tinted);
        tp.drawPixmap(0, 0, pm);
        tp.setCompositionMode(QPainter::CompositionMode_SourceIn);
        tp.fillRect(tinted.rect(), iconColor);
    }

    const QString title = QStringLiteral("Basket is empty");
    const QString subtitle = QStringLiteral("Drop files, text, or links here\nShake cursor to summon Tokri");

    QFont titleFont = font();
    titleFont.setPointSize(11);
    titleFont.setBold(true);

    QFont subtitleFont = font();
    subtitleFont.setPointSize(8);
    subtitleFont.setBold(false);

    QFontMetrics titleFm(titleFont);
    QFontMetrics subtitleFm(subtitleFont);

    const int titleH = titleFm.height();
    const int subtitleH = subtitleFm.lineSpacing() * 2;
    const int gap1 = 12; // gap between icon and title
    const int gap2 = 6;  // gap between title and subtitle

    const int totalContentH = tinted.height() + gap1 + titleH + gap2 + subtitleH;
    const int startY = qMax(16, (viewport()->height() - totalContentH) / 2);

    // Draw icon centered
    const int iconX = (viewport()->width() - tinted.width()) / 2;
    p.drawPixmap(iconX, startY, tinted);

    // Draw title
    p.setFont(titleFont);
    p.setPen(palette().color(QPalette::WindowText));
    const QRect titleRect(16, startY + tinted.height() + gap1, viewport()->width() - 32, titleH);
    p.drawText(titleRect, Qt::AlignHCenter | Qt::AlignVCenter, title);

    // Draw subtitle
    p.setFont(subtitleFont);
    p.setPen(palette().color(QPalette::PlaceholderText));
    const QRect subtitleRect(16, titleRect.bottom() + gap2, viewport()->width() - 32, subtitleH);
    p.drawText(subtitleRect, Qt::AlignHCenter | Qt::AlignTop, subtitle);
}
