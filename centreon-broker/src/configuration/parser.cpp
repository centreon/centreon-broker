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
#include <memory>
#include <stdlib.h>                  // for abort
#include <string>
#include "configuration/interface.h"
#include "configuration/log.h"
#include "configuration/lexer.h"
#include "configuration/parser.h"
#include "exception.h"
#include "io/file.h"
#include "logging.h"

#define INVALID_TOKEN_MSG "Invalid token encountered while parsing " \
                          "configuration file ..."

using namespace Configuration;

// XXX : dirty hack to support module instance
std::string gl_instance;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

/**
 *  Process configuration options of an Interface.
 *
 *  \param[in]  lexer Lexer of the configuration file.
 *  \param[out] i     Object that will be set with extracted parameters.
 */
static void HandleInterface(Configuration::Lexer& lexer,
                            Configuration::Interface& i)
{
  Configuration::Token var;

  LOGDEBUG("Interface definition ...");
  for (lexer.GetToken(var);
       var.GetType() == Configuration::Token::STRING;
       lexer.GetToken(var))
    {
      Configuration::Token val;

      // Check if the current token is valid (var name).
      if (var.GetType() != Configuration::Token::STRING
          // And the next one too (=).
          || lexer.GetToken(val) || (val.GetType() != Configuration::Token::ASSIGNMENT)
          // And the next-next one too (var value).
          || lexer.GetToken(val) || (val.GetType() != Configuration::Token::STRING))
        throw (Exception(0, INVALID_TOKEN_MSG));

      // Extract var strings.
      const std::string& var_str = var.GetText();
      const std::string& val_str = val.GetText();

      // Parse variable.
      if (var_str == "db")
        i.db = val_str;
      else if (var_str == "failover")
        i.failover_name = val_str;
      else if (var_str == "filename")
        i.filename = val_str;
      else if (var_str == "host")
        {
          i.host = val_str;
          if (Configuration::Interface::IPV4_SERVER == i.type)
            i.type = Configuration::Interface::IPV4_CLIENT;
          else if (Configuration::Interface::IPV6_SERVER == i.type)
            i.type = Configuration::Interface::IPV6_CLIENT;
          else if (Configuration::Interface::UNIX_SERVER == i.type)
            i.type = Configuration::Interface::UNIX_CLIENT;
        }
      else if (var_str == "interface")
        i.interface = val_str;
      else if (var_str == "password")
        i.password = val_str;
      else if (var_str == "port")
        i.port = strtoul(val_str.c_str(), NULL, 0);
      else if (var_str == "protocol")
        i.protocol = ((val_str == "xml") ? Configuration::Interface::XML
                                         : Configuration::Interface::NDO);
      else if (var_str == "socket")
        i.socket = val_str;
      else if (var_str == "user")
        i.user = val_str;
      else if (var_str == "type")
        {
          if (val_str == "file")
            i.type = Configuration::Interface::FILE;
          else if ((val_str == "ip") || (val_str == "ipv4"))
            i.type = (i.host.empty() ? Configuration::Interface::IPV4_SERVER
                                     : Configuration::Interface::IPV4_CLIENT);
          else if (val_str == "ipv6")
            i.type = (i.host.empty() ? Configuration::Interface::IPV6_SERVER
                                     : Configuration::Interface::IPV6_CLIENT);
          else if (val_str == "mysql")
            i.type = Configuration::Interface::MYSQL;
          else if (val_str == "oracle")
            i.type = Configuration::Interface::ORACLE;
          else if (val_str == "postgresql")
            i.type = Configuration::Interface::POSTGRESQL;
          else if (val_str == "unix")
            i.type = (i.host.empty() ? Configuration::Interface::UNIX_SERVER
                                     : Configuration::Interface::UNIX_CLIENT);
        }
#ifdef USE_TLS
      else if (var_str == "ca")
        i.ca = val_str;
      else if (var_str == "cert")
        i.cert = val_str;
      else if (var_str == "compress")
        i.compress = strtoul(val_str.c_str(), NULL, 0);
      else if (var_str == "key")
        i.key = val_str;
      else if (var_str == "tls")
        i.tls = ((val_str == "yes") || strtoul(val_str.c_str(), NULL, 0));
#endif /* USE_TLS */
    }

  return ;
}

/**
 *  Process configuration options of a Log.
 *
 *  \param[in]  lexer Lexer of the configuration file.
 *  \param[out] log   Object that will be set with extracted parameters.
 */
static void HandleLog(Configuration::Lexer& lexer, Configuration::Log& log)
{
  Configuration::Token var;

  LOGDEBUG("Log definition ...");
  for (lexer.GetToken(var);
       var.GetType() != Configuration::Token::END && var.GetType() != Configuration::Token::BLOCK_END;
       lexer.GetToken(var))
    {
      Configuration::Token val;

      // Check if the current token is valid (var name).
      if ((var.GetType() != Configuration::Token::STRING)
          // And the next one too (=).
          || lexer.GetToken(val) || (val.GetType() != Configuration::Token::ASSIGNMENT)
          // And the next-next one too (var value).
          || lexer.GetToken(val) || (val.GetType() != Configuration::Token::STRING))
        throw (Exception(0, INVALID_TOKEN_MSG));

      // Extract var strings.
      const std::string var_str = var.GetText();
      const std::string val_str = val.GetText();

      // Parse variable.
      if (var_str == "facility")
        log.facility = strtol(val_str.c_str(), NULL, 0);
      else if (var_str == "flags")
        {
          unsigned int flags;

          flags = 0;

          // We will break when there's no more pipes.
          do
            {
              if (val.GetText() == "DEBUG")
                flags |= CentreonBroker::Logging::DEBUG;
              else if (val.GetText() == "ERROR")
                flags |= CentreonBroker::Logging::ERROR;
              else if (val.GetText() == "INFO")
                flags |= CentreonBroker::Logging::INFO;
              lexer.ContextSave();
              if (lexer.GetToken(val)
                  || (val.GetType() != Configuration::Token::PIPE)
                  || lexer.GetToken(val)
                  || (val.GetType() != Configuration::Token::STRING))
                {
                  lexer.ContextRestore();
                  val.SetType(Configuration::Token::UNKNOWN);
                }
              else
                lexer.ContextPop();
            } while (val.GetType() == Configuration::Token::STRING);
          log.flags = flags;
        }
      else if (var_str == "path")
        log.file = val_str;
      else if (var_str == "type")
        {
          if (val_str == "file")
            log.type = Configuration::Log::FILE;
          else if (val_str == "stderr")
            log.type = Configuration::Log::STDERR;
          else if (val_str == "stdout")
            log.type = Configuration::Log::STDOUT;
          else if (val_str == "syslog")
            log.type = Configuration::Log::SYSLOG;
        }
    }

  return ;
}



/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy constructor.
 *
 *  Parser is not copyable. Any attempt to use the copy constructor will result
 *  in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] parser Unused.
 */
Parser::Parser(const Parser& parser)
{
  (void)parser;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  Parser is not copyable. Any attempt to use the assignment operator will
 *  result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] parser Unused.
 *
 *  \return *this
 */
Parser& Parser::operator=(const Parser& parser)
{
  (void)parser;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor,
 */
Parser::Parser() {}

/**
 *  Destructor.
 */
Parser::~Parser() {}

/**
 *  \brief Parse a configuration file.
 *
 *  Parse the file filename and add objects to proper lists.
 *
 *  \param[in]  filename Path to the file to parse.
 *  \param[out] inputs   List of input configuration objects.
 *  \param[out] logs     List of log configuration objects.
 *  \param[out] outputs  List of output configuration objects.
 */
void Parser::Parse(const std::string& filename,
                   std::list<Interface>& inputs,
                   std::list<Log>& logs,
                   std::list<Interface>& outputs)
{
  std::auto_ptr<IO::File> filestream(new IO::File);
  std::string name;
  Configuration::Token val;
  Configuration::Token var;

  // Open configuration file.
  filestream->Open(filename.c_str(), IO::File::READ);

  // The lexer will split the configuration file into undividable tokens.
  Configuration::Lexer lexer(filestream.get());

  filestream.release();
  for (lexer.GetToken(var), lexer.GetToken(val);
       var.GetType() == Configuration::Token::STRING;
       lexer.GetToken(var), lexer.GetToken(val))
    {
      name.clear();
      switch (val.GetType())
        {
          // Assignment sign, we're setting a variable.
         case Configuration::Token::ASSIGNMENT:
          if (lexer.GetToken(val) || (val.GetType() != Configuration::Token::STRING))
            throw (Exception(0, INVALID_TOKEN_MSG));
          if (var.GetText() == "instance")
            gl_instance = val.GetText();
          // XXX : set global variable
          break ;
          // Block name.
         case Configuration::Token::STRING:
          name = val.GetText();
          if (lexer.GetToken(val) || val.GetType() != Configuration::Token::BLOCK_START)
           throw (Exception(0, INVALID_TOKEN_MSG));
          // Yes, no break, continue to BLOCK_START.
          // Starting a block, launching proper handler.
         case Configuration::Token::BLOCK_START:
          if (var.GetText() == "input")
            {
              Configuration::Interface in;

              HandleInterface(lexer, in);
              in.name = name;
              inputs.push_back(in);
            }
          else if (var.GetText() == "log")
            {
              Configuration::Log log;

              HandleLog(lexer, log);
              logs.push_back(log);
            }
          else if (var.GetText() == "output")
            {
              Configuration::Interface out;

              HandleInterface(lexer, out);
              out.name = name;
              outputs.push_back(out);
            }
          else
            throw (Exception(0, INVALID_TOKEN_MSG));
          break ;
          // Invalid token.
         default:
          throw (Exception(0, INVALID_TOKEN_MSG));
        };
    }

  // Assign failovers.
  std::list<Configuration::Interface>::iterator it;
  it = outputs.begin();
  while (it != outputs.end())
    {
      if (!it->failover.get() && !it->failover_name.empty())
        {
          std::list<Configuration::Interface>::iterator failover;

          // Find failover.
          for (failover = outputs.begin();
               failover != outputs.end();
               ++failover)
            if (failover->name == it->failover_name)
              break ;

          // Failover found. Store it in base object and remove it from list.
          if (failover != outputs.end())
            {
              it->failover.reset(new Configuration::Interface(*failover));
              outputs.erase(failover);
              it = outputs.begin();
            }
          else
            throw (Exception(0, "Could not find failover name."));
        }
      else
        ++it;
    }

  return ;
}
