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
#include <stdlib.h>                   // for strtoul
#include <unistd.h>                   // for sleep
#include "concurrency/lock.h"
#include "concurrency/thread.h"
#include "configuration/interface.h"
#include "configuration/manager.h"
#include "configuration/parser.h"
#include "exception.h"
#include "interface/db/destination.h"
#include "interface/factory.h"
#include "io/acceptor.h"
#include "logging.h"
#include "multiplexing/publisher.h"
#include "multiplexing/subscriber.h"
#include "processing/failover_out.h"
#include "processing/feeder.h"
#include "processing/listener.h"

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
 *  Helper function to update configuration. This method is called whenever
 *  SIGHUP is received.
 */
static void UpdateHelper(int signum)
{
  (void)signum;
  signal(SIGHUP, SIG_IGN);
  LOGINFO("Configuration file update requested...");
  LOGINFO("  WARNING: this feature is still experimental.");
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
Configuration::Manager::Manager()
{
  // (Re-)Register handler of SIGHUP
  LOGDEBUG("Registering handler for runtime configuration update ...");
  signal(SIGHUP, UpdateHelper);
}

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
  : Concurrency::ThreadListener()
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
  Concurrency::Lock lock(this->mutex_);

  LOGDEBUG("Closing configuration manager...");

  // Ask input threads to quit ASAP.
  LOGDEBUG("Closing input objects ...");
  for (std::map<Configuration::Interface, Concurrency::Thread*>::iterator
         it = this->inputs_.begin();
       it != this->inputs_.end();
       ++it)
    it->second->Exit();

  LOGDEBUG("Closing output objects ...");
  for (std::map<Configuration::Interface, Processing::Feeder*>::iterator
         it = this->outputs_.begin();
       it != this->outputs_.end();
       ++it)
    it->second->Exit();

  LOGDEBUG("Shuting down remaining threads ...");
  for (std::list<Concurrency::Thread*>::iterator
         it = this->spontaneous_.begin();
       it != this->spontaneous_.end();
       ++it)
    (*it)->Exit();

  // Does nothing of the log outputs. This way, they will remain valid until
  // the program exits.
  this->logs_.clear();

  // Wait for all threads to have exited.
  LOGDEBUG("Waiting for threads termination ...");
  lock.Release();
  this->mutex_.Lock();
  while (!this->inputs_.empty()
         || !this->outputs_.empty()
         || !this->spontaneous_.empty())
    {
      this->mutex_.Unlock();
      sleep(1);
      this->mutex_.Lock();
    }
  this->mutex_.Unlock();

  // Reap them all.
  LOGDEBUG("Reaping last objects ...");
  this->Reap();

  return ;
}

/**
 *  Callback called when a new thread has just been created.
 *
 *  \param[in] thread Handle of the newly created thread.
 */
void Configuration::Manager::OnCreate(Concurrency::Thread* thread)
{
  Concurrency::Lock lock(this->mutex_);

  // Search for thread handle in input list.
  for (std::map<Interface, Concurrency::Thread*>::iterator
         it = this->inputs_.begin();
       it != this->inputs_.end();
       ++it)
    if (it->second == thread)
      return ;

  // Search for thread handle in output list.
  for (std::map<Interface, Processing::Feeder*>::iterator
	 it = this->outputs_.begin();
       it != this->outputs_.end();
       ++it)
    if (it->second == thread)
      return ;

  // Not found, store it as spontaneous.
  this->spontaneous_.push_back(thread);

  return ;
}

/**
 *  Callback called when one of the launched thread is about to exit.
 *
 *  \param[in] thread Handle of the thread about to exit.
 */
void Configuration::Manager::OnExit(Concurrency::Thread* thread)
{
  Concurrency::Lock lock(this->mutex_);

  // Search for thread handle in input list.
  for (std::map<Interface, Concurrency::Thread*>::iterator
         it = this->inputs_.begin();
       it != this->inputs_.end();
       ++it)
    if (it->second == thread)
      {
	this->to_reap_.push_back(thread);
	this->inputs_.erase(it);
	return ;
      }

  // Search for thread handle in output list.
  for (std::map<Interface, Processing::Feeder*>::iterator
	 it = this->outputs_.begin();
       it != this->outputs_.end();
       ++it)
    if (it->second == thread)
      {
	this->to_reap_.push_back(thread);
	this->outputs_.erase(it);
        return ;
      }

  // Search for thread handle in spontaneous list.
  std::list<Concurrency::Thread*>::iterator it;

  it = std::find(this->spontaneous_.begin(), this->spontaneous_.end(), thread);
  if (it != this->spontaneous_.end())
    {
      this->to_reap_.push_back(thread);
      this->spontaneous_.erase(it);
    }

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
 *  Terminated thread are stored within a list, waiting for cleanup.
 */
void Configuration::Manager::Reap()
{
  while (1) // Condition is within the loop.
    {
      Concurrency::Lock lock(this->mutex_);

      // Check that there are some threads to reap.
      if (this->to_reap_.empty())
	break ;

      // Delete current thread.
      std::auto_ptr<Concurrency::Thread> thread(this->to_reap_.front());

      this->to_reap_.pop_front();
      lock.Release();
    }

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
  Concurrency::Lock lock(this->mutex_);

  // Parse configuration file.
  {
    Parser parser;

    parser.Parse(this->filename_, inputs, logs, outputs);
  }

  // Remove logs that are not present in conf anymore or which don't have the
  // same configuration.
  for (std::list<Configuration::Log>::iterator it = this->logs_.begin();
       it != this->logs_.end();)
    {
      logs_it = std::find(logs.begin(), logs.end(), *it);
      if (logs.end() == logs_it)
        {
          LOGDEBUG("Removing unwanted log object...");
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
  for (logs_it = logs.begin(); logs_it != logs.end(); ++logs_it)
    {
      LOGDEBUG("Adding new logging object...");
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
  for (std::map<Configuration::Interface, Processing::Feeder*>::iterator it = this->outputs_.begin();
       it != this->outputs_.end();)
    {
      outputs_it = std::find(outputs.begin(), outputs.end(), it->first);
      if (outputs.end() == outputs_it)
        {
          LOGDEBUG("Removing unwanted output object...");
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
  for (outputs_it = outputs.begin(); outputs_it != outputs.end(); ++outputs_it)
    {
      std::auto_ptr<Processing::FailoverOut> feeder;
      std::auto_ptr<Multiplexing::Subscriber> subscriber;

      LOGDEBUG("Adding new output object...");
      subscriber.reset(new Multiplexing::Subscriber);
      feeder.reset(new Processing::FailoverOut);
      // XXX
      feeder->Run(subscriber.get(),
                  *outputs_it);
      subscriber.release();
      this->outputs_[*outputs_it] = feeder.get();
      feeder.release();
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
          LOGDEBUG("Removing unwanted input object...");
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
  for (inputs_it = inputs.begin(); inputs_it != inputs.end(); ++inputs_it)
    {
      LOGDEBUG("Adding new input object...");
      std::auto_ptr<IO::Acceptor> acceptor(
        ::Interface::Factory::Instance().Acceptor(*inputs_it));

      // Create the new client acceptor
      std::auto_ptr<Processing::Listener> listener(
        new Processing::Listener());

      listener->Init(acceptor.get(), Processing::Listener::NDO, this);
      acceptor.release();
      this->inputs_[*inputs_it] = listener.get();
      listener.release();
    }

  return ;
}
