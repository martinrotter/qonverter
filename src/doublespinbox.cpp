#include "doublespinbox.h"
#include "lineedit.h"


DoubleSpinBox::DoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent) {
  // Make sure that English locale is used
  // because dot is better for separator than comma is.
  setLocale(QLocale::English);

  // TODO: Problem with styling. Solve with QPaletter if
  // other solutions fail.
  LineEdit *line_edit = new LineEdit();
  line_edit->setClearButtonEnabled(false);
  setLineEdit(line_edit);
}
