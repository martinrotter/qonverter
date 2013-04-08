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

#include <QColor>

#include "calculatorhighlighter.h"
#include "settings.h"
#include "calculatorinput.h"
#include "defs.h"


TextBlockData::TextBlockData() : QTextBlockUserData() {
}

TextBlockData::~TextBlockData() {
  qDeleteAll(m_parentheses);
}

QVector<ParenthesisInfo*> TextBlockData::getParenthesises() {
  return m_parentheses;
}

void TextBlockData::insert(ParenthesisInfo *info) {
  int i = 0;
  while (i < m_parentheses.size() &&
         info->m_position > m_parentheses.at(i)->m_position) {
    i++;
  }
  m_parentheses.insert(i, info);
}

HighlightingRule CalculatorHighlighter::getRule(const HighlighterRole &type) {
  return m_highlightingRules[type];
}

QColor CalculatorHighlighter::getColorForToken(const HighlighterRole &type) {
  return Settings::value(APP_CFG_CALC, m_ruleKeys[type],
                         m_correctColors[type]).value<QColor>();
}

void CalculatorHighlighter::setColorForToken(const HighlighterRole &type, const QColor &color) {
  Settings::setValue(APP_CFG_CALC, m_ruleKeys[type], color);
}

QList<QColor> CalculatorHighlighter::getDefaultColors() {
  return m_correctColors;
}

QList<QColor> CalculatorHighlighter::generateAdaptiveColors(const QColor &bg,
                                                            const QColor &fg,
                                                            int no_colors) {
  QList<QColor> colors;
  int HUE_BASE = bg.hue() == -1 ? 90 : bg.hue();
  int h, s, v;

  for (int i = 0; i < no_colors; i++) {
    h = int(HUE_BASE + (360.0 / no_colors * i)) % 360;
    s = 240;
    v = int(qMax(bg.value(), fg.value()) * 0.85);

    int M = 35;
    if ((h < bg.hue() + M &&h > bg.hue() - M) ||
        (h < fg.hue() + M &&h > fg.hue() - M)) {
      h = ((bg.hue() + fg.hue()) / (i + 1)) % 360;
      s = ((bg.saturation() + fg.saturation() + 2 * i) / 2) % 256;
      v = ((bg.value() + fg.value() + 2 * i) / 2) % 256;
    }
    colors.append(QColor::fromHsv(h, s, v));
  }

  return colors;
}

void CalculatorHighlighter::initializeColors(const QList<QColor> &colors) {
  // Set colors from adaptive coloring as default ones.
  m_correctColors = generateAdaptiveColors(m_input->palette().color(QPalette::Base),
                                           m_input->palette().color(QPalette::Text),
                                           6);
  m_highlightingRules.clear();
  HighlightingRule rule;

  // Add highlighting rules/colors for each element.
  QTextCharFormat format_numbers;
  format_numbers.setForeground(colors.isEmpty() ?
                                 getColorForToken(NUMBERS) :
                                 colors[(CalculatorHighlighter::HighlighterRole) NUMBERS]);
  rule.m_pattern = QRegExp("[-+]?[0-9]*.?[0-9]+([eE][-+]?[0-9]+)?[i]?");
  rule.m_format = format_numbers;
  m_highlightingRules.append(rule);

  QTextCharFormat format_functions;
  format_functions.setForeground(colors.isEmpty() ?
                                   getColorForToken(FUNCTIONS) :
                                   colors[(CalculatorHighlighter::HighlighterRole) FUNCTIONS]);
  rule.m_pattern = QRegExp("\\b[0-9A-Za-z'_]+(?=\\()");
  rule.m_format = format_functions;
  m_highlightingRules.append(rule);

  QTextCharFormat format_operation;
  format_operation.setForeground(colors.isEmpty() ?
                                   getColorForToken(OPERATORS) :
                                   colors[(CalculatorHighlighter::HighlighterRole) OPERATORS]);
  rule.m_pattern = QRegExp("and|or|xor|==|!=|√|<=|>=|&|<<|>>|\\+|\\-|\\=|\\*|\\/|\\%|\\^|<|>|\\:");
  rule.m_format = format_operation;
  m_highlightingRules.append(rule);

  QTextCharFormat format_braces_b;
  format_braces_b.setBackground(colors.isEmpty() ?
                                  getColorForToken(BRACES_B) :
                                  colors[(CalculatorHighlighter::HighlighterRole) BRACES_B]);
  rule.m_pattern = QRegExp("[(|)]");
  rule.m_format = format_braces_b;
  m_highlightingRules.append(rule);

  QTextCharFormat format_braces_f;
  format_braces_f.setForeground(colors.isEmpty() ?
                                  getColorForToken(BRACES_F) :
                                  colors[(CalculatorHighlighter::HighlighterRole) BRACES_F]);
  rule.m_pattern = QRegExp("[(|)]");
  rule.m_format = format_braces_f;
  m_highlightingRules.append(rule);

  QTextCharFormat format_strings;
  format_strings.setForeground(colors.isEmpty() ?
                                 getColorForToken(STRINGS) :
                                 colors[(CalculatorHighlighter::HighlighterRole) STRINGS]);
  rule.m_pattern = QRegExp("\"[a-zA-Z]*\"");
  rule.m_format = format_strings;
  m_highlightingRules.append(rule);
}


CalculatorHighlighter::CalculatorHighlighter(CalculatorInput *parent)
  : QSyntaxHighlighter(parent->document()), m_input(parent) {

  m_ruleKeys << COLOR_NUMBERS << COLOR_FUNCTIONS << COLOR_OPERATORS <<
                COLOR_BRACES_B << COLOR_BRACES_F << COLOR_STRINGS;

  initializeColors();
}

void CalculatorHighlighter::highlightBlock(const QString &text) {
  foreach (const HighlightingRule &rule, m_highlightingRules) {
    QRegExp expression(rule.m_pattern);
    int index = expression.indexIn(text);
    while (index >= 0) {
      int length = expression.matchedLength();
      setFormat(index, length, rule.m_format);
      index = expression.indexIn(text, index + length);
    }
  }

  TextBlockData *data = new TextBlockData();

  int leftPos = text.indexOf('(');
  while (leftPos != -1) {
    ParenthesisInfo *info = new ParenthesisInfo();
    info->m_character = '(';
    info->m_position = leftPos;

    data->insert(info);
    leftPos = text.indexOf('(', leftPos + 1);
  }

  int rightPos = text.indexOf(')');
  while (rightPos != -1) {
    ParenthesisInfo *info = new ParenthesisInfo();
    info->m_character = ')';
    info->m_position = rightPos;

    data->insert(info);
    rightPos = text.indexOf(')', rightPos + 1);
  }

  setCurrentBlockUserData(data);
  setCurrentBlockState(0);
}
