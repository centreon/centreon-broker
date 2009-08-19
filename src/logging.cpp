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

#include <boost/thread.hpp>
#include <cassert>
#include <sstream>
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
  return ;
}

/**
 *  \brief Copy data members of Logging::Output.
 *
 *  Make a copy of internal parameters. The parameters hold in the output
 *  variable will be transfered to the current instance and discarded in
 *  output.
 *
 *  \param[in] output Object to copy data from.
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
 *  \brief Output default constructor.
 *
 *  Does nothing.
 */
Logging::Output::Output() : flags_(0), stream_(NULL) {}

/**
 *  \brief Output copy constructor.
 *
 *  The parameters hold in output will be transfered to the current instance
 *  and discarded in output.
 *
 *  \param[in] output Object to copy data from.
 */
Logging::Output::Output(const Logging::Output& output)
{
  this->InternalCopy(output);
}

/**
 *  \brief Output destructor.
 *
 *  Will close the wrapped file if open.
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
 *  \brief Overload of the assignment operator.
 *
 *  The parameters hold in output will be transfered to the current instance
 *  and discarded in output.
 *
 *  \param[in] output Object to copy data from.
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
 *  \brief Logging copy constructor.
 *
 *  Shouldn't be used (call abort()).
 */
Logging::Logging(const Logging& l)
{
  (void)l;
  assert(false);
  abort();
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Shouldn't be used (call abort()).
 */
Logging& Logging::operator=(const Logging& l)
{
  (void)l;
  assert(false);
  abort();
  return (*this);
}

/**
 *  \brief Log a message into the appropriate outputs.
 *
 *  This is the basic method used to log messages to streams. It will browse
 *  the stream list and push the message if the flags match.
 *
 *  \param[in] str Message to log.
 *  \param[in] flag Type of message.
 */
void Logging::LogBase(const char* str, Logging::MsgType msg_type) throw ()
{
  try
    {
      boost::unique_lock<boost::mutex> lock(this->mutex_);
      std::string full_msg;

      // Build the full message string
      {
	std::ostringstream ss;

	full_msg = "[";
	ss << boost::this_thread::get_id();
	full_msg.append(ss.str());
	full_msg.append("] ");
	full_msg.append(str);
      }

      // Browse all outputs
      for (std::list<Output>::iterator ito = this->outputs_.begin();
	   ito != this->outputs_.end();
	   ito++)
	if ((*ito).flags_ & msg_type)
	  *(*ito).stream_ << full_msg << std::endl;

      // Special case : stderr
      if (this->stderr_flags_ & msg_type)
	std::cerr << full_msg << std::endl;

      // Special case : stdout
      if (this->stdout_flags_ & msg_type)
	std::cout << full_msg << std::endl;

      // Syslog
      if (this->syslog_flags_ & msg_type)
	{
	  int priority;

	  if (msg_type & DEBUG)
	    priority = LOG_DEBUG;
	  else if (msg_type & ERROR)
	    priority = LOG_ERR;
	  else
	    priority = LOG_INFO;
	  syslog(priority, str, NULL);
	}
    }
  catch (...) {}
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
Logging::Logging()
  : stderr_flags_(0),
    stdout_flags_(0),
    syslog_flags_(0) {}

/**
 *  Logging destructor.
 */
Logging::~Logging() throw ()
{
  // Outputs will self-destruct, we only have to close syslog, if necessary.
  if (this->syslog_flags_)
    closelog();
}

#ifndef NDEBUG
/**
 *  Add a debug output.
 *
 *  \param[in] str Debug message to log.
 */
void Logging::LogDebug(const char* str) throw ()
{
  this->LogBase(str, DEBUG);
  return ;
}
#endif /* !NDEBUG */

/**
 *  Add an error output.
 *
 *  \param[in] str Error message to log.
 */
void Logging::LogError(const char* str) throw ()
{
  this->LogBase(str, ERROR);
  return ;
}

/**
 *  Specify a file on which log should be sent.
 *
 *  \param[in] filename  Path of the file to log to.
 *  \param[in] log_flags Specify which kind of messages should be log. Specify
 *                       0 (default) to remove the file.
 */
void Logging::LogInFile(const char* filename, int log_flags)
{
  boost::unique_lock<boost::mutex> lock(this->mutex_);

  // Add or modify an output
  if (log_flags)
    {
      // Try to find the output
      for (std::list<Output>::iterator it = this->outputs_.begin();
	   it != this->outputs_.end();
	   it++)
	if (it->filename_ == filename)
	  {
	    it->flags_ = log_flags;
	    return ;
	  }

      // We didn't find it so add it.
      {
	Output out;

	out.filename_ = filename;
	out.flags_ = log_flags;
	out.stream_ = new std::ofstream;
	out.stream_->open(filename);
	this->outputs_.push_back(out);
      }
    }
  // Remove output
  else
    {
      for (std::list<Output>::iterator it = this->outputs_.begin();
           it != this->outputs_.end();
	   it++)
	if (it->filename_ == filename)
	  this->outputs_.erase(it);
    }
  return ;
}

/**
 *  Add an info output.
 *
 *  \param[in] str Informational message to log.
 */
void Logging::LogInfo(const char* str) throw ()
{
  this->LogBase(str, INFO);
  return ;
}

/**
 *  Determines whether or not output should be sent to the syslog facility.
 *
 *  \param[in] log_flags Specify which kind of messages should be sent to the
 *                       syslog facility (0 for none).
 */
void Logging::LogInSyslog(int log_flags) throw ()
{
  this->syslog_flags_ = log_flags;
  if (this->syslog_flags_)
    openlog("CentreonBroker", 0, LOG_USER);
  else
    closelog();
  return ;
}

/**
 *  Determines whether or not output should be sent to stderr.
 *
 *  \param[in] log_flags Specify which kind of messages should be sent to
 *                       stderr (0 for none).
 */
void Logging::LogToStderr(int log_flags) throw ()
{
  this->stderr_flags_ = log_flags;
  return ;
}

/**
 *  Determines whether or not output should be sent to stdout.
 *
 *  \param[in] log_flags Specify which kind of messages should be sent to
 *                       stdout (0 for none).
 */
void Logging::LogToStdout(int log_flags) throw ()
{
  this->stdout_flags_ = log_flags;
  return ;
}
