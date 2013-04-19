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

#include <QThread>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>

#include <sstream>
#include <iomanip>
#include <limits>

#include "calculator.h"
#include "extensions.h"
#include "settings.h"
#include "defs.h"
#include "database.h"
#include "constantsmodel.h"
#include "mpFuncNonCmplx.h"


Calculator::Calculator(QObject *parent) : QObject(parent) {
  // Register custom types for passing via signal/slot mechanism.
  qRegisterMetaType<Value>("Value");
  qRegisterMetaType<Calculator::CallerFunction>("Calculator::CallerFunction");

  // Set proper object name.
  setObjectName("Calculator");
}

Calculator::~Calculator() {
  // Delete needed properties.
  qDebug("Deleting calculator.");
  qDeleteAll(m_memoryPlaces);
  delete m_parser;
  delete m_constantsModel;
}

void Calculator::loadStoredMemoryPlaces() {
  // Create database connection for storing variables and so on.
  QSqlDatabase database = Database::addDatabaseConnection(objectName());

  // Create query object and string.
  QSqlQuery query_obj(database);
  QString query_load_variables = "SELECT name, value, desc FROM q_variables";
  QString var_name, var_value, var_desc;

  // Execute the query.
  query_obj.exec(query_load_variables);

  int name_col = query_obj.record().indexOf("name");
  int value_col = query_obj.record().indexOf("value");
  int desc_col = query_obj.record().indexOf("desc");
  Value value_of_variable;

  // Iterate over the variables.
  while (query_obj.next() == true) {
    // Fetch variable data from database.
    var_name = query_obj.value(name_col).toString();
    var_value = query_obj.value(value_col).toString();
    var_desc = query_obj.value(desc_col).toString();

    // Variable has empty content, this is probably the best
    // way to encode VOID variables.
    if (var_value.isEmpty()) {
      // Construct new VOID value.
      value_of_variable = Value();
      qDebug("Constructing new VOID value for variable '%s'.",
             qPrintable(var_name));
    }
    // Variable contains specific value.
    else {
      try {
        // Try to re-establish the value.
        value_of_variable = calculateExpressionSynchronously(INTERNAL,
                                                             var_value);
      }
      catch (...) {
        // Value couldn't be re-established.
        // Skip this variable.
        qWarning("Variable '%s' was not re-established. It is not well-formed.",
                 qPrintable(var_name));
        continue;
      }
    }

    // Try to add variable with reconstructed value.
    if (!addMemoryPlace(var_name, var_desc,
                        MemoryPlace::EXPLICIT_VARIABLE, value_of_variable)) {
      qWarning("Variable '%s' could not be added. Value was reconstructed but variable name is not allowed.",
               qPrintable(var_name));
    }
  }

  // Remove database connection.
  Database::removeDatabaseConnection(objectName());
}

void Calculator::loadMemoryPlaces() {
  // Load built-in constants.
  loadConstants();

  // Create initial "ansx", "ans" variables.
  changeAns(Value(0));

  // Load variables stored in SQLite database.
  loadStoredMemoryPlaces();

  // Create "m" variable.
  // This fails (returns false) if "m" was added from database.
  addMemoryPlace("m", tr("predefined memory"), MemoryPlace::SPECIAL_VARIABLE, Value(0));
}

void Calculator::consolidateMemoryPlaces() {
  var_maptype vmap = m_parser->GetVar();
  QString variable_name;

  // Go through all defined variables.
  for (var_maptype::iterator item = vmap.begin(); item!=vmap.end(); ++item) {
    Variable &var = (Variable&) *(item->second);

    variable_name = QString::fromStdWString(item->first);

    if (m_memoryPlaces.contains(variable_name) == false) {
      // We found variable which exists in calculator engine,
      // but is not in external calculator list, ergo, this variable was
      // implicitly created during calculator engine lifetime.
      m_memoryPlaces.insert(variable_name, new MemoryPlace(variable_name));
      m_memoryPlaces[variable_name]->m_value = (Value*) var.GetPtr();
      m_memoryPlaces[variable_name]->m_variable = &var;
      m_memoryPlaces[variable_name]->m_type = MemoryPlace::IMPLICIT_VARIABLE;
    }
  }
}

void Calculator::saveMemoryPlaces() {
  // Create database connection for storing variables and so on.
  QSqlDatabase database = Database::addDatabaseConnection(objectName());

  // Create query and query strings.
  QSqlQuery query_obj(database);
  QString query_clear_table = "DELETE FROM q_variables";
  QString query_add_variable = "INSERT INTO q_variables VALUES('%1', '%2', '%3');";

  // Clear table contents.
  query_obj.exec(query_clear_table);

  // Start the transaction.
  query_obj.exec("BEGIN TRANSACTION");

  // Qo through variables.
  // Special variables have application-session scope and are not saved,
  // but this is not the case of "m" variable.
  foreach (MemoryPlace *place, m_memoryPlaces) {
    if (place->m_type == MemoryPlace::EXPLICIT_VARIABLE ||
        place->m_type == MemoryPlace::IMPLICIT_VARIABLE) {
      // We need to store "" for VOID variables, for better recovering
      // value when variables are loaded.
      query_obj.exec(query_add_variable.arg(place->m_name,
                                            place->m_value->GetType() == 'v' ?
                                              "" :
                                              QString::fromStdWString(place->m_value->ToString()),
                                            place->m_description));
    }
  }

  // Commit the transaction.
  query_obj.exec("COMMIT");

  Database::removeDatabaseConnection(objectName());
}

bool Calculator::removeMemoryPlace(const QString &name) {
  if (m_memoryPlaces.contains(name) == true) {
    // Variable/constant was found so remove it.
    if (m_memoryPlaces.value(name)->m_type == MemoryPlace::CONSTANT) {
      // Remove constant.
      m_parser->RemoveConst(name.toStdWString());
    }
    else {
      // Remove variable.
      m_parser->RemoveVar(name.toStdWString());
    }

    // Clear memory places from the list.
    delete m_memoryPlaces.take(name);
    return true;
  }
  return false;
}

bool Calculator::addMemoryPlace(const QString &name,
                                const QString &desc,
                                const MemoryPlace::Type &type,
                                const Value &initial_value) {
  try {
    if (m_memoryPlaces.contains(name) == true) {
      throw ParserError(ErrorContext(ecINTERNAL_ERROR, 0, name.toStdWString()));
    }
    else {
      m_memoryPlaces.insert(name, new MemoryPlace(name, desc, initial_value, type));

      switch(type) {
        case MemoryPlace::CONSTANT:
          m_parser->DefineConst((const std::wstring&) name.toStdWString(),
                                *m_memoryPlaces[name]->m_value);
          break;
        case MemoryPlace::EXPLICIT_VARIABLE:
        case MemoryPlace::IMPLICIT_VARIABLE:
        case MemoryPlace::SPECIAL_VARIABLE:
          m_parser->DefineVar((const std::wstring&) name.toStdWString(),
                              *m_memoryPlaces[name]->m_variable);
          break;
        default:
          break;
      }
    }
  }
  catch (const ParserError &e) {
    qDebug("Entity '%s' cannot be added because the same entity already exists.",
           qPrintable(name));

    delete m_memoryPlaces.take(name);
    return false;
  }

  return true;
}

bool Calculator::editMemoryPlace(const QString &name,
                                 const QString &new_desc,
                                 const MemoryPlace::Type &new_type,
                                 const Value &new_value) {
  removeMemoryPlace(name);
  addMemoryPlace(name, new_desc, new_type, new_value);
  return true;
}

ConstantsModel *Calculator::getConstantsModel() {
  return m_constantsModel;
}

bool Calculator::isNameAllowed(const QString &name) {
  try {
    std::wstring name_wstr = name.toStdWString();
    m_parser->CheckName(name_wstr, m_parser->ValidNameChars());

    if (m_parser->IsVarDefined(name_wstr) ||
        m_parser->IsConstDefined(name_wstr) ||
        m_parser->IsFunDefined(name_wstr) ||
        m_parser->IsOprtDefined(name_wstr) ||
        m_parser->IsPostfixOprtDefined(name_wstr) ||
        m_parser->IsInfixOprtDefined(name_wstr)) {
      throw ParserError(ErrorContext(ecVARIABLE_DEFINED, 0, name_wstr));
    }
  }
  catch (...) {
    return false;
  }
  return true;
}

bool Calculator::isFunDefined(const QString &function_name) {
  return m_parser->IsFunDefined(function_name.toStdWString());
}

int Calculator::countOfMemoryPlaces() {
  return m_memoryPlaces.size();
}

QVariant Calculator::queryVariable(const QString &name,
                                   ConstantsModel::QueryType info_class) {
  QList<QString> keys = m_memoryPlaces.uniqueKeys();
  return queryVariable(keys.indexOf(name), info_class);
}

QVariant Calculator::queryVariable(int index, ConstantsModel::QueryType info_class) {
  if (index >= 0 && index < m_memoryPlaces.size()) {
    QList<QString> keys = m_memoryPlaces.uniqueKeys();

    switch (info_class) {
      case ConstantsModel::NAME:
        return m_memoryPlaces[keys[index]]->m_name;
      case ConstantsModel::VALUE:
        return QString::fromStdWString(m_memoryPlaces[keys[index]]->m_value->ToString());
      case ConstantsModel::DESCRIPTION:
        return m_memoryPlaces[keys[index]]->m_description;
      case ConstantsModel::ENTITY_TYPE:
        return m_memoryPlaces[keys[index]]->m_type;
      case ConstantsModel::VALUE_TYPE:
        return m_memoryPlaces[keys[index]]->m_value->GetType();
      case ConstantsModel::RAW_VALUE:
        // Sometimes we need to direct access to Value object.
        return QVariant::fromValue(m_memoryPlaces[keys[index]]->m_value);
      default:
        return QVariant();
    }
  }
  else {
    return QVariant();
  }
}

// Constants are initialized just ONCE, when calculator is created.
void Calculator::loadConstants() {
  // Clear previously defined constants.
  m_parser->ClearConst();

  // Define constants.
  addMemoryPlace("i", tr("imaginary unit"), MemoryPlace::CONSTANT, cmplx_type(0.0, 1.0));
  addMemoryPlace("_phi", tr("golden ratio"), MemoryPlace::CONSTANT, 1.618033988749894);
  addMemoryPlace("_d", tr("silver ratio"), MemoryPlace::CONSTANT, 2.414213562373095);
  addMemoryPlace("_pi", tr("Archimedes' constant"), MemoryPlace::CONSTANT, 3.141592653589793);
  addMemoryPlace("_e", tr("Euler's number"), MemoryPlace::CONSTANT, 2.718281828459045);
  addMemoryPlace("_z", tr("Apéry's constant"), MemoryPlace::CONSTANT, 1.202056903159594);
  addMemoryPlace("_c", tr("Euler–Mascheroni constant"), MemoryPlace::CONSTANT, 0.577215664901532);
  addMemoryPlace("_p", tr("Pythagoras' constant"), MemoryPlace::CONSTANT, 1.414213562373095);
  addMemoryPlace("_g", tr("Gelfond's constant"), MemoryPlace::CONSTANT, 23.140692632779269);
  addMemoryPlace("_gs", tr("Gelfond-Schneider constant"), MemoryPlace::CONSTANT, 2.665144142690225);
  addMemoryPlace("_fr", tr("Feigenbaum reduction parameter"), MemoryPlace::CONSTANT, -2.502907875095892);
  addMemoryPlace("_fb", tr("Feigenbaum bifurcation velocity"), MemoryPlace::CONSTANT, 4.669201609102990);
  addMemoryPlace("_ga", tr("Gauss' constant"), MemoryPlace::CONSTANT, 0.834626841674073);
  addMemoryPlace("_la", tr("Laplace limit constant"), MemoryPlace::CONSTANT, 0.662743419349181);
  addMemoryPlace("_ma", tr("magic angle [in radians]"), MemoryPlace::CONSTANT, 0.955316618124509);
  addMemoryPlace("_ta", tr("tetrahedral angle [in radians]"), MemoryPlace::CONSTANT, 1.910633236249018);
  addMemoryPlace("_gr", tr("gravitoid constant"), MemoryPlace::CONSTANT, 1.240806478802799);
  addMemoryPlace("_sd", tr("square drill constant"), MemoryPlace::CONSTANT, 0.987700390736053);
  addMemoryPlace("_ar", tr("Artin's constant"), MemoryPlace::CONSTANT, 0.373955813619202);

  // Define functions.
  addMemoryPlace("asin", tr("arcus sine"), MemoryPlace::FUNCTION);
  addMemoryPlace("acos", tr("arcus cosine"), MemoryPlace::FUNCTION);
  addMemoryPlace("atan", tr("arcus tangent"), MemoryPlace::FUNCTION);
  addMemoryPlace("asinh", tr("arcus hyperbolic sine"), MemoryPlace::FUNCTION);
  addMemoryPlace("acosh", tr("arcus hyperbolic cosine"), MemoryPlace::FUNCTION);
  addMemoryPlace("atanh", tr("arcus hyperbolic tangent"), MemoryPlace::FUNCTION);
  addMemoryPlace("sin", tr("sine"), MemoryPlace::FUNCTION);
  addMemoryPlace("cos", tr("cosine"), MemoryPlace::FUNCTION);
  addMemoryPlace("tan", tr("tangent"), MemoryPlace::FUNCTION);
  addMemoryPlace("sinh", tr("hyperbolic sine"), MemoryPlace::FUNCTION);
  addMemoryPlace("cosh", tr("hyperbolic cosine"), MemoryPlace::FUNCTION);
  addMemoryPlace("tanh", tr("hyperbolic tangent"), MemoryPlace::FUNCTION);
  addMemoryPlace("abs", tr("absolute value of a number"), MemoryPlace::FUNCTION);
  addMemoryPlace("ln", tr("natural logarithm (base _e)"), MemoryPlace::FUNCTION);
  addMemoryPlace("log", tr("logarithm (base 10)"), MemoryPlace::FUNCTION);
  addMemoryPlace("log10", tr("logarithm (base 10)"), MemoryPlace::FUNCTION);
  addMemoryPlace("log2", tr("logarithm (base 2)"), MemoryPlace::FUNCTION);
  addMemoryPlace("sqrt", tr("square root"), MemoryPlace::FUNCTION);
  addMemoryPlace("exp", tr("exponential function"), MemoryPlace::FUNCTION);
  addMemoryPlace("min", tr("minimum of numbers"), MemoryPlace::FUNCTION);
  addMemoryPlace("max", tr("maximum of numbers"), MemoryPlace::FUNCTION);
  addMemoryPlace("sum", tr("sum of numbers"), MemoryPlace::FUNCTION);
  addMemoryPlace("str2dbl", tr("string to double converter"), MemoryPlace::FUNCTION);
  addMemoryPlace("strlen", tr("length of a string"), MemoryPlace::FUNCTION);
  addMemoryPlace("toupper", tr("upper-cased version of the string"), MemoryPlace::FUNCTION);
  addMemoryPlace("tolower", tr("lower-cased version of the string"), MemoryPlace::FUNCTION);
  addMemoryPlace("real", tr("real number of the complex number"), MemoryPlace::FUNCTION);
  addMemoryPlace("imag", tr("imaginary part of the complex number"), MemoryPlace::FUNCTION);
  addMemoryPlace("arg", tr("arguent of the complex number"), MemoryPlace::FUNCTION);
  addMemoryPlace("conj", tr("complex conjugate of the complex number"), MemoryPlace::FUNCTION);
  addMemoryPlace("norm", tr("norm of the complex number"), MemoryPlace::FUNCTION);
  addMemoryPlace("median", tr("median"), MemoryPlace::FUNCTION);
}

void Calculator::initialize() {
  m_parser = new ParserX(pckCOMMON | pckCOMPLEX | pckSTRING);
  m_parser->EnableOptimizer(true);

  // Set custom error message handler.
  m_parser->ResetErrorMessageProvider(new ParserMessageProviderEnglish());

  // Register custom functions and operators.
  // All operators and functions are freed automatically.
  m_parser->DefineInfixOprtChars(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ()/+-*^?<>=#!$%&|~'_√"));
  m_parser->DefineFun(new FunMedian());
  m_parser->DefineInfixOprt(new OprtSqrt());

  // These are normally in pckNON_COMPLEX but that package
  // conflicts with complex. So add some functions
  // from it manually.
  m_parser->DefineFun(new FunASin());
  m_parser->DefineFun(new FunACos());
  m_parser->DefineFun(new FunATan());
  m_parser->DefineFun(new FunASinH());
  m_parser->DefineFun(new FunACosH());
  m_parser->DefineFun(new FunATanH());

  // Enables or disables outputs for calculator.
  m_parser->EnableDebugDump(false, false);

  // Create constants model.
  m_constantsModel = new ConstantsModel(this);

  // Load variables and constants.
  loadMemoryPlaces();

  emit initialized();
}

Value Calculator::calculateExpressionSynchronously(Calculator::CallerFunction function,
                                                   QString expression) {
  switch (function) {
    case CALCULATOR_ONTHEFLY:
    case CONVERTER_ONTHEFLY:
    case EDITOR_ONTHEFLY:
      // Used for evaluating variables stored in textual form.
    case INTERNAL:
      m_parser->EnableAutoCreateVar(false);
      break;
    case CALCULATOR_RESULT:
      m_parser->EnableAutoCreateVar(Settings::value(APP_CFG_CALC,
                                                    "implicit_value_creation",
                                                    false).toBool());
      break;
    default:
      break;
  }

  m_parser->SetExpr(expression.toStdWString());

  qDebug().nospace() << "Evaluating expression in thread " << QThread::currentThreadId() << ".";
  return m_parser->Eval();
}

void Calculator::calculateExpression(Calculator::CallerFunction function,
                                     QString expression) {
  Value result;

  try {
    // Try to calculate the result.
    result = calculateExpressionSynchronously(function,
                                              expression);
  }
  catch (ParserError &e) {
    // We got error so asker needs to be alerted about that.
    // Setting special enum value so that asker can react differently.
    switch (function) {
      case CALCULATOR_ONTHEFLY:
      case CALCULATOR_RESULT:
        emit resultCalculated(Calculator::CALCULATOR_ERROR,
                              Value(),
                              QString::fromStdWString(e.GetMsg()));
        break;
      case CONVERTER_ONTHEFLY:
        emit resultCalculated(Calculator::CONVERTER_ERROR,
                              Value(),
                              QString::fromStdWString(e.GetMsg()));
        break;
      case EDITOR_ONTHEFLY:
        emit resultCalculated(Calculator::EDITOR_ERROR);
      default:
        break;
    }
    return;
  }

  // In some cases, there is need of extra work or specific behavior.
  // Update model with memory places, because new implicit variable
  // could be created in this expression.
  if (function == CALCULATOR_RESULT) {
    changeAns(result);
    consolidateMemoryPlaces();
  }

  // Emit out the final result.
  emit resultCalculated(function, result);
}

void Calculator::changeAns(const Value &new_value) {
  Value *value_of_ans = queryVariable("ans",
                                      ConstantsModel::RAW_VALUE).value<Value*>();

  editMemoryPlace("ansx",
                  tr("result of 'ans' before previous calculation"),
                  MemoryPlace::SPECIAL_VARIABLE,
                  value_of_ans == nullptr ? Value(0) : Value(*value_of_ans));
  editMemoryPlace("ans",
                  tr("result of previous calculation"),
                  MemoryPlace::SPECIAL_VARIABLE,
                  new_value);
}
