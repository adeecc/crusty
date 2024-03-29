#include <algorithm>
#include <common/errorlogger.hpp>
#include <fstream>
#include <iostream>
#include <iterator>
#include <parser/lexer.hpp>

using namespace Crust;

const std::array<std::string, (size_t)Lexer::Token::UNKNOWN + 1> Lexer::token_to_str = {
    "KW_INT_32",
    "KW_INT_64",
    "KW_UINT_32",
    "KW_UINT_64",
    "KW_FLOAT_32",
    "KW_FLOAT_64",
    "KW_STRING",
    "KW_BOOL",
    "KW_VOID",
    "KW_TRUE",
    "KW_FALSE",
    "KW_LET",
    "KW_IF",
    "KW_ELIF",
    "KW_ELSE",
    "KW_FOR",
    "KW_IN",
    "KW_WHILE",
    "KW_FN",
    "KW_RETURN",

    "INT_LITERAL",
    "FLOAT_LITERAL",
    "STR_LITERAL",

    "IDENTIFIER",

    "OP_PLUS",
    "OP_MINUS",
    "OP_MULT",
    "OP_DIV",
    "OP_MOD",
    "OP_AND",
    "OP_OR",
    "OP_GT",
    "OP_GE",
    "OP_EQ",
    "OP_NE",
    "OP_LE",
    "OP_LT",

    "DOT",
    "SEMI_COLON",
    "COLON",
    "COMMA",
    "AMP",
    "RANGE",
    "ASSIGN",
    "NAMESPACE",
    "LBRACE",
    "RBRACE",
    "LBRACKET",
    "RBRACKET",
    "LPAREN",
    "RPAREN",

    "COMMENT",

    "TOK_SOF",

    "TOK_EOF",

    "UNKNOWN",
};

bool Lexer::init(const std::string& filename) {
    std::ifstream stream(filename);
    if (stream) {
        mBuffer.assign(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
        mBufferIt = mBuffer.begin();
        mSrcLoc.init();
        return true;
    }
    return false;
}

char Lexer::advance() {
    mSrcLoc.advance(*mBufferIt == '\n');
    ++mBufferIt;
    if (mBufferIt == mBuffer.end()) return 0;
    return *mBufferIt;
}

Lexer::Token Lexer::getNextTokenAndComment() {
    while (mBufferIt != mBuffer.end() && isspace(*mBufferIt)) {
        advance();
    }

    // End of file, we're done
    if (mBufferIt == mBuffer.end())
        return Token::TOK_EOF;

    const char currentChar = *mBufferIt;

    switch (currentChar) {
        case '.':
            if (advance() == '.') {
                advance();
                return Token::RANGE;
            } else {
                return Token::DOT;
            }
        case ',':
            advance();
            return Token::COMMA;
        case ':':
            if (advance() == ':') {
                advance();
                return Token::NAMESPACE;
            } else {
                return Token::COLON;
            }
        case ';':
            advance();
            return Token::SEMI_COLON;
        case '&':
            advance();
            return Token::AMP;
        case '{':
            advance();
            return Token::LBRACE;
        case '[':
            advance();
            return Token::LBRACKET;
        case '(':
            advance();
            return Token::LPAREN;
        case '}':
            advance();
            return Token::RBRACE;
        case ']':
            advance();
            return Token::RBRACKET;
        case ')':
            advance();
            return Token::RPAREN;
        case '=':
            if (advance() == '=') {
                advance();
                return Token::OP_EQ;
            } else {
                return Token::ASSIGN;
            }

        case '+':
            advance();
            return Token::OP_PLUS;
        case '*':
            advance();
            return Token::OP_MULT;
        case '%':
            advance();
            return Token::OP_MOD;
        case '-':
            advance();
            return Token::OP_MINUS;
        case '/':
            if (advance() == '/') {
                // A comment covers a whole line
                while (mBufferIt != mBuffer.end() && *mBufferIt != '\n')
                    advance();
                return Token::COMMENT;
            } else {
                return Token::OP_DIV;
            }

        case '>':
            if (advance() == '=') {
                advance();
                return Token::OP_GE;
            } else {
                return Token::OP_GT;
            }
        case '<':
            if (advance() == '=') {
                advance();
                return Token::OP_LE;
            } else {
                return Token::OP_LT;
            }
        case '!':
            if (advance() == '=') {
                advance();
                return Token::OP_NE;
            } else {
                ErrorLogger::printErrorAtLocation(ErrorLogger::ErrorType::INVALID_SYMBOL, mSrcLoc);
                return Token::UNKNOWN;  // TODO: Add the Not operator?
            }

        case '\"':
            mCurrentStr.clear();
            while ((advance() != 0) && (*mBufferIt != '\n') && (*mBufferIt != '\"'))
                mCurrentStr += *mBufferIt;

            if (mBufferIt == mBuffer.end()) {  // Buffer Ended before closing string
                ErrorLogger::printErrorAtLocation(ErrorLogger::ErrorType::MISSING_CLOSING_QUOTE, mSrcLoc);
                return Token::UNKNOWN;
            } else if (*mBufferIt == '\n') {  // Line Ended before closing string
                ErrorLogger::printErrorAtLocation(ErrorLogger::ErrorType::NEW_LINE_IN_LITERAL, mSrcLoc);

                while ((mBufferIt != mBuffer.end()) && (*mBufferIt != '\"'))
                    advance();

                advance();

                return Token::UNKNOWN;
            } else {  // String Found!
                advance();
                return Token::STR_LITERAL;
            }

        default:
            if (isalpha(currentChar) or currentChar == '_') {
                mCurrentStr = currentChar;
                while (advance() != 0 and (isalnum(*mBufferIt) or *mBufferIt == '_')) {
                    mCurrentStr += *mBufferIt;
                }

                return tokenizeCurrentStr();
            }

            else if (isdigit(currentChar)) {
                std::string numberStr;
                do {
                    numberStr += *mBufferIt;
                } while (isdigit(advance()));

                if (*mBufferIt == '.') {
                    numberStr += *mBufferIt;
                    advance();

                    do {
                        numberStr += *mBufferIt;
                    } while (isdigit(advance()));

                    mCurrentFloat = std::stof(numberStr);
                    return Token::FLOAT_LITERAL;
                }

                else if (isalpha(*mBufferIt)) {
                    ErrorLogger::printErrorAtLocation(ErrorLogger::ErrorType::NUMBER_BAD_SUFFIX, mSrcLoc);

                    while ((mBufferIt != mBuffer.end()) and (*mBufferIt != ';'))
                        advance();

                    if (mBufferIt != mBuffer.end())
                        advance();

                    return Token::UNKNOWN;
                }

                else {
                    mCurrentInt = std::stoi(numberStr);
                    return Token::INT_LITERAL;
                }
            } else {
                advance();
                ErrorLogger::printErrorAtLocation(ErrorLogger::ErrorType::INVALID_SYMBOL, mSrcLoc);
                return Token::UNKNOWN;
            }
    }

    ErrorLogger::printErrorAtLocation(ErrorLogger::ErrorType::INVALID_SYMBOL, mSrcLoc);
    return Token::UNKNOWN;
}

Lexer::Token Lexer::getNextToken() {
    Token current = getNextTokenAndComment();
    while (current == Lexer::Token::COMMENT) {
        current = getNextTokenAndComment();
    }

#ifndef NDEBUG
    std::cout << "Returning Token: " << Lexer::token_to_str[(size_t)current] << "\n";
#endif

    return current;
}

Lexer::Token Lexer::tokenizeCurrentStr() {
    if (mCurrentStr == "i32")
        return Token::KW_INT_32;
    else if (mCurrentStr == "i64")
        return Token::KW_INT_64;
    else if (mCurrentStr == "u32")
        return Token::KW_UINT_32;
    else if (mCurrentStr == "u64")
        return Token::KW_UINT_64;
    else if (mCurrentStr == "f32")
        return Token::KW_FLOAT_32;
    else if (mCurrentStr == "f64")
        return Token::KW_FLOAT_64;
    else if (mCurrentStr == "string")
        return Token::KW_STRING;
    else if (mCurrentStr == "bool")
        return Token::KW_BOOL;
    else if (mCurrentStr == "void")
        return Token::KW_VOID;
    else if (mCurrentStr == "true")
        return Token::KW_TRUE;
    else if (mCurrentStr == "false")
        return Token::KW_FALSE;
    else if (mCurrentStr == "let")
        return Token::KW_LET;

    else if (mCurrentStr == "if")
        return Token::KW_IF;
    else if (mCurrentStr == "elif")
        return Token::KW_ELIF;
    else if (mCurrentStr == "else")
        return Token::KW_ELSE;
    else if (mCurrentStr == "for")
        return Token::KW_FOR;
    else if (mCurrentStr == "in")
        return Token::KW_IN;
    else if (mCurrentStr == "while")
        return Token::KW_WHILE;

    else if (mCurrentStr == "fn")
        return Token::KW_FN;
    else if (mCurrentStr == "return")
        return Token::KW_RETURN;

    else if (mCurrentStr == "and")
        return Token::OP_AND;
    else if (mCurrentStr == "or")
        return Token::OP_OR;
    else
        return Token::IDENTIFIER;
}