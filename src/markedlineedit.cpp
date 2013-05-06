#include <QToolButton>
#include "markedlineedit.h"


// TODO: Pri kliknuti na fajfku se zobrazi stav
// napriklad jako tooltip nebo balloon tip
// http://qt.gitorious.org/qt/qt/blobs/4.7/src/gui/util/qsystemtrayicon_p.h trida (QBaloonTip)
// pokusit se tu tridu vyseparovat.
// nastavit korektnější šířku pro ikonu
MarkedLineEdit::MarkedLineEdit(QWidget *parent) : LineEdit(parent) {
  m_btnMark = new QToolButton(this);

  int frame_width = frameWidth();
  QSize sz = sizeHint();

  // Prepare icons for valid and invalid state.
  m_iconOk = QIcon::fromTheme("security-high",
                              QIcon(":/graphics/valid.png"));
  m_iconError = QIcon::fromTheme("security-low",
                                 QIcon(":/graphics/invalid.png"));

  // Set the icon to have the same dimensions as is the height of line edit.
  m_btnMark->setIconSize(QSize(sz.height(),
                               sz.height()));

  // Set correct cursor.
  m_btnMark->setCursor(Qt::ArrowCursor);

  // Make sure that mark icon has no border and padding.
  m_btnMark->setStyleSheet("QToolButton { border: none; padding: 0px; }");

  // Make room at the right end of line edit for mark button.
  // Make sure there is extra margin between line edit and mark icon.
  // There is very UNPLEASANT issue in Qt. Default CSS properties of child
  // class override properties of parent class.
  setStyleSheet(QString("%1 %2").arg(QString(MARKED_LINE_EDIT_MARGIN).arg(sz.height() + frame_width),
                                     QString(LINE_EDIT_PADDING).arg(m_btnClear->sizeHint().width() + frameWidth() + 1)));
}

MarkedLineEdit::~MarkedLineEdit() {
  delete m_btnMark;
}

void MarkedLineEdit::resizeEvent(QResizeEvent *event) {
  Q_UNUSED(event);

  QSize sz_clear = m_btnClear->sizeHint();
  QSize sz_mark = m_btnMark->sizeHint();
  QSize sz_lineedit = sizeHint();

  int frame_width = frameWidth();

  // We need to move the clear button more to the left.
  m_btnClear->move(rect().right() - sz_lineedit.height() - sz_clear.width() - frame_width,
                   rect().bottom() - sz_clear.height() + frame_width);

  // Move mark button to the right of line edit.
  m_btnMark->move(rect().right() - sz_lineedit.height(),
                  rect().bottom() - sz_mark.height() + 2 * frame_width);
}

void MarkedLineEdit::setEnabled(bool enable) {
  LineEdit::setEnabled(enable);

  int frame_width = frameWidth();

  if (enable) {
    setStyleSheet(QString("%1 %2").arg(QString(MARKED_LINE_EDIT_MARGIN).arg(sizeHint().height() + frame_width),
                                       QString(LINE_EDIT_PADDING).arg(m_btnClear->sizeHint().width() + frame_width + 1)));
  }
  else {
    setStyleSheet(QString("%1 %2").arg(QString(MARKED_LINE_EDIT_MARGIN).arg(sizeHint().height() + frame_width),
                                       QString(LINE_EDIT_PADDING).arg(0)));
  }

  onTextChanged(text());
}

void MarkedLineEdit::setIcon(Status icon) {
  switch (icon) {
    case OK:
      m_btnMark->setIcon(m_iconOk);
      break;
    case ERROR:
      m_btnMark->setIcon(m_iconError);
      break;
    default:
      break;
  }
}
