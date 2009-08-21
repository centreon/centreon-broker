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

#include <cstring>
#include "client_acceptor.h"
#include "conf/input.h"
#include "conf/log.h"
#include "conf/output.h"
#include "conf/manager.h"
#include "db_output.h"
#include "io/net4.h"
#include "io/net6.h"
#include "io/unix.h"
#include "logging.h"

using namespace CentreonBroker;
using namespace CentreonBroker::Conf;

/**************************************
*                                     *
*          Static functions           *
*                                     *
**************************************/

/**
 *  Create a new ClientAcceptor object from its configuration.
 *
 *  \param[in] conf Configuration of the new ClientAcceptor.
 *
 *  \return A new configured ClientAcceptor.
 */
static ClientAcceptor* CreateInput(const Input& conf)
{
  ClientAcceptor* ca;
  IO::Acceptor* acceptor;
  IO::Net4Acceptor* n4a;
  IO::Net6Acceptor* n6a;
  IO::UnixAcceptor* ua;

  switch (conf.GetType())
    {
     case Input::IPV4:
      n4a = new IO::Net4Acceptor;
      if (conf.GetIPInterface().empty())
	n4a->Listen(conf.GetIPPort());
      else
	n4a->Listen(conf.GetIPPort(), conf.GetIPInterface().c_str());
      acceptor = n4a;
      break ;

     case Input::IPV6:
      n6a = new IO::Net6Acceptor;
      if (conf.GetIPInterface().empty())
	n6a->Listen(conf.GetIPPort());
      else
	n6a->Listen(conf.GetIPPort(), conf.GetIPInterface().c_str());
      acceptor = n6a;
      break ;

     case Input::UNIX:
      ua = new IO::UnixAcceptor;
      ua->Listen(conf.GetUnixSocketPath().c_str());
      acceptor = ua;
      break ;

     default:
       // XXX : throw exception or do something else
       ;
    }
  ca = new CentreonBroker::ClientAcceptor;
  ca->Run(acceptor);
  return (ca);
}

/**
 *  Create a new DBOutput object from its configuration.
 *
 *  \param[in] conf Configuration of the new DBOutput.
 *
 *  \return A new configured DBOutput.
 */
static DBOutput* CreateOutput(const Output& conf)
{
  DBOutput* dbo;

  switch (conf.GetType())
    {
#ifdef USE_MYSQL
     case Output::MYSQL:
      dbo = new DBOutput(DB::Connection::MYSQL);
      break ;
#endif /* USE_MYSQL */

#ifdef USE_ORACLE
     case Output::ORACLE:
      dbo = new DBOutput(DB::Connection::ORACLE);
      break ;
#endif /* USE_ORACLE */

#ifdef USE_POSTGRESQL
     case Output::POSTGRESQL:
      dbo = new DBOutput(DB::Connection::POSTGRESQL);
      break ;
#endif /* USE_POSTGRESQL */

     default:
      // XXX : throw exception or something
      ;
    }
  dbo->Init(conf.GetHost(), conf.GetUser(), conf.GetPassword(), conf.GetDB());
  return (dbo);
}

/**
 *  Process configuration options of an Input.
 *
 *  \param[in]  ifs Input stream.
 *  \param[out] in  Object that will be set with extracted parameters.
 */
static void HandleInput(std::ifstream& ifs, Input& in)
{
  char buffer[2048];

#ifndef NDEBUG
  logging.LogDebug("Input definition...");
#endif /* !NDEBUG */
  ifs.getline(buffer, sizeof(buffer));
  while (ifs.good())
    {
      char* key;
      char* value;

      key = buffer + strspn(buffer, " \t");
      if ('}' == key[0])
	break ;
      value = strchr(key, '=');
      if (value)
	{
	  *value = '\0';
	  value++;
	}

      if ('#' == key[0])
	; // Skip line
      else if (!strcmp(key, "interface"))
	in.SetIPInterface(value ? value : "");
      else if (!strcmp(key, "port"))
	in.SetIPPort(value ? strtoul(value, NULL, 0) : 0);
      else if (!strcmp(key, "socket"))
	in.SetUnixSocketPath(value ? value : "");
      else if (!strcmp(key, "type"))
	{
	  if (value)
	    {
	      if (!strcmp(value, "ipv4"))
		in.SetType(Input::IPV4);
	      else if (!strcmp(value, "ipv6"))
		in.SetType(Input::IPV6);
	      else if (!strcmp(value, "unix"))
		in.SetType(Input::UNIX);
	      else
		in.SetType(Input::UNKNOWN);
	    }
	  else
	    in.SetType(Input::UNKNOWN);
	}
      ifs.getline(buffer, sizeof(buffer));
    }
  return ;
}

/**
 *  Process configuration options of a Log.
 *
 *  \param[in]  ifs Input stream.
 *  \param[out] log Object that will be set with extracted parameters.
 */
static void HandleLog(std::ifstream& ifs, Log& log)
{
  char buffer[2048];

#ifndef NDEBUG
  logging.LogDebug("Log definition");
#endif /* !NDEBUG */
  ifs.getline(buffer, sizeof(buffer));
  while (ifs.good())
    {
      char* key;
      char* value;

      key = buffer + strspn(buffer, " \t");
      if ('}' == key[0])
	break ;
      value = strchr(key, '=');
      if (value)
	{
	  *value = '\0';
	  value++;
	}

      if ('#' == key[0])
	; // Skip line
      else if (!strcmp(key, "flags"))
	{
	  unsigned int flags;
	  char* val;

	  flags = 0;
	  val = strtok(value, " |");
	  while (val)
	    {
	      if (!strcmp("DEBUG", val))
		flags |= CentreonBroker::Logging::DEBUG;
	      else if (!strcmp("ERROR", val))
		flags |= CentreonBroker::Logging::ERROR;
	      else if (!strcmp("INFO", val))
		flags |= CentreonBroker::Logging::INFO;
	      val = strtok(NULL, " |");
	    }
	  log.SetFlags(flags);
	}
      else if (!strcmp(key, "path"))
	log.SetFilePath(value);
      else if (!strcmp(key, "type"))
	{
	  if (value)
	    {
	      if (!strcmp(value, "file"))
		log.SetType(Log::FILE);
	      else if (!strcmp(value, "stderr"))
		log.SetType(Log::STDERR);
	      else if (!strcmp(value, "stdout"))
		log.SetType(Log::STDOUT);
	      else if (!strcmp(value, "syslog"))
		log.SetType(Log::SYSLOG);
	    }
	  else
	    log.SetType(Log::UNKNOWN);
	}
      ifs.getline(buffer, sizeof(buffer));
    }
  return ;
}

/**
 *  Process configuration options of an Output.
 *
 *  \param[in]  ifs Input stream.
 *  \param[out] out Object that will be set with extracted parameters.
 */
static void HandleOutput(std::ifstream& ifs, Output& out)
{
  char buffer[2048];

#ifndef NDEBUG
  logging.LogDebug("Output definition");
#endif /* !NDEBUG */
  ifs.getline(buffer, sizeof(buffer));
  while (ifs.good())
    {
      char* key;
      char* value;

      key = buffer + strspn(buffer, " \t");
      if ('}' == key[0])
	break ;
      value = strchr(key, '=');
      if (value)
	{
	  *value = '\0';
	  value++;
	}

      if ('#' == key[0])
	; // Skip line
      else if (!strcmp(key, "db"))
	out.SetDB(value ? value : "");
      else if (!strcmp(key, "host"))
	out.SetHost(value ? value : "");
      else if (!strcmp(key, "password"))
	out.SetPassword(value ? value : "");
      else if (!strcmp(key, "type"))
	{
	  if (value)
	    {
	      if (!strcmp(value, "mysql"))
		out.SetType(Output::MYSQL);
	      else if (!strcmp(value, "oracle"))
		out.SetType(Output::ORACLE);
	      else if (!strcmp(value, "postgresql"))
		out.SetType(Output::POSTGRESQL);
	      else
		out.SetType(Output::UNKNOWN);
	    }
	  else
	    out.SetType(Output::UNKNOWN);
	}
      else if (!strcmp(key, "user"))
	out.SetUser(value ? value : "");
      ifs.getline(buffer, sizeof(buffer));
    }
  return ;
}

/**
 *  Register a new log output to the logging facility.
 *
 *  \param[in] conf Configuration of the new log output.
 */
static void RegisterLog(const Log& log)
{
  switch (log.GetType())
    {
     case Log::FILE:
      CentreonBroker::logging.LogInFile(log.GetFilePath().c_str(),
                                        log.GetFlags());
      break ;
     case Log::STDERR:
      CentreonBroker::logging.LogToStderr(log.GetFlags());
      break ;
     case Log::STDOUT:
      CentreonBroker::logging.LogToStdout(log.GetFlags());
      break ;
     case Log::SYSLOG:
      CentreonBroker::logging.LogInSyslog(log.GetFlags());
      break ;
    default:
      // XXX : throw exception or something
      ;
    }
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
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
 *  \brief Manager destructor.
 *
 *  Delete all objects created by the file parsing.
 */
Manager::~Manager()
{
  this->Close();
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
  std::ifstream ifs;

  ifs.open(filename.c_str());
  if (ifs)
    {
      char buffer[1024];

      ifs.getline(buffer, sizeof(buffer));
      while (ifs.good())
	{
	  char* cmd;
	  char* lasts;
	  char* name;

	  cmd = strtok_r(buffer, " \t", &lasts);
	  if (!cmd)
	    cmd = buffer; // empty string
	  name = strtok_r(NULL, " \t", &lasts);
	  if (buffer[0] == '#')
	    ;
	  else if (!strcmp(cmd, "log"))
	    {
	      Log log;

	      if (name)
		log.SetName(name);
	      HandleLog(ifs, log);
	      RegisterLog(log);
	      this->logs_.push_back(log);
	    }
	  else if (!strcmp(cmd, "input"))
	    {
	      Input in;

	      if (name)
		in.SetName(name);
	      HandleInput(ifs, in);
	      this->inputs_[in] = CreateInput(in);
	    }
	  else if (!strcmp(cmd, "output"))
	    {
	      Output out;

	      if (name)
		out.SetName(name);
	      HandleOutput(ifs, out);
	      this->outputs_[out] = CreateOutput(out);
	    }
	  ifs.getline(buffer, sizeof(buffer));
	}
      ifs.close();
    }
  else
    throw (CentreonBroker::Exception(0, "Could not load configuration file"));
  return ;
}

/**
 *  Update a previously opened configuration file.
 */
void Manager::Update()
{
}
