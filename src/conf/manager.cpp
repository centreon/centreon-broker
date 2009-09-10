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

#include <boost/thread/mutex.hpp>
#include <csignal>
#include <cstdlib>
#include <memory>
#include "client_acceptor.h"
#include "conf/lexer.h"
#include "conf/manager.h"
#include "db_output.h"
#include "exception.h"
#include "io/file.h"
#include "io/io.h"
#include "io/net4.h"
#include "io/net6.h"
#ifdef USE_TLS
# include "io/tls.h"
#endif /* USE_TLS */
#include "io/unix.h"
#include "logging.h"

using namespace CentreonBroker;
using namespace CentreonBroker::Conf;

#define INVALID_TOKEN_MSG "Invalid token encountered while parsing " \
                          "configuration file ..."

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

/**
 *  The configuration manager pointer and its associated mutex.
 */
static std::auto_ptr<Manager> instance;
static boost::mutex           instancem;

/**
 *  Get or build the instance of the configuration manager.
 *
 *  \return The potentially newly created configuration manager.
 */
Manager& Manager::GetInstance()
{
  // This is the double lock pattern. It's not 100% safe, but it seems to have
  // the best safeness/performance ratio.
  if (!instance.get())
    {
      boost::unique_lock<boost::mutex> lock(instancem);

      if (!instance.get())
	instance.reset(new Manager());
    }
  return (*instance);
}

/**
 *  Process configuration options of an Input.
 *
 *  \param[in]  lexer Lexer of the configuration file.
 *  \param[out] in    Object that will be set with extracted parameters.
 */
static void HandleInput(Lexer& lexer, Input& in)
{
#ifndef NDEBUG
  logging.LogDebug("Input definition ...");
#endif /* !NDEBUG */
  Token var;

  for (lexer.GetToken(var);
       var.GetType() == Token::STRING;
       lexer.GetToken(var))
    {
      Token val;

      // Check if the current token is valid (var name).
      if (var.GetType() != Token::STRING
          // And the next one too (=).
          || lexer.GetToken(val) || (val.GetType() != Token::ASSIGNMENT)
          // And the next-next one too (var value).
	  || lexer.GetToken(val) || (val.GetType() != Token::STRING))
	throw (Exception(0, INVALID_TOKEN_MSG));

      // Extract var strings.
      const std::string& var_str = var.GetText();
      const std::string& val_str = val.GetText();

      // Parse variable.
      if (var_str == "interface")
	in.SetIPInterface(val_str);
      else if (var_str == "port")
	in.SetIPPort(strtoul(val_str.c_str(), NULL, 0));
      else if (var_str == "socket")
	in.SetUnixSocketPath(val_str);
      else if (var_str == "type")
	{
	  if ((val_str == "ip") || (val_str == "ipv4"))
	    in.SetType(Input::IPV4);
	  else if (val_str == "ipv6")
	    in.SetType(Input::IPV6);
	  else if (val_str == "unix")
	    in.SetType(Input::UNIX);
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
static void HandleLog(Lexer& lexer, Log& log)
{
#ifndef NDEBUG
  logging.LogDebug("Log definition ...");
#endif /* !NDEBUG */
  Token var;

  for (lexer.GetToken(var);
       var.GetType() != Token::END && var.GetType() != Token::BLOCK_END;
       lexer.GetToken(var))
    {
      Token val;

      // Check if the current token is valid (var name).
      if ((var.GetType() != Token::STRING)
          // And the next one too (=).
	  || lexer.GetToken(val) || (val.GetType() != Token::ASSIGNMENT)
	  // And the next-next one too (var value).
	  || lexer.GetToken(val) || (val.GetType() != Token::STRING))
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
                  || (val.GetType() != Token::PIPE)
		  || lexer.GetToken(val)
                  || (val.GetType() != Token::STRING))
		{
		  lexer.ContextRestore();
		  val.SetType(Token::UNKNOWN);
		}
	      else
		lexer.ContextPop();
	    } while (val.GetType() == Token::STRING);
	  log.SetFlags(flags);
	}
      else if (var_str == "path")
	log.SetFilePath(val_str);
      else if (var_str == "type")
	{
	  if (val_str == "file")
	    log.SetType(Log::FILE);
	  else if (val_str == "stderr")
	    log.SetType(Log::STDERR);
	  else if (val_str == "stdout")
	    log.SetType(Log::STDOUT);
	  else if (val_str == "syslog")
	    log.SetType(Log::SYSLOG);
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
static void HandleOutput(Lexer& lexer, Output& out)
{
#ifndef NDEBUG
  logging.LogDebug("Output definition ...");
#endif /* !NDEBUG */
  Token var;

  for (lexer.GetToken(var);
       var.GetType() != Token::END && var.GetType() != Token::BLOCK_END;
       lexer.GetToken(var))
    {
      Token val;

      // Check if the current token is valid (var name).
      if (var.GetType() != Token::STRING
	  // And the next one too (=).
	  || lexer.GetToken(val) || (val.GetType() != Token::ASSIGNMENT)
	  // And the next-next one too (var value).
	  || lexer.GetToken(val) || (val.GetType() != Token::STRING))
	throw (Exception(0, INVALID_TOKEN_MSG));

      // Extract var strings.
      const std::string& var_str = var.GetText();
      const std::string& val_str = val.GetText();

      // Parse variable.
      if (var_str == "db")
	out.SetDB(val_str);
      else if (var_str == "dumpfile")
	out.SetDumpFile(val_str);
      else if (var_str == "host")
	out.SetHost(val_str);
      else if (var_str == "password")
	out.SetPassword(val_str);
      else if (var_str == "type")
	{
	  if (val_str == "mysql")
	    out.SetType(Output::MYSQL);
	  else if (val_str == "oracle")
	    out.SetType(Output::ORACLE);
	  else if (val_str == "postgresql")
	    out.SetType(Output::POSTGRESQL);
	}
      else if (var_str == "user")
	out.SetUser(val_str);
    }
  return ;
}

/**
 *  Helper function to update configuration. This method is called whenever
 *  SIGHUP is received.
 */
static void UpdateHelper(int signum) throw ()
{
  (void)signum;
  signal(SIGHUP, SIG_IGN);
  CentreonBroker::logging.LogInfo("Configuration file update requested...");
  CentreonBroker::logging.LogInfo("  WARNING: this feature is still " \
                                  "experimental.");
  instance->Update();
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
Manager::Manager() {}

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
Manager::Manager(const Manager& manager)
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
Manager& Manager::operator=(const Manager& manager)
{
  this->Open(manager.filename_);
  return (*this);
}

/**
 *  Parse and analyze the configuration file and store resulting configuration
 *  in appropriate objects.
 *
 *  \param[out] inputs  Object where inputs configurations will be stored.
 *  \param[out] logs    Object where logs configurations will be stored.
 *  \param[out] outputs Object where outputs configurations will be stored.
 */
void Manager::Analyze(std::list<Input>& inputs,
                      std::list<Log>& logs,
                      std::list<Output>& outputs)
{
  std::auto_ptr<IO::FileStream> filestream(
    new IO::FileStream(this->filename_, IO::FileStream::READ));
  Token val;
  Token var;
  Lexer lexer(filestream.get());

  filestream.release();
  for (lexer.GetToken(var), lexer.GetToken(val);
       var.GetType() == Token::STRING;
       lexer.GetToken(var), lexer.GetToken(val))
    {
      switch (val.GetType())
	{
	 // Assignment sign, we're setting a variable.
         case Token::ASSIGNMENT:
	  if (lexer.GetToken(val) || (val.GetType() != Token::STRING))
	    throw (Exception(0, INVALID_TOKEN_MSG));
	  // XXX : set global variable
	  break ;
	 // Block name, can safely be discarded.
         case Token::STRING:
	  if (lexer.GetToken(val) || val.GetType() != Token::BLOCK_START)
	    throw (Exception(0, INVALID_TOKEN_MSG));
	 // Starting a bloc, launching proper handler.
         case Token::BLOCK_START:
          if (var.GetText() == "input")
	    {
	      Input in;

	      HandleInput(lexer, in);
	      inputs.push_back(in);
	    }
	  else if (var.GetText() == "log")
	    {
	      Log log;

	      HandleLog(lexer, log);
	      logs.push_back(log);
	    }
	  else if (var.GetText() == "output")
	    {
	      Output out;

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
 *  \brief Manager destructor.
 *
 *  Delete all objects created by the file parsing.
 */
Manager::~Manager()
{
  this->Close();
}

/**
 *  Delete all objects created by the file parsing.
 */
void Manager::Close()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Closing configuration manager...");
  CentreonBroker::logging.LogDebug("Closing input objects...");
#endif /* !NDEBUG */
  for (std::map<Input, CentreonBroker::ClientAcceptor*>::iterator
         it = this->inputs_.begin();
       it != this->inputs_.end();
       it++)
    delete (it->second);
  this->inputs_.clear();

#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Closing output objects...");
#endif /* !NDEBUG */
  for (std::map<Output, CentreonBroker::DBOutput*>::iterator
         it = this->outputs_.begin();
       it != this->outputs_.end();
       it++)
    {
      it->second->Destroy();
      delete (it->second);
    }
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
void Manager::Open(const std::string& filename)
{
  this->filename_ = filename;
  this->Update();
  return ;
}

/**
 *  Update a previously opened configuration file.
 */
void Manager::Update() throw ()
{
  std::list<Input> inputs;
  std::list<Input>::iterator inputs_it;
  std::list<Log> logs;
  std::list<Log>::iterator logs_it;
  std::list<Output> outputs;
  std::list<Output>::iterator outputs_it;

  this->Analyze(inputs, logs, outputs);

  // Remove logs that are not present in conf anymore or which don't have the
  // same configuration.
  for (std::list<Log>::iterator it = this->logs_.begin();
       it != this->logs_.end();)
    {
      logs_it = std::find(logs.begin(), logs.end(), *it);
      if (logs.end() == logs_it)
	{
#ifndef NDEBUG
	  CentreonBroker::logging.LogDebug("Removing unwanted log object...");
#endif /* !NDEBUG */
	  switch (it->GetType())
	    {
             case Log::FILE:
              CentreonBroker::logging.LogInFile(it->GetFilePath().c_str(), 0);
              break ;
             case Log::STDERR:
	      CentreonBroker::logging.LogToStderr(0);
	      break ;
             case Log::STDOUT:
	      CentreonBroker::logging.LogToStdout(0);
	      break ;
             case Log::SYSLOG:
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
      switch (logs_it->GetType())
	{
         case Log::FILE:
	  CentreonBroker::logging.LogInFile(logs_it->GetFilePath().c_str(),
                                            logs_it->GetFlags());
	  break ;
         case Log::STDERR:
	  CentreonBroker::logging.LogToStderr(logs_it->GetFlags());
	  break ;
         case Log::STDOUT:
	  CentreonBroker::logging.LogToStdout(logs_it->GetFlags());
	  break ;
         case Log::SYSLOG:
	  CentreonBroker::logging.LogInSyslog(logs_it->GetFlags());
	  break ;
         default:
	  ;
	}
      this->logs_.push_back(*logs_it);
    }

  // Remove outputs that are not present in conf anymore or which don't have
  // the same configuration.
  for (std::map<Output, DBOutput*>::iterator it = this->outputs_.begin();
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
      std::auto_ptr<CentreonBroker::DBOutput> dbo;
      const Output& output(*outputs_it);

#ifndef NDEBUG
      CentreonBroker::logging.LogDebug("Adding new output object...");
#endif /* !NDEBUG */

#ifdef USE_MYSQL
      if (Output::MYSQL == output.GetType())
	dbo.reset(new DBOutput(DB::Connection::MYSQL));
      else
#endif /* USE_MYSQL */

#ifdef USE_ORACLE
      if (Output::ORACLE == output.GetType())
        dbo.reset(new DBOutput(DB::Connection::ORACLE));
      else
#endif /* USE_ORACLE */

#ifdef USE_POSTGRESQL
      if (Output::POSTGRESQL == output.GetType())
	dbo.reset(new DBOutput(DB::Connection::POSTGRESQL));
#endif /* USE_POSTGRESQL */

      dbo->SetConnectionRetryInterval(output.GetConnectionRetryInterval());
      dbo->SetDumpFile(output.GetDumpFile());
      dbo->SetQueryCommitInterval(output.GetQueryCommitInterval());
      dbo->SetTimeCommitInterval(output.GetTimeCommitInterval());
      dbo->Init(output.GetHost(),
                output.GetUser(),
                output.GetPassword(),
                output.GetDB());
      this->outputs_[output] = dbo.get();
      dbo.release();
    }

  // Remove inputs that are not present in conf anymore or which don't have the
  // same configuration.
  for (std::map<Input, CentreonBroker::ClientAcceptor*>::iterator
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
      std::auto_ptr<CentreonBroker::IO::Acceptor> acceptor;

      switch (inputs_it->GetType())
	{
         case Input::IPV4:
	  {
	    std::auto_ptr<CentreonBroker::IO::Net4Acceptor> net4a(
              new CentreonBroker::IO::Net4Acceptor());

	    if (inputs_it->GetIPInterface().empty())
	      net4a->Listen(inputs_it->GetIPPort());
	    else
	      net4a->Listen(inputs_it->GetIPPort(),
                            inputs_it->GetIPInterface().c_str());
	    acceptor.reset(net4a.get());
	    net4a.release();
	  }
	  break ;
         case Input::IPV6:
	  {
	    std::auto_ptr<CentreonBroker::IO::Net6Acceptor> net6a(
              new CentreonBroker::IO::Net6Acceptor());

	    if (inputs_it->GetIPInterface().empty())
	      net6a->Listen(inputs_it->GetIPPort());
	    else
	      net6a->Listen(inputs_it->GetIPPort(),
                               inputs_it->GetIPInterface().c_str());
	    acceptor.reset(net6a.get());
	    net6a.release();
	  }
	  break ;
         case Input::UNIX:
	  {
	    std::auto_ptr<CentreonBroker::IO::UnixAcceptor> unixa(
              new CentreonBroker::IO::UnixAcceptor());

	    unixa->Listen(inputs_it->GetUnixSocketPath().c_str());
	    acceptor.reset(unixa.get());
	    unixa.release();
	  }
	  break ;
         default:
	  ;
	}

#ifdef USE_TLS
      // Check for TLS support
      if (((Input::IPV4 == inputs_it->GetType())
	   || (Input::IPV6 == inputs_it->GetType())
           || (Input::UNIX == inputs_it->GetType()))
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
      std::auto_ptr<CentreonBroker::ClientAcceptor> ca(
        new CentreonBroker::ClientAcceptor());

      ca->Run(acceptor.get());
      acceptor.release();
      this->inputs_[*inputs_it] = ca.get();
      ca.release();
    }

  // (Re-)Register handler of SIGHUP
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Registering handler for runtime " \
                                   "configuration update...");
#endif /* !NDEBUG */
  signal(SIGHUP, UpdateHelper);

  return ;
}
