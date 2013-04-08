#include <QObject>

#include <cassert>

#include "mpParserMessageProvider.h"
#include "mpError.h"


MUP_NAMESPACE_START

//-------------------------------------------------------------------------------------------------
//
// class ParserMessageProviderBase - Base class for message providers
//
//-------------------------------------------------------------------------------------------------

ParserMessageProviderBase::ParserMessageProviderBase()
    :m_vErrMsg(0)
{}

//-----------------------------------------------------------------------------------------------
ParserMessageProviderBase::~ParserMessageProviderBase()
{}

//-----------------------------------------------------------------------------------------------
void ParserMessageProviderBase::Init()
{
    InitErrorMessages();
    for (int i=0; i<ecCOUNT; ++i)
    {
	if (!m_vErrMsg[i].length())
	    throw std::runtime_error("Incomplete translation (at least one error code missing)");
    }
}

//---------------------------------------------------------------------------------------------
string_type ParserMessageProviderBase::GetErrorMsg(EErrorCodes eError) const
{
    int nError = (int)eError;
    return (nError<(int)m_vErrMsg.size()) ? m_vErrMsg[nError] : string_type();
}

//-----------------------------------------------------------------------------------------------
//
// class ParserMessageProviderEnglish - English Parser Messages (default)
//
//-----------------------------------------------------------------------------------------------

ParserMessageProviderEnglish::ParserMessageProviderEnglish()
    :ParserMessageProviderBase()
{}

//-----------------------------------------------------------------------------------------------
void ParserMessageProviderEnglish::InitErrorMessages()
{
    m_vErrMsg.resize(ecCOUNT);

    m_vErrMsg[ecUNASSIGNABLE_TOKEN]      = QObject::tr("Undefined token \"$IDENT$\" found at position $POS$.").toStdWString();
    m_vErrMsg[ecINTERNAL_ERROR]          = QObject::tr("Internal error.").toStdWString();
    m_vErrMsg[ecUNKNOWN_ESCAPE_SEQUENCE] = QObject::tr("Unknown escape sequence.").toStdWString();
    m_vErrMsg[ecINVALID_NAME]            = QObject::tr("Invalid function, variable or constant name.").toStdWString();
    m_vErrMsg[ecINVALID_FUN_PTR]         = QObject::tr("Invalid pointer to callback function.").toStdWString();
    m_vErrMsg[ecINVALID_VAR_PTR]         = QObject::tr("Invalid pointer to variable.").toStdWString();
    m_vErrMsg[ecUNEXPECTED_OPERATOR]     = QObject::tr("Unexpected operator \"$IDENT$\" found at position $POS$.").toStdWString();
    m_vErrMsg[ecUNEXPECTED_EOF]          = QObject::tr("Unexpected end of expression found at position $POS$.").toStdWString();
    m_vErrMsg[ecUNEXPECTED_COMMA]        = QObject::tr("Unexpected comma found at position $POS$.").toStdWString();
    m_vErrMsg[ecUNEXPECTED_PARENS  ]     = QObject::tr("Unexpected parenthesis \"$IDENT$\" found at position $POS$.").toStdWString();
    m_vErrMsg[ecUNEXPECTED_FUN]          = QObject::tr("Unexpected function \"$IDENT$\" found at position $POS$.").toStdWString();
    m_vErrMsg[ecUNEXPECTED_VAL]          = QObject::tr("Unexpected value \"$IDENT$\" found at position $POS$.").toStdWString();
    m_vErrMsg[ecUNEXPECTED_VAR]          = QObject::tr("Unexpected variable \"$IDENT$\" found at position $POS$.").toStdWString();
    m_vErrMsg[ecUNEXPECTED_STR]          = QObject::tr("Unexpected string token found at position $POS$.").toStdWString();
    m_vErrMsg[ecUNEXPECTED_CONDITIONAL]  = QObject::tr("The \"$IDENT$\" operator must be preceded by a closing bracket.").toStdWString();
    m_vErrMsg[ecUNEXPECTED_NEWLINE]      = QObject::tr("Unexprected newline.").toStdWString();
    m_vErrMsg[ecMISSING_PARENS]          = QObject::tr("Missing parenthesis.").toStdWString();
    m_vErrMsg[ecMISSING_ELSE_CLAUSE]     = QObject::tr("If-then-else operator is missing an else clause.").toStdWString();
    m_vErrMsg[ecMISPLACED_COLON]         = QObject::tr("Misplaced colon at position $POS$.").toStdWString();
    m_vErrMsg[ecTOO_MANY_PARAMS]         = QObject::tr("Too many parameters passed to function \"$IDENT$\".").toStdWString();
    m_vErrMsg[ecTOO_FEW_PARAMS]          = QObject::tr("Too few parameters passed to function \"$IDENT$\".").toStdWString();
    m_vErrMsg[ecDIV_BY_ZERO]             = QObject::tr("Division by zero occurred.").toStdWString();
    m_vErrMsg[ecDOMAIN_ERROR]            = QObject::tr("The value passed as argument to function/operator \"$IDENT$\" is not part of its domain.").toStdWString();
    m_vErrMsg[ecNAME_CONFLICT]           = QObject::tr("Name conflict.").toStdWString();
    m_vErrMsg[ecOPT_PRI]                 = QObject::tr("Invalid value for operator priority (must be greater or equal to zero).").toStdWString();
    m_vErrMsg[ecBUILTIN_OVERLOAD]        = QObject::tr("Binary operator identifier conflicts with a built in operator.").toStdWString();
    m_vErrMsg[ecUNTERMINATED_STRING]     = QObject::tr("Unterminated string starting at position $POS$.").toStdWString();
    m_vErrMsg[ecSTRING_EXPECTED]         = QObject::tr("String function called with a non string type of argument.").toStdWString();
    m_vErrMsg[ecVAL_EXPECTED]            = QObject::tr("Numerical function called with a non value type of argument.").toStdWString();
    m_vErrMsg[ecTYPE_CONFLICT]           = QObject::tr("Value \"$IDENT$\" is of type '$TYPE1$'. There is no implicit conversion to type '$TYPE2$'.").toStdWString();
    m_vErrMsg[ecTYPE_CONFLICT_FUN]       = QObject::tr("Argument $ARG$ of function/operator \"$IDENT$\" is of type '$TYPE1$' whereas type '$TYPE2$' was expected.").toStdWString();
    m_vErrMsg[ecTYPE_CONFLICT_IDX]       = QObject::tr("Index to \"$IDENT$\" must be a positive integer value. '$TYPE1$' is not an acceptable type.").toStdWString();
    m_vErrMsg[ecGENERIC]                 = QObject::tr("Parser error.").toStdWString();
    m_vErrMsg[ecINVALID_TYPE]            = QObject::tr("Invalid argument type.").toStdWString();
    m_vErrMsg[ecINVALID_TYPECAST]        = QObject::tr("Value type conversion from type '$TYPE1$' to '$TYPE2$' is not supported!").toStdWString();
    m_vErrMsg[ecARRAY_SIZE_MISMATCH]     = QObject::tr("Array size mismatch.").toStdWString();
    m_vErrMsg[ecNOT_AN_ARRAY]            = QObject::tr("Using the index operator on the scalar variable \"$IDENT$\" is not allowed.").toStdWString();
    m_vErrMsg[ecUNEXPECTED_SQR_BRACKET]  = QObject::tr("Unexpected \"]\".").toStdWString();
    m_vErrMsg[ecINDEX_OUT_OF_BOUNDS]     = QObject::tr("Index to variable \"$IDENT$\" is out of bounds.").toStdWString();
    m_vErrMsg[ecINDEX_DIMENSION]         = QObject::tr("Index operator dimension error.").toStdWString();
    m_vErrMsg[ecMISSING_SQR_BRACKET]     = QObject::tr("Missing \"]\".").toStdWString();
    m_vErrMsg[ecASSIGNEMENT_TO_VALUE]    = QObject::tr("Assignment operator \"$IDENT$\" can't be used in this context.").toStdWString();
    m_vErrMsg[ecEVAL]                    = QObject::tr("Can't evaluate function/operator \"$IDENT$\": $HINT$").toStdWString();
    m_vErrMsg[ecINVALID_PARAMETER]       = QObject::tr("Parameter $ARG$ of function \"$IDENT$\" is invalid.").toStdWString();
    m_vErrMsg[ecINVALID_NUMBER_OF_PARAMETERS] = QObject::tr("Invalid number of function arguments.").toStdWString();
    m_vErrMsg[ecOVERFLOW]                     = QObject::tr("Possible arithmetic overflow occurred in function/operator \"$IDENT$\".").toStdWString();
    m_vErrMsg[ecMATRIX_DIMENSION_MISMATCH]    = QObject::tr("Matrix dimension error.").toStdWString();
    m_vErrMsg[ecVARIABLE_DEFINED]   = QObject::tr("Variable \"$IDENT$\" is already defined.").toStdWString();
    m_vErrMsg[ecCONSTANT_DEFINED]   = QObject::tr("Constant \"$IDENT$\" is already defined.").toStdWString();
    m_vErrMsg[ecFUNOPRT_DEFINED]    = QObject::tr("Function/operator \"$IDENT$\" is already defined.").toStdWString();
}

MUP_NAMESPACE_END
