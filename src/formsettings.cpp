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

#include <QDir>
#include <QTranslator>
#include <QProcess>
#include <QFontDialog>
#include <QColorDialog>
#include <QStyleFactory>

#include "messagebox.h"
#include "settings.h"
#include "formsettings.h"
#include "formmain.h"
#include "defs.h"
#include "uifactory.h"
#include "calculatorhighlighter.h"


FormSettings::FormSettings(QWidget *parent) : QDialog(parent), m_ui(new Ui::FormSettings) {
  m_ui->setupUi(this);

  // Make sure application is not closed due to this window behavior.
  setAttribute(Qt::WA_QuitOnClose, false);

  // Set flags.
  setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog);

  // Prepare UI.
  setFixedSize(size());
  setWindowIcon(QIcon::fromTheme("preferences-system",
                                 QIcon(":/graphics/settings_all.png")));
  m_ui->m_txtCalcPreview->setCompleterModel();

  // Prepare sections panel.
  m_ui->m_stackedSettings->setCurrentIndex(0);
  m_ui->m_listSettings->item(0)->setIcon(QIcon::fromTheme("applications-system",
                                                          QIcon(":/graphics/settings_general.png")));
  m_ui->m_listSettings->item(1)->setIcon(QIcon::fromTheme("applications-graphics",
                                                          QIcon(":/graphics/settings_ui.png")));
  m_ui->m_listSettings->item(2)->setIcon(QIcon::fromTheme("applications-internet",
                                                          QIcon(":/graphics/settings_language.png")));
  m_ui->m_listSettings->item(3)->setIcon(QIcon::fromTheme("applications-science",
                                                          QIcon(":/graphics/settings_calculator.png")));

  // Prepare language list.
  m_ui->m_treeLangs->setHeaderHidden(false);
  m_ui->m_treeLangs->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  m_ui->m_treeLangs->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  m_ui->m_treeLangs->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
  m_ui->m_treeLangs->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
  m_ui->m_treeLangs->header()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
  m_ui->m_treeLangs->header()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

  // Save settings when 'OK' button is used.
  connect(this, &FormSettings::accepted, this, &FormSettings::saveSettings);

  // Display change font dialog and reload settings.
  connect(m_ui->m_btnCalcFont, &QPushButton::clicked, [=] () {
    m_tempSettings.m_calcFont = QFontDialog::getFont(nullptr, m_tempSettings.m_calcFont,
                                                     this, tr("Change Calculator Input Font"));
    loadCalculator();
  });

  // Display change font dialog and reload settings.
  connect(m_ui->m_btnOtfFont, &QPushButton::clicked, [=] () {
    m_tempSettings.m_otfFont = QFontDialog::getFont(nullptr, m_tempSettings.m_otfFont,
                                                    this, tr("Change On-The-Fly Mode Font"));
    loadCalculator();
  });

  // Setup special buttons for changing colors.
  QList<QPushButton*> color_buttons;
  color_buttons << m_ui->m_btnCalcNumbers << m_ui->m_btnCalcFunctions << m_ui->m_btnCalcOperators <<
                   m_ui->m_btnCalcParB << m_ui->m_btnCalcParF << m_ui->m_btnCalcStrings;

  // Make sure that each button allows user to change color for each pattern.
  for (int i = 0; i < color_buttons.size(); i++) {
    connect(color_buttons[i], &QPushButton::clicked, [=] () {
      QColor new_color = QColorDialog::getColor(m_tempSettings.m_calcColors[i],
                                                this,
                                                tr("Select New Color"));

      if (new_color.isValid()) {
        m_tempSettings.m_calcColors[i] = new_color;
        loadCalculator();
      }
    });
  }

  // Resets color to default ones.
  connect(m_ui->m_btnCalcReset, &QPushButton::clicked, [=] () {
    for (int i = 0; i < m_tempSettings.m_calcColors.size(); i++) {
      m_tempSettings.m_calcColors = m_highlighter->getDefaultColors();
    }

    loadCalculator();
  });

  // Some gui tweaking.
  connect(m_ui->m_radioUseTray, &QRadioButton::toggled,
          m_ui->m_cmbTrayAction, &QComboBox::setEnabled);

  connect(m_ui->m_checkOnTheFly, &QCheckBox::stateChanged, [=] (int state) {
    m_ui->m_spinFlyTime->setEnabled(state == Qt::Checked);
    m_ui->m_lblFlyTime->setEnabled(state == Qt::Checked);
  });

  // Make necessary creations.
  m_highlighter = new CalculatorHighlighter(m_ui->m_txtCalcPreview);

  // Make sure fonts are loaded from settings just when dialog is created.
  m_tempSettings.m_calcFont = Settings::value(APP_CFG_CALC, "font_input",
                                              qApp->font()).value<QFont>();
  m_tempSettings.m_otfFont = Settings::value(APP_CFG_CALC, "on_the_fly_font",
                                             qApp->font()).value<QFont>();

  // Load initial colors into temporary settings
  int count_of_colors = m_highlighter->getDefaultColors().size();
  for (int i = 0; i < count_of_colors; i++) {
    m_tempSettings.m_calcColors << m_highlighter->getColorForToken((CalculatorHighlighter::HighlighterRole) i);
  }

  // Fill dialog with needed data.
  loadGeneral();
  loadInterface();
  loadLanguages();
  loadCalculator();
}

FormSettings::~FormSettings() {
  delete m_ui;
  delete m_highlighter;
}

void FormSettings::saveSettings() {
  // Save all categories.
  saveGeneral();
  saveInterface();
  saveCalculator();
  saveLanguages();

  // Make sure that settings is synced.
  Settings::checkSettings();
}

void FormSettings::loadLanguages() {
  QDir file_dir(APP_LANG_PATH);
  QFileInfoList file_list = file_dir.entryInfoList(QStringList() << "qonverter_*.qm",
                                                   QDir::Files | QDir::NoDotAndDotDot,
                                                   QDir::Name);

  foreach (QFileInfo file, file_list) {
    QTranslator translator;
    translator.load(file.absoluteFilePath());

    QString lang_name = translator.translate("QObject", "LANG_NAME");
    QString lang_abbrev = translator.translate("QObject", "LANG_ABBREV");
    QString lang_version = translator.translate("QObject", "LANG_VERSION");
    QString lang_author = translator.translate("QObject", "LANG_AUTHOR");
    QString lang_email = translator.translate("QObject", "LANG_EMAIL");
    QString lang_website = translator.translate("QObject", "LANG_WEBSITE");

    if (lang_name.isEmpty() ||
        lang_abbrev.isEmpty() ||
        lang_version.isEmpty()) {
      continue;
    }
    else {
      QTreeWidgetItem *item = new QTreeWidgetItem();
      item->setIcon(0, QIcon(QString(":/graphics/flags/%1.png").arg(lang_abbrev)));
      item->setText(0, lang_name);
      item->setText(1, lang_abbrev);
      item->setText(2, lang_version);
      item->setText(3, lang_author);
      item->setText(4, lang_email);
      item->setText(5, lang_website);

      m_ui->m_treeLangs->addTopLevelItem(item);
    }
    qDebug("Adding translation %s to dialog.", qPrintable(file.absoluteFilePath()));
  }

  // Now mark currently selected language.
  QList<QTreeWidgetItem*> list_langs = m_ui->m_treeLangs->findItems(Settings::value(APP_CFG_LANG,
                                                                                    "language",
                                                                                    "qonverter_en.qm").toString().section(QRegExp("[_|.]+"),
                                                                                                                          1,
                                                                                                                          1),
                                                                    Qt::MatchExactly,
                                                                    1);
  if (!list_langs.isEmpty()) {
    m_ui->m_treeLangs->setCurrentItem(list_langs[0]);
  }
  else {
    QList<QTreeWidgetItem*> list_def = m_ui->m_treeLangs->findItems("en",
                                                                    Qt::MatchExactly,
                                                                    1);
    if (!list_def.isEmpty()) {
      m_ui->m_treeLangs->setCurrentItem(list_def[0]);
    }
  }
}

void FormSettings::saveLanguages() {
  QString actual_lang = Settings::value(APP_CFG_LANG,
                                        "language",
                                        "qonverter_en.qm").toString();

  if (m_ui->m_treeLangs->selectedItems().isEmpty()) {
    return;
  }

  QString new_lang = QString("qonverter_%1.qm").arg(m_ui->m_treeLangs->currentItem()->text(1));

  if (new_lang != actual_lang) {
    Settings::setValue(APP_CFG_LANG, "language", new_lang);

    if (MessageBox::question(this, tr("Language Changed"),
                             tr("Language of Qonverter was changed. Note that changes will take effect on next Qonverter start."),
                             tr("Do you want to restart now?"),
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes) ==
        QMessageBox::Yes) {
      if (!QProcess::startDetached(qApp->applicationFilePath())) {
        MessageBox::warning(this,
                            tr("Problem with Qonverter Restart"),
                            tr("Qonverter couldn't be restarted, please restart it manually for changes to take effect."));
      }
      else {
        qApp->quit();
      }
    }
  }
}

void FormSettings::saveColor(const CalculatorHighlighter::HighlighterRole &role, const QColor &color) {
  FormCalculator::getInstance()->getHighlighter()->setColorForToken(role, color);
}

void FormSettings::setButtonColor(QPushButton *button, const QColor &color) {
  button->setAutoFillBackground(true);
  button->setStyleSheet(QString("QPushButton { background-color: rgb(%1,%2,%3); }")
                        .arg(QString::number(color.red()),
                             QString::number(color.green()),
                             QString::number(color.blue())));
}

void FormSettings::loadCalculator() {
  // Load colors into buttons and reload highlighter.
  m_highlighter->initializeColors(m_tempSettings.m_calcColors);

  m_ui->m_txtCalcPreview->setFont(m_tempSettings.m_calcFont);

  // Quite hacky but working.
  m_ui->m_txtCalcPreview->setPlainText(m_ui->m_txtCalcPreview->toPlainText());

  setButtonColor(m_ui->m_btnCalcNumbers,
                 m_tempSettings.m_calcColors[CalculatorHighlighter::NUMBERS]);
  setButtonColor(m_ui->m_btnCalcFunctions,
                 m_tempSettings.m_calcColors[CalculatorHighlighter::FUNCTIONS]);
  setButtonColor(m_ui->m_btnCalcOperators,
                 m_tempSettings.m_calcColors[CalculatorHighlighter::OPERATORS]);
  setButtonColor(m_ui->m_btnCalcParF,
                 m_tempSettings.m_calcColors[CalculatorHighlighter::BRACES_F]);
  setButtonColor(m_ui->m_btnCalcParB,
                 m_tempSettings.m_calcColors[CalculatorHighlighter::BRACES_B]);
  setButtonColor(m_ui->m_btnCalcStrings,
                 m_tempSettings.m_calcColors[CalculatorHighlighter::STRINGS]);

  // Other calculator setting.
  m_ui->m_checkOnTheFly->setChecked(Settings::value(APP_CFG_CALC,
                                                    "on-the-fly",
                                                    true).toBool());
  m_ui->m_checkVars->setChecked(Settings::value(APP_CFG_CALC,
                                                "implicit_value_creation",
                                                false).toBool());
  m_ui->m_spinErrorTime->setValue(Settings::value(APP_CFG_CALC,
                                                  "length_error",
                                                  3000).toInt());
  m_ui->m_spinFlyTime->setValue(Settings::value(APP_CFG_CALC,
                                                "length_fly",
                                                1500).toInt());
  m_ui->m_checkFinish->setChecked(Settings::value(APP_CFG_CALC,
                                                  "select_result",
                                                  true).toBool());

  // Auto-completion.
  m_ui->m_spinCompleterMaxItems->setValue(Settings::value(APP_CFG_CALC,
                                                          "completer_max_items",
                                                          7).toInt());
}

void FormSettings::saveCalculator() {
  Settings::setValue(APP_CFG_CALC, "font_input", m_tempSettings.m_calcFont);
  Settings::setValue(APP_CFG_CALC, "on_the_fly_font", m_tempSettings.m_otfFont);

  for (int i = 0; i < m_tempSettings.m_calcColors.size(); i++) {
    FormCalculator::getInstance()->getHighlighter()->setColorForToken((CalculatorHighlighter::HighlighterRole) i,
                                                                      m_tempSettings.m_calcColors[i]);
  }

  Settings::setValue(APP_CFG_CALC, "on-the-fly", m_ui->m_checkOnTheFly->isChecked());
  Settings::setValue(APP_CFG_CALC, "implicit_value_creation",
                     m_ui->m_checkVars->isChecked());
  Settings::setValue(APP_CFG_CALC, "length_error", m_ui->m_spinErrorTime->value());
  Settings::setValue(APP_CFG_CALC, "length_fly", m_ui->m_spinFlyTime->value());
  Settings::setValue(APP_CFG_CALC, "select_result", m_ui->m_checkFinish->isChecked());

  // Auto-completion.
  Settings::setValue(APP_CFG_CALC, "completer_max_items",
                     m_ui->m_spinCompleterMaxItems->value());
}

void FormSettings::loadInterface() {
  // Load styles.
  m_ui->m_listStyles->addItems(QStyleFactory::keys());
  QList<QListWidgetItem*> list_of_styles = m_ui->m_listStyles->findItems(Settings::value(APP_CFG_GUI,
                                                                                         "style",
                                                                                         qApp->style()->objectName()).toString(),
                                                                         Qt::MatchFixedString);

  if (!list_of_styles.isEmpty()) {
    m_ui->m_listStyles->setCurrentItem(list_of_styles[0]);
  }

  // Load skins.
  QFileInfoList skin_folders = QDir(APP_SKIN_PATH).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks,
                                                                 QDir::Name);

  foreach (QFileInfo folder, skin_folders) {
    QFileInfoList contained_skin_files = QDir(folder.absoluteFilePath()).entryInfoList(QStringList() << "*.qss");

    foreach (QFileInfo skin_file, contained_skin_files) {
      QListWidgetItem *skin_item = new QListWidgetItem(m_ui->m_listSkins);
      skin_item->setToolTip(skin_file.absoluteFilePath());
      skin_item->setText(folder.fileName() + "/" + skin_file.fileName());

      m_ui->m_listSkins->addItem(skin_item);
    }
  }

  QList<QListWidgetItem*> list_of_skins = m_ui->m_listSkins->findItems(Settings::value(APP_CFG_GUI,
                                                                                       "skin",
                                                                                       "base/plain.qss").toString(),
                                                                       Qt::MatchFixedString);

  if (!list_of_skins.isEmpty()) {
    m_ui->m_listSkins->setCurrentItem(list_of_skins[0]);
  }

  // Load tray icon state
  m_ui->m_radioUseTray->setChecked(Settings::value(APP_CFG_GUI,
                                                   "tray_icon_enabled",
                                                   false).toBool());
  m_ui->m_cmbTrayAction->setCurrentIndex(Settings::value(APP_CFG_GUI,
                                                         "tray_icon_action",
                                                         0).toInt());
}

void FormSettings::saveInterface() {
  if (!m_ui->m_listStyles->selectedItems().isEmpty()) {
    Settings::setValue(APP_CFG_GUI, "style", m_ui->m_listStyles->currentItem()->text());
  }

  if (!m_ui->m_listSkins->selectedItems().isEmpty()) {
    Settings::setValue(APP_CFG_GUI, "skin", m_ui->m_listSkins->currentItem()->text());
  }

  // Load tray icon state
  Settings::setValue(APP_CFG_GUI,
                     "tray_icon_enabled",
                     m_ui->m_radioUseTray->isChecked());
  Settings::setValue(APP_CFG_GUI,
                     "tray_icon_action",
                     m_ui->m_cmbTrayAction->currentIndex());
}

void FormSettings::loadGeneral() {
#ifdef Q_OS_WIN
  QSettings sett("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
  m_ui->m_checkStartup->setChecked((sett.value(APP_LOW_NAME, "").toString().replace('\\', '/') == QApplication::applicationFilePath()));
#else
  m_ui->m_checkStartup->setVisible(false);
#endif
  m_ui->m_listMode->setCurrentRow(Settings::value(APP_CFG_GEN,
                                                  "start_mode", 0).toInt());
  m_ui->m_checkLaunchHidden->setChecked(Settings::value(APP_CFG_GEN,
                                                        "start_hidden",
                                                        false).toBool());
}

void FormSettings::saveGeneral() {
#ifdef Q_OS_WIN
  QSettings sett("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
  if (m_ui->m_checkStartup->isChecked()) {
    sett.setValue(APP_LOW_NAME, QApplication::applicationFilePath().replace('/', '\\'));
  }
  else {
    sett.remove(APP_LOW_NAME);
  }
#endif
  if (!m_ui->m_listMode->selectedItems().isEmpty()) {
    Settings::setValue(APP_CFG_GEN,
                       "start_mode", m_ui->m_listMode->currentRow());
  }
  Settings::setValue(APP_CFG_GEN,
                     "start_hidden",
                     m_ui->m_checkLaunchHidden->isChecked());
}
