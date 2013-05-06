#ifndef MARKEDLINEEDIT_H
#define MARKEDLINEEDIT_H

#define MARKED_LINE_EDIT_MARGIN "MarkedLineEdit { margin-right: %1px; }"

#include "lineedit.h"


class MarkedLineEdit : public LineEdit {
    Q_OBJECT

  public:
    enum Status {
      OK,
      ERROR
    };

    explicit MarkedLineEdit(QWidget *parent = 0);
    ~MarkedLineEdit();
    
  public slots:
    void setIcon(Status icon);

    virtual void setEnabled(bool enable);

  protected:
    void resizeEvent(QResizeEvent *event);

  private:
    QIcon m_iconOk;
    QIcon m_iconError;

    QToolButton *m_btnMark;
    bool m_btnMarkEnabled;
};

#endif // MARKEDLINEEDIT_H
