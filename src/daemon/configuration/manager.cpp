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

#include <algorithm>
#include <memory>
#include <signal.h>
#include <stdlib.h>                       // for strtoul
#include "concurrency/thread.h"
#include "configuration/lexer.h"
#include "configuration/interface.h"
#include "configuration/manager.h"
#include "db/mysql/connection.h"
#include "exception.h"
#include "interface/db/destination.h"
#include "io/acceptor.h"
#include "io/file.h"
#include "io/net/ipv4.h"
#include "io/net/ipv6.h"
#include "io/net/unix.h"
#include "logging.h"
#include "processing/high_availability.h"
#include "processing/listener.h"

#define INVALID_TOKEN_MSG "Invalid token encountered while parsing " \
                          "configuration file ..."

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

/**
 *  Get the instance of the configuration manager.
 *
 *  \return The instance of the configuration manager.
 */
Configuration::Manager& Configuration::Manager::Instance()
{
  static Configuration::Manager manager;

  return (manager);
}

/**
 *  Process configuration options of an Input.
 *
 *  \param[in]  lexer Lexer of the configuration file.
 *  \param[out] in    Object that will be set with extracted parameters.
 */
static void HandleInput(Configuration::Lexer& lexer, Configuration::Interface& in)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Input definition ...");
#endif /* !NDEBUG */
  Configuration::Token var;

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
      if (var_str == "interface")
	in.interface = val_str;
      else if (var_str == "port")
	in.port = strtoul(val_str.c_str(), NULL, 0);
      else if (var_str == "socket")
	in.socket = val_str;
      else if (var_str == "type")
	{
	  if ((val_str == "ip") || (val_str == "ipv4"))
	    in.type = Configuration::Interface::IPV4_SERVER;
	  else if (val_str == "ipv6")
	    in.type = Configuration::Interface::IPV6_SERVER;
	  else if (val_str == "unix")
	    in.type = Configuration::Interface::UNIX_SERVER;
	}
#ifdef USE_TLS
      else if (var_str == "ca")
	in.SetTLSCA(val_str);
      else if (var_str == "cert")
	in.SetTLSCert(val_str);
      else if (var_str == "compress")
	in.SetTLSCompress(strtoul(val_str.c_str(), NULL, 0));
      else if (var_str == "key")
	in.SetTLSKey(val_str);
      else if (var_str == "tls")
	in.SetTLS((val_str == "yes") || strtoul(val_str.c_str(), NULL, 0));
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
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Log definition ...");
#endif /* !NDEBUG */
  Configuration::Token var;

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
      if (var_str == "flags")
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

/**
 *  Process configuration options of an Output.
 *
 *  \param[in]  lexer Lexer of the configuration file.
 *  \param[out] out   Object that will be set with extracted parameters.
 */
static void HandleOutput(Configuration::Lexer& lexer, Configuration::Interface& out)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Output definition ...");
#endif /* !NDEBUG */
  Configuration::Token var;

  for (lexer.GetToken(var);
       var.GetType() != Configuration::Token::END && var.GetType() != Configuration::Token::BLOCK_END;
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
	out.db = val_str;
      else if (var_str == "dumpfile")
	; // XXX out.SetDumpFile(val_str);
      else if (var_str == "host")
	out.host = val_str;
      else if (var_str == "password")
	out.password = val_str;
      else if (var_str == "type")
	{
	  if (val_str == "mysql")
	    out.type = Configuration::Interface::MYSQL;
	  else if (val_str == "oracle")
	    out.type = Configuration::Interface::ORACLE;
	  else if (val_str == "postgresql")
	    out.type = Configuration::Interface::POSTGRESQL;
	}
      else if (var_str == "user")
	out.user = val_str;
    }
  return ;
}

/**
 *  Helper function to update configuration. This method is called whenever
 *  SIGHUP is received.
 */
static void UpdateHelper(int signum)
{
  (void)signum;
  signal(SIGHUP, SIG_IGN);
  CentreonBroker::logging.LogInfo("Configuration file update requested...");
  CentreonBroker::logging.LogInfo("  WARNING: this feature is still " \
                                  "experimental.");
  Configuration::Manager::Instance().Update();
  signal(SIGHUP, UpdateHelper);
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Manager default constructor.
 */
Configuration::Manager::Manager() {}

/**
 *  \brief Manager copy constructor.
 *
 *  Open the file stored within the given object, parse it and take actions as
 *  it would normally be done with Open().
 *
 *  \param[in] manager Object containing file to open again.
 *
 *  \see Open
 */
Configuration::Manager::Manager(const Configuration::Manager& manager)
{
  this->operator=(manager);
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Open the file stored within the given object, parse it and take actions as
 *  it would normally be done with Open().
 *
 *  \param[in] manager Object containin the file to open again.
 *
 *  \see Open
 */
Configuration::Manager& Configuration::Manager::operator=(const Configuration::Manager& manager)
{
  this->Open(manager.filename_);
  return (*this);
}

/**
 *  \brief Manager destructor.
 *
 *  Delete all objects created by the file parsing.
 */
Configuration::Manager::~Manager()
{
  this->Close();
}

/**
 *  Parse and analyze the configuration file and store resulting configuration
 *  in appropriate objects.
 *
 *  \param[out] inputs  Object where inputs configurations will be stored.
 *  \param[out] logs    Object where logs configurations will be stored.
 *  \param[out] outputs Object where outputs configurations will be stored.
 */
void Configuration::Manager::Analyze(std::list<Configuration::Interface>& inputs,
                                     std::list<Configuration::Log>& logs,
                                     std::list<Configuration::Interface>& outputs)
{
  std::auto_ptr<IO::File> filestream(new IO::File);
  Configuration::Token val;
  Configuration::Token var;

  filestream->Open(this->filename_.c_str(), IO::File::READ);

  Configuration::Lexer lexer(filestream.get());

  filestream.release();
  for (lexer.GetToken(var), lexer.GetToken(val);
       var.GetType() == Configuration::Token::STRING;
       lexer.GetToken(var), lexer.GetToken(val))
    {
      switch (val.GetType())
	{
	 // Assignment sign, we're setting a variable.
	case Configuration::Token::ASSIGNMENT:
	  if (lexer.GetToken(val) || (val.GetType() != Configuration::Token::STRING))
	    throw (Exception(0, INVALID_TOKEN_MSG));
	  // XXX : set global variable
	  break ;
	 // Block name, can safely be discarded.
	case Configuration::Token::STRING:
	   if (lexer.GetToken(val) || val.GetType() != Configuration::Token::BLOCK_START)
	    throw (Exception(0, INVALID_TOKEN_MSG));
	 // Starting a bloc, launching proper handler.
	case Configuration::Token::BLOCK_START:
          if (var.GetText() == "input")
	    {
	      Configuration::Interface in;

	      HandleInput(lexer, in);
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

	      HandleOutput(lexer, out);
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
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Delete all objects created by the file parsing.
 */
void Configuration::Manager::Close()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Closing configuration manager...");
  CentreonBroker::logging.LogDebug("Closing input objects...");
#endif /* !NDEBUG */
  for (std::map<Configuration::Interface, Concurrency::Thread*>::iterator
         it = this->inputs_.begin();
       it != this->inputs_.end();
       it++)
    delete (it->second);
  this->inputs_.clear();

#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Closing output objects...");
#endif /* !NDEBUG */
  for (std::map<Configuration::Interface, Processing::HighAvailability*>::iterator
         it = this->outputs_.begin();
       it != this->outputs_.end();
       it++)
    delete (it->second);
  this->outputs_.clear();

  // Does nothing of the log outputs. This way, they will remain valid until
  // the program exits.
  this->logs_.clear();

  return ;
}

/**
 *  Open a configuration file.
 *
 *  \param[in] filename Configuration file.
 */
void Configuration::Manager::Open(const std::string& filename)
{
  this->filename_ = filename;
  this->Update();
  return ;
}

/**
 *  Update a previously opened configuration file.
 */
void Configuration::Manager::Update()
{
  std::list<Configuration::Interface> inputs;
  std::list<Configuration::Interface>::iterator inputs_it;
  std::list<Configuration::Log> logs;
  std::list<Configuration::Log>::iterator logs_it;
  std::list<Configuration::Interface> outputs;
  std::list<Configuration::Interface>::iterator outputs_it;

  this->Analyze(inputs, logs, outputs);

  // Remove logs that are not present in conf anymore or which don't have the
  // same configuration.
  for (std::list<Configuration::Log>::iterator it = this->logs_.begin();
       it != this->logs_.end();)
    {
      logs_it = std::find(logs.begin(), logs.end(), *it);
      if (logs.end() == logs_it)
	{
#ifndef NDEBUG
	  CentreonBroker::logging.LogDebug("Removing unwanted log object...");
#endif /* !NDEBUG */
	  switch (it->type)
	    {
	    case Configuration::Log::FILE:
              CentreonBroker::logging.LogInFile(it->file.c_str(), 0);
              break ;
	    case Configuration::Log::STDERR:
	      CentreonBroker::logging.LogToStderr(0);
	      break ;
	    case Configuration::Log::STDOUT:
	      CentreonBroker::logging.LogToStdout(0);
	      break ;
	    case Configuration::Log::SYSLOG:
	      CentreonBroker::logging.LogInSyslog(0);
	      break ;
             default:
	      ;
	    }
	  this->logs_.erase(it++);
	}
      else
	{
	  logs.erase(logs_it);
	  it++;
	}
    }

  // Add new logs.
  for (logs_it = logs.begin(); logs_it != logs.end(); logs_it++)
    {
#ifndef NDEBUG
      CentreonBroker::logging.LogDebug("Adding new logging object...");
#endif /* !NDEBUG */
      switch (logs_it->type)
	{
	case Configuration::Log::FILE:
	  CentreonBroker::logging.LogInFile(logs_it->file.c_str(),
                                            logs_it->flags);
	  break ;
	case Configuration::Log::STDERR:
	  CentreonBroker::logging.LogToStderr(logs_it->flags);
	  break ;
	case Configuration::Log::STDOUT:
	  CentreonBroker::logging.LogToStdout(logs_it->flags);
	  break ;
	case Configuration::Log::SYSLOG:
	  CentreonBroker::logging.LogInSyslog(logs_it->flags);
	  break ;
         default:
	  ;
	}
      this->logs_.push_back(*logs_it);
    }

  // Remove outputs that are not present in conf anymore or which don't have
  // the same configuration.
  for (std::map<Configuration::Interface, Processing::HighAvailability*>::iterator it = this->outputs_.begin();
       it != this->outputs_.end();)
    {
      outputs_it = std::find(outputs.begin(), outputs.end(), it->first);
      if (outputs.end() == outputs_it)
	{
#ifndef NDEBUG
	  CentreonBroker::logging.LogDebug("Removing unwanted output object...");
#endif /* !NDEBUG */
	  delete (it->second);
	  this->outputs_.erase(it++);
	}
      else
	{
	  outputs.erase(outputs_it);
	  it++;
	}
    }

  // Add new outputs.
  for (outputs_it = outputs.begin(); outputs_it != outputs.end(); outputs_it++)
    {
      std::auto_ptr<CentreonBroker::DB::Connection> dbc;
      std::auto_ptr< ::Interface::DB::Destination > dbd;
      std::auto_ptr<Processing::HighAvailability> ha;
      const Configuration::Interface& output(*outputs_it);

#ifndef NDEBUG
      CentreonBroker::logging.LogDebug("Adding new output object...");
#endif /* !NDEBUG */

#ifdef USE_MYSQL
      if (Configuration::Interface::MYSQL == output.type)
	dbc.reset(new CentreonBroker::DB::MySQLConnection());
#endif /* USE_MYSQL */

#ifdef USE_ORACLE
# ifdef USE_MYSQL
      else
# endif /* USE_MYSQL */
      if (Output::ORACLE == output.GetType())
        dbo.reset(new DBOutput(DB::Connection::ORACLE));
#endif /* USE_ORACLE */

#ifdef USE_POSTGRESQL
# if defined(USE_MYSQL) || defined(USE_ORACLE)
      else
# endif /* USE_MYSQL || USE_ORACLE */
      if (Output::POSTGRESQL == output.GetType())
	dbo.reset(new DBOutput(DB::Connection::POSTGRESQL));
#endif /* USE_POSTGRESQL */

      /*
      dbo->SetConnectionRetryInterval(output.GetConnectionRetryInterval());
      dbo->SetDumpFile(output.GetDumpFile());
      dbo->SetQueryCommitInterval(output.GetQueryCommitInterval());
      dbo->SetTimeCommitInterval(output.GetTimeCommitInterval());
      dbo->Init(output.GetHost(),
                output.GetUser(),
                output.GetPassword(),
                output.GetDB());
      */
      // XXX init
      dbd.reset(new ::Interface::DB::Destination);
      dbd->Init(dbc.get());
      dbc.release();
      ha.reset(new Processing::HighAvailability);
      ha->Init(dbd.release());
      ha.release();
      this->outputs_[output] = ha.get();
      ha.release();
    }

  // Remove inputs that are not present in conf anymore or which don't have the
  // same configuration.
  for (std::map<Interface, Concurrency::Thread*>::iterator
         it = this->inputs_.begin();
       it != this->inputs_.end();)
    {
      inputs_it = std::find(inputs.begin(), inputs.end(), it->first);
      if (inputs.end() == inputs_it)
	{
#ifndef NDEBUG
	  CentreonBroker::logging.LogDebug("Removing unwanted input object...");
#endif /* !NDEBUG */
	  delete (it->second);
	  this->inputs_.erase(it++);
	}
      else
	{
	  inputs.erase(inputs_it);
	  it++;
	}
    }

  // Add new inputs.
  for (inputs_it = inputs.begin(); inputs_it != inputs.end(); inputs_it++)
    {
#ifndef NDEBUG
      CentreonBroker::logging.LogDebug("Adding new input object...");
#endif /* !NDEBUG */
      std::auto_ptr<IO::Acceptor> acceptor;

      switch (inputs_it->type)
	{
	case Configuration::Interface::IPV4_SERVER:
	  {
	    std::auto_ptr<IO::Net::IPv4Acceptor> net4a(
	       new IO::Net::IPv4Acceptor());

	    if (inputs_it->interface.empty())
	      net4a->Listen(inputs_it->port);
	    else
	      net4a->Listen(inputs_it->port,
                            inputs_it->interface.c_str());
	    acceptor.reset(net4a.get());
	    net4a.release();
	  }
	  break ;
	case Configuration::Interface::IPV6_SERVER:
	  {
	    std::auto_ptr<IO::Net::IPv6Acceptor> net6a(
	       new IO::Net::IPv6Acceptor());

	    if (inputs_it->interface.empty())
	      net6a->Listen(inputs_it->port);
	    else
	      net6a->Listen(inputs_it->port,
                               inputs_it->interface.c_str());
	    acceptor.reset(net6a.get());
	    net6a.release();
	  }
	  break ;
	case Configuration::Interface::UNIX_SERVER:
	  {
	    std::auto_ptr<IO::Net::UnixAcceptor> unixa(
	       new IO::Net::UnixAcceptor());

	    unixa->Listen(inputs_it->socket.c_str());
	    acceptor.reset(unixa.get());
	    unixa.release();
	  }
	  break ;
         default:
	  ;
	}

#ifdef USE_TLS
      // Check for TLS support
      if (((Input::IPV4_SERVER == inputs_it->GetType())
	   || (Input::IPV6_SERVER == inputs_it->GetType())
           || (Input::UNIX_SERVER == inputs_it->GetType()))
          && inputs_it->GetTLS())
	{
	  std::auto_ptr<CentreonBroker::IO::TLSAcceptor> tlsa(
            new CentreonBroker::IO::TLSAcceptor());

	  if (!inputs_it->GetTLSCert().empty()
              && !inputs_it->GetTLSKey().empty())
	    tlsa->SetCert(inputs_it->GetTLSCert().c_str(),
                          inputs_it->GetTLSKey().c_str());
	  if (!inputs_it->GetTLSCA().empty())
	    tlsa->SetTrustedCA(inputs_it->GetTLSCA().c_str());
	  tlsa->SetCompression(inputs_it->GetTLSCompress());
	  tlsa->Listen(acceptor.get());
	  acceptor.release();
	  acceptor.reset(tlsa.get());
	  tlsa.release();
	}
#endif /* USE_TLS */

      // Create the new client acceptor
      std::auto_ptr<Processing::Listener> listener(
        new Processing::Listener());

      listener->Init(acceptor.get(), Processing::Listener::NDO);
      acceptor.release();
      this->inputs_[*inputs_it] = listener.get();
      listener.release();
    }

  // (Re-)Register handler of SIGHUP
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Registering handler for runtime " \
                                   "configuration update...");
#endif /* !NDEBUG */
  signal(SIGHUP, UpdateHelper);

  return ;
}
