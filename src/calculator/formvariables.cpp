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

#include <QPointer>
#include <QModelIndex>
#include <QKeyEvent>

#include "formvariables.h"
#include "formeditvariable.h"
#include "calculatorwrapper.h"
#include "calculator.h"
#include "constantsmodel.h"
#include "constantsfiltermodel.h"


FormVariables *FormVariables::s_instance;

FormVariables::FormVariables(QWidget *parent) : QDialog(parent), m_ui(new Ui::FormVariables) {
  m_ui->setupUi(this);

  // Set flags.
  setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog);
  setWindowIcon(QIcon::fromTheme("document-properties",
                                 QIcon(":/graphics/variables.png")));

  // Needed because classical singleton fails due to UI files behavior.
  s_instance = this;

  // Store calculator model and filter model, this could spare function calls.
  m_filterModel = (ConstantsFilterModel*) m_ui->m_treeVarsConsts->model();
  m_model = (ConstantsModel*) m_filterModel->sourceModel();

  connect(m_ui->m_treeVarsConsts, &ConstantsView::currentItemChanged,
          this, &FormVariables::enableVariableButtons);

  connect(m_ui->m_btnAddVariable, &QPushButton::clicked,
          this, &FormVariables::addVariable);

  connect(m_ui->m_treeVarsConsts, &ConstantsView::doubleClicked,
          m_ui->m_btnEditVariable, &QPushButton::click);

  connect(m_ui->m_btnEditVariable, &QPushButton::clicked,
          this, &FormVariables::editVariable);

  connect(m_ui->m_btnDeleteVariable, &QPushButton::clicked,
          this, &FormVariables::deleteVariable);

  // Make sure that tree view is reloaded each time the selective combo box
  // index is changed.
  connect(m_ui->m_cmbSelectType,
          static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
          [=] (int index) {
    m_filterModel->setFilterMemoryPlaceType(static_cast<ConstantsFilterModel::FilterMemoryPlaceType>(index));
    enableVariableButtons(m_ui->m_treeVarsConsts->currentIndex());
  });

  connect(m_ui->m_cmbSelectType,
          static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
          m_ui->m_txtFilter, &LineEdit::clear);

  connect(m_ui->m_txtFilter, &LineEdit::textChanged,
          m_filterModel,
          static_cast<void (ConstantsFilterModel::*) (const QString &text)>(&ConstantsFilterModel::setFilterRegExp));
}

void FormVariables::enableVariableButtons(const QModelIndex &current) {
  QModelIndex source_index = m_filterModel->mapToSource(current);
  MemoryPlace::Type type = static_cast<MemoryPlace::Type>(m_model->data(m_model->index(source_index.row(),
                                                                                       (int) ConstantsModel::ENTITY_TYPE),
                                                                        Qt::UserRole).toInt());

  m_ui->m_btnEditVariable->setEnabled(type != MemoryPlace::CONSTANT && type != MemoryPlace::FUNCTION);
  m_ui->m_btnDeleteVariable->setEnabled(type != MemoryPlace::CONSTANT && type != MemoryPlace::FUNCTION && type != MemoryPlace::SPECIAL_VARIABLE);
}

FormVariables::~FormVariables() {
  delete m_ui;
}

FormVariables *FormVariables::getInstance() {
  return s_instance;
}

void FormVariables::addVariable() {
  FormEditVariable(this).execAdd();

  // Reset the view.
  m_ui->m_treeVarsConsts->reset();
}

void FormVariables::editVariable() {
  QModelIndex current_idx = m_filterModel->mapToSource(m_ui->m_treeVarsConsts->currentIndex());

  if (current_idx.isValid() == false) {
    return;
  }

  QString name = m_model->data(m_model->index(current_idx.row(), (int) ConstantsModel::NAME),
                               Qt::DisplayRole).toString();
  QString value = m_model->data(m_model->index(current_idx.row(), (int) ConstantsModel::VALUE),
                                Qt::DisplayRole).toString();
  QString type = m_model->data(m_model->index(current_idx.row(), (int) ConstantsModel::VALUE_TYPE),
                               Qt::DisplayRole).toString();
  QString desc = m_model->data(m_model->index(current_idx.row(), (int) ConstantsModel::DESCRIPTION),
                               Qt::UserRole).toString();
  MemoryPlace::Type var_type = static_cast<MemoryPlace::Type>(m_model->data(m_model->index(current_idx.row(),
                                                                                           (int) ConstantsModel::ENTITY_TYPE),
                                                                            Qt::UserRole).toInt());

  FormEditVariable(this).execEdit(name, value, desc, type, var_type);
}

void FormVariables::keyPressEvent(QKeyEvent *e) {
  if (e->key() == Qt::Key_Delete) {
    m_ui->m_btnDeleteVariable->click();
  }
  QDialog::keyPressEvent(e);
}

void FormVariables::deleteVariable() {
  QModelIndex current_idx = m_filterModel->mapToSource(m_ui->m_treeVarsConsts->currentIndex());

  CalculatorWrapper::getInstance().getCalculator()->removeMemoryPlace(m_model->data(m_model->index(current_idx.row()),
                                                                                    Qt::DisplayRole).toString());
  m_ui->m_treeVarsConsts->reset();
}
