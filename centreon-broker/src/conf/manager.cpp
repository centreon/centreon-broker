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
#include <boost/thread/mutex.hpp>
#include <csignal>
#include <cstring>
#include "client_acceptor.h"
#include "conf/input.h"
#include "conf/log.h"
#include "conf/output.h"
#include "conf/manager.h"
#include "db_output.h"
#include "io/net4.h"
#include "io/net6.h"
#include "io/tls.h"
#include "io/unix.h"
#include "logging.h"

using namespace CentreonBroker;
using namespace CentreonBroker::Conf;

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
      else if (!strcmp(key, "ca"))
	in.SetTLSCA(value ? value : "");
      else if (!strcmp(key, "cert"))
	in.SetTLSCert(value ? value : "");
      else if (!strcmp(key, "compress"))
	in.SetTLSCompress(value ? strtoul(value, NULL, 0) : false);
      else if (!strcmp(key, "interface"))
	in.SetIPInterface(value ? value : "");
      else if (!strcmp(key, "key"))
	in.SetTLSKey(value ? value : "");
      else if (!strcmp(key, "port"))
	in.SetIPPort(value ? strtoul(value, NULL, 0) : 0);
      else if (!strcmp(key, "socket"))
	in.SetUnixSocketPath(value ? value : "");
      else if (!strcmp(key, "tls") && value)
	in.SetTLS(!strcmp(value, "yes") || strtoul(value, NULL, 0));
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
 *  Helper function to update configuration. This method is called whenever
 *  SIGUSR1 is received.
 */
static void UpdateHelper(int signum)
{
  (void)signum;
  CentreonBroker::logging.LogInfo("Configuration file update requested...");
  CentreonBroker::logging.LogInfo("  WARNING: this feature is still " \
                                  "experimental.");
  instance->Update();
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
  this->filename_ = filename;
  this->Update();
  return ;
}

/**
 *  Parse the configuration file and store resulting configuration in
 *  appropriate objects.
 *
 *  \param[out] inputs  Object where inputs configurations will be stored.
 *  \param[out] logs    Object where logs configurations will be stored.
 *  \param[out] outputs Object where outputs configurations will be stored.
 */
void Manager::Parse(std::list<Input>& inputs,
                    std::list<Log>& logs,
                    std::list<Output>& outputs)
{
  std::ifstream ifs;

  ifs.open(this->filename_.c_str());
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
	      logs.push_back(log);
	    }
	  else if (!strcmp(cmd, "input"))
	    {
	      Input in;

	      if (name)
		in.SetName(name);
	      HandleInput(ifs, in);
	      inputs.push_back(in);
	    }
	  else if (!strcmp(cmd, "output"))
	    {
	      Output out;

	      if (name)
		out.SetName(name);
	      HandleOutput(ifs, out);
	      outputs.push_back(out);
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
  std::list<Input> inputs;
  std::list<Input>::iterator inputs_it;
  std::list<Log> logs;
  std::list<Log>::iterator logs_it;
  std::list<Output> outputs;
  std::list<Output>::iterator outputs_it;

  this->Parse(inputs, logs, outputs);

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
      std::auto_ptr<CentreonBroker::DBOutput> dbo(
        new CentreonBroker::DBOutput(CentreonBroker::DB::Connection::MYSQL));
      const Output& output(*outputs_it);

#ifndef NDEBUG
      CentreonBroker::logging.LogDebug("Adding new output object...");
#endif /* !NDEBUG */
      dbo->SetConnectionRetryInterval(output.GetConnectionRetryInterval());
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

      // Create the new client acceptor
      std::auto_ptr<CentreonBroker::ClientAcceptor> ca(
        new CentreonBroker::ClientAcceptor());

      ca->Run(acceptor.get());
      acceptor.release();
      this->inputs_[*inputs_it] = ca.get();
      ca.release();
    }

  // (Re-)Register handler of SIGUSR1
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Registering handler for runtime " \
                                   "configuration update...");
#endif /* !NDEBUG */
  signal(SIGUSR1, UpdateHelper);

  return ;
}
