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

#include "memoryplace.h"


MemoryPlace::MemoryPlace(const QString &name) {
  m_name = name;
}

MemoryPlace::MemoryPlace(const QString &name, const QString &description,
                         const Value &value, const Type &type) {
  m_name = name;
  m_description = description;
  m_value = new Value(value);
  m_type = type;
  m_variable = m_type == CONSTANT || m_type == FUNCTION ?
                 nullptr :
                 new Variable(m_value);
}

MemoryPlace::~MemoryPlace() {
  // Free resources of this object if:
  if (m_type == CONSTANT || m_type == FUNCTION) {
    delete m_value;
    qDebug("Constant '%s' deleted.", qPrintable(m_name));
  }
  else if (m_type == EXPLICIT_VARIABLE || m_type == SPECIAL_VARIABLE) {
    delete m_value;
    delete m_variable;
    qDebug("Variable '%s' deleted.", qPrintable(m_name));
  }
  else {
    qDebug("Implicitly-created variable '%s' deleted.", qPrintable(m_name));
  }
}
