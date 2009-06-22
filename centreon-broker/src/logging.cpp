/*
** logging.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/12/09 Matthieu Kermagoret
** Last update 06/22/09 Matthieu Kermagoret
*/

#include <cassert>
#include <syslog.h>
#include "logging.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Global Variables          *
*                                     *
**************************************/

Logging CentreonBroker::logging;


/******************************************************************************
*                                                                             *
*                                                                             *
*                                Output                                       *
*                                                                             *
*                                                                             *
*  This private class within Logging will contain an handle to a file. We     *
*  will define a destructor, copy constructor and operator= so that           *
*  ressources are released properly.                                          *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Clean the current object (ie. close wrapped file if necessary).
 */
void Logging::Output::Clean()
{
  if (this->stream_)
    {
      if (this->stream_->is_open())
	this->stream_->close();
      delete (this->stream_);
      this->stream_ = NULL;
    }
}

/**
 *  Make a copy of internal parameters. The parameters hold in the output
 *  variable will be transfered to the current instance and discarded in
 *  output.
 */
void Logging::Output::InternalCopy(const Logging::Output& output)
{
  Output* o;

  o = const_cast<Output*>(&output);
  this->filename_ = o->filename_;
  o->filename_.clear();
  this->flags_ = o->flags_;
  o->flags_ = 0;
  this->stream_ = o->stream_;
  o->stream_ = NULL;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Output default constructor (does nothing).
 */
Logging::Output::Output() : flags_(0), stream_(NULL)
{
}

/**
 *  Output copy constructor. The parameters hold in output will be transfered
 *  to the current instance and discarded in output.
 */
Logging::Output::Output(const Logging::Output& output)
{
  this->InternalCopy(output);
}

/**
 *  Output destructor. Will close the wrapped file if open.
 */
Logging::Output::~Output() throw ()
{
  try
    {
      this->Clean();
    }
  catch (...)
    {
    }
}

/**
 *  Output operator= overload. The parameters hold in output will be transfered
 *  to the current instance and discarded in output.
 */
Logging::Output& Logging::Output::operator=(const Logging::Output& output)
{
  this->Clean();
  this->InternalCopy(output);
  return (*this);
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                Logging                                      *
*                                                                             *
*                                                                             *
*  This class will be used to log informations about the running process.     *
*  Every information can be classified in one of three categories : debug,    *
*  info or error. This logging facility can forward messages from             *
*  application to one or more outputs (file, syslog, ...).                    *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Logging copy constructor (shouldn't be used).
 */
Logging::Logging(const Logging& l)
{
  (void)l;
  assert(false);
}

/**
 *  Logging operator= overload.
 */
Logging& Logging::operator=(const Logging& l)
{
  (void)l;
  assert(false);
  return (*this);
}

/**
 *  Log a message into the appropriate outputs.
 */
void Logging::LogBase(const char* str, int flags, bool indent) throw ()
{
  try
    {
      int idnt;
      boost::unique_lock<boost::mutex> lock(this->mutex_);
      boost::thread::id self;

      self = boost::this_thread::get_id();
      idnt = (*this->indent_.find(self)).second;

      for (std::vector<Output>::iterator ito = this->outputs_.begin();
	   ito != this->outputs_.end();
	   ito++)
	if ((*ito).flags_ & flags)
	  {
	    std::ostream* os;

	    os = (*ito).stream_;
	    *os << '[' << self << "] ";
	    for (int i = 0; i < idnt; i++)
	      *os << "  ";
	    *os << str << std::endl;
	  }
      if (this->use_syslog_ && (this->syslog_flags_ & flags))
	{
	  int priority;

	  if (flags & DEBUG)
	    priority = LOG_DEBUG;
	  else if (flags & ERROR)
	    priority = LOG_ERR;
	  else
	    priority = LOG_INFO;
	  syslog(priority, str, NULL);
	}
      if (indent)
	this->indent_[self]++;
    }
  catch (...)
    {
    }
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Logging default constructor.
 */
Logging::Logging() : syslog_flags_(0), use_syslog_(false)
{
}

/**
 *  Logging destructor.
 */
Logging::~Logging() throw ()
{
  // Outputs will self-destruct, we only have to close syslog, if necessary.
  if (this->use_syslog_)
    closelog();
}

/**
 *  Remove a level of indentation to the current thread.
 */
void Logging::Deindent() throw ()
{
  try
    {
      boost::unique_lock<boost::mutex> lock(this->mutex_);

      this->indent_[boost::this_thread::get_id()]--;
    }
  catch (...)
    {
    }
  return ;
}

/**
 *  Add a level of indentation to the current thread.
 */
void Logging::Indent() throw ()
{
  try
    {
      boost::unique_lock<boost::mutex> lock(this->mutex_);

      this->indent_[boost::this_thread::get_id()]++;
    }
  catch (...)
    {
    }
  return ;
}

#ifndef NDEBUG
/**
 *  Add a debug output.
 */
void Logging::LogDebug(const char* str, bool indent) throw ()
{
  this->LogBase(str, DEBUG, indent);
  return ;
}
#endif /* !NDEBUG */

/**
 *  Add an error output.
 */
void Logging::LogError(const char* str, bool indent) throw ()
{
  this->LogBase(str, ERROR, indent);
  return ;
}

/**
 *  Specify a file on which log should be sent.
 */
void Logging::LogInFile(const char* filename, int log_flags)
{
  Output out;

  out.filename_ = filename;
  out.flags_ = log_flags;
  out.stream_ = new std::ofstream;
  out.stream_->open(filename);
  if (out.stream_->is_open())
    this->outputs_.push_back(out);
  return ;
}

/**
 *  Determines whether or not output should be sent to the syslog facility.
 */
void Logging::LogInSyslog(bool use_syslog, int log_flags) throw ()
{
  this->syslog_flags_ = log_flags;
  this->use_syslog_ = use_syslog;
  if (this->use_syslog_)
    openlog("CentreonBroker", 0, LOG_USER);
  else
    closelog();
  return ;
}

/**
 *  Add an info output.
 */
void Logging::LogInfo(const char* str, bool indent) throw ()
{
  this->LogBase(str, INFO, indent);
  return ;
}

/**
 *  Signal when a thread is over so that allocated ressources are freed.
 */
void Logging::ThreadEnd()
{
  boost::unique_lock<boost::mutex> lock(this->mutex_);
  std::map<boost::thread::id, int>::iterator it;

  it = this->indent_.find(boost::this_thread::get_id());
  if (it != this->indent_.end())
    this->indent_.erase(it);
  return ;
}

/**
 *  Signal when a thread start so that necessary logging ressources are
 *  allocated.
 */
void Logging::ThreadStart()
{
  boost::unique_lock<boost::mutex> lock(this->mutex_);

  this->indent_[boost::this_thread::get_id()] = 0;
  return ;
}
