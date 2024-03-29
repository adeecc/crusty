#pragma once

#include <string>
#include <type_traits>
#include <unordered_map>

namespace Crust {
class SourceLocation;

/*
 * \class ErrorLogger
 * \brief Singleton class providing error logging services for the whole TosLang compiler
 */
class ErrorLogger {
   public:
    enum class ErrorType : unsigned int {
        // Symbol
        INVALID_SYMBOL,

        // Array
        ARRAY_EMPTY_EXPR,
        ARRAY_UNEXPECTED_END,
        ARRAY_MISSING_COMMA,
        ARRAY_MISSING_NUMBER,
        ARRAY_MISSING_RIGHT_BRACKET,
        ARRAY_MULTIPLE_TYPES,
        ARRAY_NOT_A_NUMBER,
        ARRAY_ZERO_LENGTH,

        // Binary Expr
        MISSING_RHS,
        WRONG_OPERATION,
        WRONG_USE_OPERATION,

        // Call
        CALL_ARG_ERROR,
        CALL_MISSING_PAREN,
        CALL_PARAM_TYPE_ERROR,
        CALL_RETURN_ERROR,
        CALL_NB_ARGS_ERROR,

        // File
        WRONG_FILE_TYPE,
        ERROR_OPENING_FILE,

        // Function
        FN_INTERNAL,
        FN_MISSING_ARROW,
        FN_MISSING_TYPE,
        FN_MISSING_IDENTIFIER,
        FN_MISSING_LEFT_PAREN,
        FN_MISSING_RIGHT_PAREN,
        FN_MISSING_RETURN,
        FN_MISSING_RETURN_TYPE,
        FN_REDEFINITION,
        FN_UNDECLARED,

        // If
        IF_MISSING_BODY,
        IF_MISSING_COND,

        // IO
        PRINT_WRONG_INPUT_TYPE,
        SCAN_MISSING_INPUT_VAR,
        SCAN_WRONG_INPUT_TYPE,

        // Literal
        NEW_LINE_IN_LITERAL,
        MISSING_CLOSING_QUOTE,
        NUMBER_BAD_SUFFIX,

        // Param
        PARAM_MISSING_NAME,
        PARAM_MISSING_COLON,
        PARAM_MISSING_TYPE,

        // Syntax
        SYNTAX_MISSING_LBRACE,
        SYNTAX_MISSING_RBRACE,

        // Type
        WRONG_BIN_EXPR_TYPE,
        WRONG_COND_EXPR_TYPE,
        WRONG_EXPR_TYPE,
        WRONG_INIT_ARRAY_SCALAR,
        WRONG_INIT_SCALAR_ARRAY,
        WRONG_LITERAL_TYPE,
        WRONG_RETURN_TYPE,
        WRONG_VARIABLE_TYPE,

        // Var
        PARAM_REDEFINITION,
        VAR_MISSING_TYPE,
        VAR_MISSING_COLON,
        VAR_MISSING_IDENTIFIER,
        VAR_REDEFINITION,
        VAR_UNDECLARED_IDENTIFIER,
        VAR_VOID_TYPE,

        // While
        WHILE_MISSING_BODY,
        WHILE_MISSING_COND,

        // Misc
        EXPECTED_DECL,
        MISSING_SEMI_COLON,
        UNCLOSED_ML_COMMENT,
    };

   public:
    ErrorLogger(const ErrorLogger&) = delete;
    ErrorLogger& operator=(const ErrorLogger&) = delete;
    ~ErrorLogger() = default;

   public:
    static void printError(ErrorType eType);

    static void printErrorAtLocation(ErrorType eType, const SourceLocation& srcLoc);

   private:
    ErrorLogger() = default;

   private:
    static std::unordered_map<ErrorType, std::string> mErrorMessages; /*!< Maps the error types to the error messages */
    static ErrorLogger mInstance;                                     /*!< Single instance of the error logger */
};
}  // namespace Crust