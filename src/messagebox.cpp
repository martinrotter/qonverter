#include "messagebox.h"


MessageBox::MessageBox() {
}

QMessageBox::StandardButton MessageBox::question(QWidget *parent,
                                                 const QString &title,
                                                 const QString &text,
                                                 const QString &informative_text,
                                                 QMessageBox::StandardButtons buttons,
                                                 QMessageBox::StandardButton defaultButton) {
  QMessageBox msg_box(QMessageBox::Question, title,
                      text, buttons, parent);
  msg_box.setWindowModality(Qt::WindowModal);
  msg_box.setInformativeText(informative_text);

  return (QMessageBox::StandardButton) msg_box.exec();
}

QMessageBox::StandardButton MessageBox::custom(QWidget *parent,
                                               const QString &title,
                                               const QString &text,
                                               QMessageBox::StandardButtons buttons,
                                               QMessageBox::StandardButton defaultButton,
                                               const QPixmap &pixmap) {
  QMessageBox msg_box(QMessageBox::Warning, title,
                      text, buttons, parent);
  msg_box.setWindowModality(Qt::WindowModal);
  msg_box.setIconPixmap(pixmap);

  return (QMessageBox::StandardButton) msg_box.exec();
}

QMessageBox::StandardButton MessageBox::warning(QWidget *parent,
                                                const QString &title,
                                                const QString &text,
                                                QMessageBox::StandardButtons buttons,
                                                QMessageBox::StandardButton defaultButton) {
  QMessageBox msg_box(QMessageBox::Warning, title,
                      text, buttons, parent);
  msg_box.setWindowModality(Qt::WindowModal);

  return (QMessageBox::StandardButton) msg_box.exec();
}


QMessageBox::StandardButton MessageBox::information(QWidget *parent,
                                                    const QString &title,
                                                    const QString &text,
                                                    QMessageBox::StandardButtons buttons,
                                                    QMessageBox::StandardButton defaultButton) {
  QMessageBox msg_box(QMessageBox::Information, title,
                      text, buttons, parent);
  msg_box.setWindowModality(Qt::WindowModal);

  return (QMessageBox::StandardButton) msg_box.exec();
}
