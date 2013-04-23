#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QMessageBox>


// Static methods in this class wrap QMessageBox.
// Reason is that that static methods of QMessageBox
// construct application-modal dialogs but i want
// window-modal dialogs.
class MessageBox {
  private:
    MessageBox();

  public:
    // Enhanced, informative text is now part of the method
    // signature, thus, there is no reason to contruct
    // QMessageBox instances manually via constructor.
    static QMessageBox::StandardButton question(QWidget * parent,
                                                const QString &title,
                                                const QString &text,
                                                const QString &informative_text,
                                                QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    static QMessageBox::StandardButton custom(QWidget * parent,
                                              const QString &title,
                                              const QString &text,
                                              QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                              QMessageBox::StandardButton defaultButton = QMessageBox::NoButton,
                                              const QPixmap &pixmap = QPixmap());


    static QMessageBox::StandardButton warning(QWidget * parent,
                                               const QString &title,
                                               const QString &text,
                                               QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                               QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    static QMessageBox::StandardButton information(QWidget * parent,
                                                   const QString &title,
                                                   const QString &text,
                                                   QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                   QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
};

#endif // MESSAGEBOX_H
