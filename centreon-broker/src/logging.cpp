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
 *  \brief Copy data members of Logging::OutputFile.
 *
 *  If the given object is open, filename and flags will be copied to the
 *  current instance and the file will be opened.
 *
 *  \param[in] output Object to copy data from.
 */
void Logging::OutputFile::InternalCopy(const Logging::OutputFile& output_file)
{
  if (output_file.stream.is_open())
    {
      this->filename = output_file.filename;
      this->flags = output_file.flags;
      this->RealOpen();
    }
  return ;
}

/**
 *  \brief Really open the file.
 *
 *  This private method really opens the file handle. This method is used by
 *  the copy constructor, the assignment operator and the standard Open()
 *  method.
 */
void Logging::OutputFile::RealOpen()
{
  this->stream.clear();
  this->stream.open(this->filename.c_str());
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief OutputFile default constructor.
 *
 *  Does nothing.
 */
Logging::OutputFile::OutputFile() : flags(0) {}

/**
 *  \brief OutputFile copy constructor.
 *
 *  If the given object is open, filename and flags will be copied to the
 *  current instance and the file will be opened.
 *
 *  \param[in] output Object to copy data from.
 */
Logging::OutputFile::OutputFile(const Logging::OutputFile& output_file)
{
  this->InternalCopy(output_file);
}

/**
 *  \brief OutputFile destructor.
 *
 *  Will close the wrapped file if open.
 */
Logging::OutputFile::~OutputFile() throw ()
{
  try
    {
      this->Close();
    }
  catch (...)
    {
    }
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  If the given object is open, filename and flags will be copied to the
 *  current instance and the file will be opened.
 *
 *  \param[in] output Object to copy data from.
 */
Logging::OutputFile& Logging::OutputFile::operator=(
  const Logging::OutputFile& output_file)
{
  this->Close();
  this->InternalCopy(output_file);
  return (*this);
}

/**
 *  If the current stream is open, close it.
 */
void Logging::OutputFile::Close()
{
  if (this->stream.is_open())
    this->stream.close();
  return ;
}

/**
 *  \brief Open an output file.
 *
 *  Open the file given as a parameter and associate it with some flags. If a
 *  file was previously opened, it's closed.
 *
 *  \param[in] filename The path to the file that should be opened.
 */
bool Logging::OutputFile::Open(const std::string& filename, unsigned int flags)
{
  this->Close();
  this->filename = filename;
  this->flags = flags;
  this->RealOpen();
  return (!this->stream.is_open());
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
      for (std::list<OutputFile>::iterator ito = this->outputs_.begin();
	   ito != this->outputs_.end();
	   ito++)
	if ((*ito).flags & msg_type)
	  (*ito).stream << full_msg << std::endl;

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
 *                       0 (default) to remove the file from the outputs.
 */
void Logging::LogInFile(const char* filename, int log_flags)
{
  boost::unique_lock<boost::mutex> lock(this->mutex_);

  // Add or modify an output
  if (log_flags)
    {
      // Try to find the output
      for (std::list<OutputFile>::iterator it = this->outputs_.begin();
	   it != this->outputs_.end();
	   it++)
	if (it->filename == filename)
	  {
	    it->flags = log_flags;
	    return ;
	  }

      // We didn't find it so add it.
      try
	{
          // Add an element at the end of the list
	  this->outputs_.push_back(OutputFile());

	  // Get a reference to the last element
	  OutputFile& out(this->outputs_.back());

	  // Initialize it
          if (out.Open(filename, log_flags))
            this->outputs_.pop_back();
        }
      catch (...)
        {
          this->outputs_.pop_back();
        }
    }
  // Remove output
  else
    {
      for (std::list<OutputFile>::iterator it = this->outputs_.begin();
           it != this->outputs_.end();
	   ++it)
	if (it->filename == filename)
          {
            this->outputs_.erase(it);
            break ;
          }
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
