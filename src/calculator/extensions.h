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

#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include <QList>

#include <limits>

#include "mpICallback.h"
#include "mpIToken.h"
#include "mpIOprt.h"
#include "mpIValue.h"
#include "mpError.h"


using namespace mup;

class OprtSqrt : public IOprtInfix {
  public:
    OprtSqrt() : IOprtInfix(_T("√")) {
    }

    virtual void Eval(ptr_val_type &ret, const ptr_val_type *a_pArg, int a_iArgc) {
      if (a_iArgc != 1) {
        throw ParserError(ErrorContext(a_iArgc > 1 ?
                                         ecTOO_MANY_PARAMS :
                                         ecTOO_FEW_PARAMS,
                                       GetExprPos(),
                                       GetIdent()));
      }
      if (!a_pArg[0]->IsScalar()) {
        throw ParserError(ErrorContext(ecTYPE_CONFLICT_FUN,
                                       GetExprPos(),
                                       GetIdent(),
                                       a_pArg[0]->GetType(), 'f', 1));
      }

      *ret = (float_type) sqrt(a_pArg[0]->GetFloat());
    }

    virtual const char_type* GetDesc() const {
      return _T("√x - Square root of number");
    }

    virtual IToken* Clone() const {
      return new OprtSqrt(*this);
    }
};

class FunMedian : public ICallback {
  public:
    FunMedian() : ICallback(cmFUNC, _T("median"), -1) {
    }

    virtual void Eval(ptr_val_type &ret, const ptr_val_type *a_pArg, int a_iArgc) {
      if (a_iArgc < 1) {
        throw ParserError(ErrorContext(ecTOO_FEW_PARAMS, GetExprPos(),
                                       GetIdent()));
      }

      QList<float_type> list_of_elems;

      for (int i = 0; i < a_iArgc; i++) {
        switch(a_pArg[i]->GetType()) {
          case 'f':
          case 'i':
            list_of_elems << a_pArg[i]->GetFloat();
            break;
          default: {
            throw ParserError(ErrorContext(ecTYPE_CONFLICT_FUN,
                                           i+1,
                                           GetIdent(),
                                           a_pArg[i]->GetType(),
                                           'f',
                                           a_iArgc));
          }
        }
      }

      qSort(list_of_elems);
      int size_of_elems = list_of_elems.size();

      // ODD
      if (size_of_elems % 2 == 0) {
        *ret =  (list_of_elems[size_of_elems / 2] +
                list_of_elems[(size_of_elems / 2) - 1]) / 2.0;
      }
      // EVEN
      else {
        *ret = list_of_elems[(size_of_elems - 1) / 2];
      }
    }

    const char_type* GetDesc() const {
      return _T("median(x,y,z,...) - A numerical median");
    }

    IToken* Clone() const {
      return new FunMedian(*this);
    }
};

#endif // EXTENSIONS_H
