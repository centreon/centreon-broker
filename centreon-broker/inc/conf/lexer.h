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

#ifndef CONF_LEXER_H_
# define CONF_LEXER_H_

# include <list>
# include <memory>
# include <string>
# include "io/io.h"

namespace                CentreonBroker
{
  namespace              Conf
  {
    /**
     *  \class Token lexer.h "conf/lexer.h"
     *  \brief Represents a token.
     *
     *  When parsing, the data stream is first split into tokens. Those tokens
     *  will then be analyzed to produce whathever result is desired.
     *
     *  \see Lexer
     */
    class                Token
    {
     public:
      enum               Type
      {
        UNKNOWN = 0,
        ASSIGNMENT,
        BLOCK_END,
        BLOCK_START,
        END,
        PIPE,
        STRING
      };

     private:
      std::string        text_;
      Type               type_;

     public:
                         Token();
                         Token(const Token& token);
                         ~Token();
      Token&             operator=(const Token& token);
      const std::string& GetText() const throw ();
      Type               GetType() const throw ();
      void               SetText(const std::string& text);
      void               SetType(Type type) throw ();
    };

    /**
     *  \class Lexer lexer.h "conf/lexer.h"
     *  \brief Generate a token chain from a data stream.
     *
     *  In the parsing context, a lexer is responsible of splitting a raw byte
     *  input into a chain of tokens. This token chain will be then analyzed to
     *  determine what it means.
     *
     *  \see Token
     */
    class                Lexer
    {
     private:
      char               buffer_[1024];
      std::list<Token>   context_;
      unsigned int       current_;
      std::list<std::list<Token> >
                         saves_;
      unsigned int       size_;
      std::auto_ptr<CentreonBroker::IO::Stream>
                         stream_;
                         Lexer(const Lexer& lexer);
      Lexer&             operator=(const Lexer& lexer);
      char               NextChar();
      void               UngetChar(char c) throw ();

     public:
                         Lexer(IO::Stream* stream);
                         ~Lexer();
      void               ContextPop();
      void               ContextRestore();
      void               ContextSave();
      bool               GetToken(Token& token);
    };
  }
}

#endif /* !CONF_LEXER_H_ */
