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

#ifndef MEMORYPLACE_H
#define MEMORYPLACE_H

#include <QString>

#include "mpParser.h"


using namespace mup;

// Represents entity which as able to hold value.
// This includes calculator variables and constants.
struct MemoryPlace {
    enum Type {
      CONSTANT = 0,
      IMPLICIT_VARIABLE = 1,
      EXPLICIT_VARIABLE = 2,
      SPECIAL_VARIABLE = 3,
      FUNCTION = 4
    };

    QString m_name;
    QString m_description;
    Value *m_value;
    Type m_type;

    // This is used only by variables, each constant has m_variable equal to nullptr.
    // So there is way to distinguish variables from constants.
    Variable *m_variable;

    // Some variables are created implicitly by calculator engine.
    // These variables are special because they are cleaned automatically
    // by calculator engine.
    // Each constant has set this to true
    // Each variable has set this to true or false.
    bool m_shadow;

    // Constructs "empty" variable.
    // This constructor is used for constructing
    // "shallow" clones of implicitly-created variables.
    MemoryPlace(const QString &name);

    // Creates new variable or constant
    MemoryPlace(const QString &name, const QString &description,
                const Value &value, const Type &type);

    // Destructor.
    ~MemoryPlace();
};

#endif // MEMORYPLACE_H
