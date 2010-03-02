/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "configuration/lexer.h"
#include "io/stream.h"

using namespace Configuration;


/******************************************************************************
*                                                                             *
*                                                                             *
*                                    Token                                    *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Token default constructor.
 *
 *  The resulting token will be undefined ; text will be empty and type UKNOWN.
 */
Token::Token() : type_(UNKNOWN) {}

/**
 *  \brief Token copy constructor.
 *
 *  Copy type and text from the argument.
 *
 *  \param[in] token Object to copy data from.
 */
Token::Token(const Token& token) : text_(token.text_), type_(token.type_) {}

/**
 *  Token destructor.
 */
Token::~Token() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy type and text from the argument.
 *
 *  \param[in] token Object to copy data from.
 *
 *  \return *this
 */
Token& Token::operator=(const Token& token)
{
  this->text_ = token.text_;
  this->type_ = token.type_;
  return (*this);
}

/**
 *  Get the text associated with the token (if necessary).
 *
 *  \return The text associated with the token.
 */
const std::string& Token::GetText() const
{
  return (this->text_);
}

/**
 *  Get the type of the token.
 *
 *  \return The type of the token.
 */
Token::Type Token::GetType() const
{
  return (this->type_);
}

/**
 *  Set the text associated with the token.
 *
 *  \param[in] text Text to associate to the token.
 */
void Token::SetText(const std::string& text)
{
  this->text_ = text;
  return ;
}

/**
 *  Set the type of the token.
 *
 *  \param[in] type The type of the token.
 */
void Token::SetType(Token::Type type)
{
  this->type_ = type;
  return ;
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                    Lexer                                    *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Lexer copy constructor.
 *
 *  A lexer is not copyable for the moment. Any attempt to use the copy
 *  constructor will result in a call to abort().
 *
 *  \param[in] lexer Unused.
 */
Lexer::Lexer(const Lexer& lexer)
{
  (void)lexer;
  assert(false);
  abort();
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  A lexer is not copyable for the moment. Any attempt to use the assignment
 *  operator will result in a call to abort().
 *
 *  \param[in] lexer Unused.
 *
 *  \return *this
 */
Lexer& Lexer::operator=(const Lexer& lexer)
{
  (void)lexer;
  assert(false);
  abort();
  return (*this);
}

/**
 *  Get the next available byte in the byte stream.
 */
char Lexer::NextChar()
{
  char ret;

  // If stream is closed, return the end of string character.
  if (this->closed_)
    ret = '\0';
  // If buffer is empty, read more data.
  else if (this->current_ >= this->size_)
    {
      this->current_ = 0;
      // Read data from the byte stream.
      this->size_ = this->stream_->Receive(this->buffer_,
                                           sizeof(this->buffer_) - 1);
      // Read failed, stream is closed.
      if (!this->size_)
        this->closed_ = true;
      // Read succeeded, we now have some data in the buffer.
      else
        this->buffer_[this->size_] = '\0';
      ret = this->NextChar();
    }
  // If buffer is not empty, extract the next available character.
  else
    ret = this->buffer_[this->current_++];
  return (ret);
}

/**
 *  Push a character back to the buffer.
 */
void Lexer::UngetChar(char c)
{
  this->buffer_[--this->current_] = c;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Lexer constructor.
 *
 *  Build a lexer by giving it the data stream on which it will extract tokens.
 *  Once successfully constructed, the lexer will own the stream (ie. it will
 *  handle its destruction).
 *
 *  \param[in] stream Byte stream from which tokens will be generated.
 */
Lexer::Lexer(IO::Stream* stream)
  : closed_(false), current_(0), size_(0), stream_(stream) {}

/**
 *  Lexer destructor.
 */
Lexer::~Lexer() {}

/**
 *  \brief Destroy the most recently saved context.
 *
 *  The Lexer class holds internally a context list that can be expanded by
 *  ContextSave(). If one want to destroy the most recently saved one, it has
 *  to call ContextPop().
 */
void Lexer::ContextPop()
{
  this->saves_.pop_front();
  return ;
}

/**
 *  \brief Restore the most recently saved context.
 *
 *  The Lexer class holds internally a context list that can be expanded by
 *  ContextSave(). If one want to restore a previous context (typically because
 *  the parsing failed on a particular rule and want to test alternatives), it
 *  has to call ContextRestore().
 */
void Lexer::ContextRestore()
{
  this->context_ = this->saves_.front();
  this->saves_.pop_front();
  return ;
}

/**
 *  \brief Save the current context.
 *
 *  The Lexer class can backup token contexts. This can be useful when parsing
 *  and an alternative of the syntax failed.
 */
void Lexer::ContextSave()
{
  this->saves_.push_front(this->context_);
  return ;
}

/**
 *  \brief Get the next token.
 *
 *  Extract the next token from the data stream.
 *
 *  \param[out] token Next token.
 *
 *  \return true if there is no more tokens.
 */
bool Lexer::GetToken(Token& token)
{
  bool ret;

  // If the context is not empty, some token has already been parsed.
  if (!this->context_.empty())
    {
      token = this->context_.front();
      this->context_.pop_front();
      ret = false;
    }
  // Else we got to extract a token from the byte stream.
  else
    {
      char c;

      // Skip all whitespaces.
      for (c = this->NextChar(); c && isspace(c); c = this->NextChar())
        ;

      // We reached the end of the byte stream.
      if (!c)
        {
          token.SetType(Token::END);
          ret = true;
        }
      // There's some tokenizable data.
      else
        {
          ret = false;
          switch (c)
            {
             case '=':
              token.SetType(Token::ASSIGNMENT);
              break ;
             case '{':
              token.SetType(Token::BLOCK_START);
              break ;
             case '}':
              token.SetType(Token::BLOCK_END);
              break ;
             case '#': // Comment
              // Skip until the next line
              do
                {
                  c = this->NextChar();
                } while (c && c != '\n');
              if (!c)
                {
                  token.SetType(Token::END);
                  ret = true;
                }
              // If we haven't reach the end of the file,
              // retrieve the next token
              else
                ret = this->GetToken(token);
              break ;
             case '|':
              token.SetType(Token::PIPE);
              break ;
             case '"':
              {
                std::string str;

                for (c = this->NextChar(); c && c != '"'; c = this->NextChar())
                  str.push_back(c);
                token.SetText(str);
                token.SetType(Token::STRING);
              }
              break ;
             default:
              {
                std::string str;

                // Standard string, extract it until we reach a separator.
                do
                  {
                    str.push_back(c);
                    c = this->NextChar();
                  } while (c && (isalnum(c)
                                 || ('.' == c)
                                 || ('-' == c)
                                 || ('_' == c)
                                 || ('/' == c)));
                if (c)
                  this->UngetChar(c);
                token.SetText(str);
                token.SetType(Token::STRING);
              }
            }
          // Insert token in all saved contexts.
          if (!ret)
            for (std::list<std::list<Token> >::iterator it =
                   this->saves_.begin();
                 it != this->saves_.end();
                 ++it)
              (*it).push_back(token);
        }
    }
  return (ret);
}
