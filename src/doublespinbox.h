#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QDoubleSpinBox>


class DoubleSpinBox : public QDoubleSpinBox {
    Q_OBJECT

  public:
    explicit DoubleSpinBox(QWidget *parent = 0);
};

#endif // DOUBLESPINBOX_H
