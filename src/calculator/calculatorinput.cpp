/*
    This file is part of Qonverter.

    Qonverter is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Qonverter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Qonverter.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2012 - 2013 Martin Rotter
*/

#include <QCompleter>
#include <QScrollBar>

#if defined(Q_OS_WIN)
#include <windows.h>
#include <winuser.h>
#endif

#include "calculatorinput.h"
#include "calculatorhighlighter.h"
#include "formcalculator.h"
#include "settings.h"
#include "defs.h"
#include "calculatorwrapper.h"
#include "constantsmodel.h"
#include "constantsview.h"


CalculatorInput::CalculatorInput(QWidget *parent) : QPlainTextEdit(parent) {
  setTabChangesFocus(true);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setContextMenuPolicy(Qt::NoContextMenu);

  connect(this, &CalculatorInput::cursorPositionChanged, this, &CalculatorInput::matchPar);

  // Initialize completer
  m_completer = new QCompleter(this);
  m_completer->setWidget(this);
  m_completer->setCompletionMode(QCompleter::PopupCompletion);
  m_completer->setCaseSensitivity(Qt::CaseSensitive);

  // Connect completer.
  connect(m_completer,
          static_cast<void (QCompleter::*)(const QString &text)>(&QCompleter::activated),
          this, &CalculatorInput::insertCompletion);

  // Make sure that model is loaded after calculator is initialized.
  // This is needed because calculator initialization is done
  // in separated thread.
  connect(CalculatorWrapper::getInstance().getCalculator(), &Calculator::initialized,
          this, &CalculatorInput::setCompleterModel);
}

void CalculatorInput::setCompleterModel() {
  m_completer->setModel((QAbstractItemModel*) CalculatorWrapper::getInstance().getCalculator()->getConstantsModel());
  m_completer->setCompletionColumn(0);
  m_completer->setCompletionRole(Qt::DisplayRole);

  // Create view for completer and assign it to completer.
  QTreeView *tree_view = new QTreeView();
  m_completer->setPopup(tree_view);

  // Setup look & feel of the view.
  tree_view->setRootIsDecorated(false);
  tree_view->setAnimated(true);
  tree_view->setAllColumnsShowFocus(true);
  tree_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  tree_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  tree_view->header()->hide();

  // Hide some columns (these are the "value" and "type" of memory places.
  // TODO: Perhaps display "value" or "type" of memory place too.
  tree_view->header()->setSectionHidden(1, true);
  tree_view->header()->setSectionHidden(2, true);

  // Tweak width of displayed columns.
  tree_view->header()->setSectionResizeMode(0, QHeaderView::Fixed);
  tree_view->header()->setSectionResizeMode(1, QHeaderView::Stretch);
}

CalculatorInput::~CalculatorInput() {
  delete m_completer;
}

void CalculatorInput::insertCompletion(const QString& completion) {
  // Calculate cursor position for inserting text.
  QTextCursor tc = textCursor();
  int extra = completion.length() - m_completer->completionPrefix().length();

  // Insert text at position and set cursor to it.
  tc.movePosition(QTextCursor::Left);
  tc.movePosition(QTextCursor::EndOfWord);
  tc.insertText(completion.right(extra));

  // Insert additional "()" if function is the actual completion
  // and move anchor one piece left.
  if (CalculatorWrapper::getInstance().getCalculator()->isFunDefined(completion)) {
    tc.insertText("()");
    tc.movePosition(QTextCursor::Left);
  }

  setTextCursor(tc);

  // Emit text changed.
  emit textEdited(toPlainText());
}

QString CalculatorInput::textUnderCursor() const {
  QTextCursor tc = textCursor();
  tc.select(QTextCursor::WordUnderCursor);
  return tc.selectedText();
}

void CalculatorInput::initialize() {
  // Gets current font from settings and applies it to input.
  setFont(Settings::value(APP_CFG_CALC,
                          "font_input",
                          QFont("Sans", 20)).value<QFont>());
  // Set maximum number of visible items for completer.
  m_completer->setMaxVisibleItems(Settings::value(APP_CFG_CALC,
                                                  "completer_max_items",
                                                  7).toInt());
}

bool CalculatorInput::matchLeftPar(QTextBlock current_block, int index, int num_right_par, QChar lft, QChar rgt) {
  TextBlockData *data = static_cast<TextBlockData*>(current_block.userData());
  QVector<ParenthesisInfo*> infos = data->getParenthesises();
  int docPos = current_block.position();

  // Match in same line?
  for (; index < infos.size(); index++) {
    ParenthesisInfo *info = infos.at(index);

    if (info->m_character == lft) {
      num_right_par++;
      continue;
    }

    if (info->m_character == rgt && num_right_par == 0) {
      createParSelection(docPos + info->m_position);
      return true;
    }
    else {
      num_right_par--;
    }
  }

  // No match yet? Then try next block
  current_block = current_block.next();
  if (current_block.isValid()) {
    return matchLeftPar(current_block, 0, num_right_par, lft, rgt);
  }

  // No match at all
  return false;
}

bool CalculatorInput::matchRightPar(QTextBlock current_block, int index, int num_left_par, QChar lft, QChar rgt) {
  TextBlockData *data = static_cast<TextBlockData*>(current_block.userData());
  QVector<ParenthesisInfo*> infos = data->getParenthesises();
  int docPos = current_block.position();

  // Match in same line?
  for (int j = index; j >= 0; j--) {
    ParenthesisInfo *info = infos.at(j);

    if (info->m_character == rgt) {
      num_left_par++;
      continue;
    }

    if (info->m_character == lft && num_left_par == 0) {
      createParSelection(docPos + info->m_position);
      return true;
    }
    else {
      num_left_par--;
    }
  }

  // No match yet? Then try previous block
  current_block = current_block.previous();
  if (current_block.isValid()) {
    // Recalculate correct index first
    TextBlockData *data = static_cast<TextBlockData*>(current_block.userData());
    QVector<ParenthesisInfo*> infos = data->getParenthesises();

    return matchRightPar(current_block, infos.size() - 1, num_left_par, lft, rgt);
  }

  // No match at all
  return false;
}

// Inspired by http://qt-project.org/doc/qt-4.8/tools-customcompleter.html.
void CalculatorInput::keyPressEvent(QKeyEvent *e) {
  switch (e->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
      if (!m_completer->popup()->isVisible()) {
        emit submitted(toPlainText());
        return;
      }
    case Qt::Key_Escape:
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
      e->ignore();
      return;
#if defined(Q_OS_WIN)
    case Qt::Key_Comma:
      if (e->nativeVirtualKey() == VK_DECIMAL) {
        insertPlainText(".");
        e->ignore();
        return;
      }
#endif
    default:
      e->accept();
      break;
  }

  // TODO: Find out the cause of "missing" newly created items in the popup w/o current hack.
  // THIS IS VERY UGLY HACK!!!!!!!!!!!!!!!!!!!
  m_completer->setModel(CalculatorWrapper::getInstance().getCalculator()->getConstantsModel());

  bool trigger_hortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space);
  if (!trigger_hortcut) {
    QPlainTextEdit::keyPressEvent(e);
  }

  if (!e->text().isEmpty()) {
    emit textEdited(toPlainText());
  }

  bool ctrl_shift_pressed = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
  if (ctrl_shift_pressed && e->text().isEmpty()) {
    return;
  }

  static QString word_delimiters("~!@#$%^&*()+{}|:\"<>?,./;'[]\\-=");

  QRect cr = cursorRect();

  // Move the viewport of popup treeview little down
  // and make it horizontally fit in the parent textbox.
  cr.setLeft(0);
  cr.setWidth(width());
  cr.translate(0, 3);

  QString completion_prefix = textUnderCursor();

  bool has_modifier = (e->modifiers() != Qt::NoModifier) &&
                      (ctrl_shift_pressed == false);

  if (!trigger_hortcut &&
      (has_modifier || e->text().isEmpty() || completion_prefix.length() < 1 ||
       word_delimiters.contains(e->text().right(1)))) {
    m_completer->popup()->hide();
    return;
  }

  if (completion_prefix != m_completer->completionPrefix()) {
    m_completer->setCompletionPrefix(completion_prefix);
    //m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
  }

  // Display auto-completion panel.
  m_completer->complete(cr);
}

void CalculatorInput::matchPar() {
  QList<QTextEdit::ExtraSelection> selections;
  QTextBlock textBlock = textCursor().block();
  TextBlockData *data = static_cast<TextBlockData *>(textBlock.userData());

  setExtraSelections(selections);

  if (data != nullptr) {
    QVector<ParenthesisInfo *> infos = data->getParenthesises();
    int pos = textCursor().block().position();

    for (int i = 0; i < infos.size(); i++) {
      ParenthesisInfo *info = infos.at(i);
      int curPos = textCursor().position() - textBlock.position();

      // Clicked on a left parenthesis?
      if (info->m_position == curPos - 1 && info->m_character == '(') {
        if (matchLeftPar(textBlock, i + 1, 0)) {
          createParSelection(pos + info->m_position);
        }
      }

      // Clicked on a right parenthesis?
      if (info->m_position == curPos - 1 && info->m_character == ')') {
        if (matchRightPar(textBlock, i - 1, 0)) {
          createParSelection(pos + info->m_position);
        }
      }
    }
  }
}

void CalculatorInput::createParSelection(int pos) {
  QList<QTextEdit::ExtraSelection> selections = extraSelections();
  QTextEdit::ExtraSelection selection;
  QTextCharFormat format = selection.format;

  format.setBackground(FormCalculator::getInstance()->getHighlighter()->getRule(CalculatorHighlighter::BRACES_B).m_format.background());
  selection.format = format;

  QTextCursor cursor = textCursor();
  cursor.setPosition(pos);
  cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

  selection.cursor = cursor;
  selections.append(selection);
  setExtraSelections(selections);
}
