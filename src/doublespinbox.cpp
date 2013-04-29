#include "doublespinbox.h"
#include "lineedit.h"


DoubleSpinBox::DoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent) {
  // Make sure that English locale is used
  // because dot is better for separator than comma is.
  setLocale(QLocale::English);

  // Replace default line edit component with custom one.
  LineEdit *line_edit = new LineEdit();
  line_edit->setMinimumHeight(line_edit->minimumSizeHint().height());
  line_edit->setStyleSheet("LineEdit { background: transparent; }");

  // Clear button is not needed.
  line_edit->setClearButtonEnabled(false);

  setLineEdit(line_edit);
}
