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

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include "event_publisher.h"
#include "events/acknowledgement.h"
#include "events/comment.h"
#include "events/connection.h"
#include "events/connection_status.h"
#include "events/downtime.h"
#include "events/event.h"
#include "events/host.h"
#include "events/host_group.h"
#include "events/host_status.h"
#include "events/log.h"
#include "events/program_status.h"
#include "events/service.h"
#include "events/service_status.h"
#include "file_output.h"
#include "logging.h"
#include "mapping.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  \brief FileOutput copy constructor.
 *
 *  Any attempt to use it will result in a call to abort().
 */
FileOutput::FileOutput(const FileOutput& fo) : EventSubscriber()
{
  (void)fo;
  assert(false);
  abort();
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Any attempt to use it will result in a call to abort().
 */
FileOutput& FileOutput::operator=(const FileOutput& fo)
{
  (void)fo;
  assert(false);
  abort();
  return (*this);
}

/**
 *  Dump an event.
 */
template <typename T>
void FileOutput::Dump(const T& event,
                      const DB::DataMember<T> dm[],
                      unsigned int& wb)
{
  int type;

  type = event.GetType();
  this->ofs_.write((const char*)&type, sizeof(type));
  type = event.instance.size();
  this->ofs_.write((const char*)&type, sizeof(type));
  this->ofs_.write(event.instance.c_str(), type);
  wb += type + 2 * sizeof(type);
  for (unsigned int i = 0; dm[i].name; i++)
    {
      switch (dm[i].type)
	{
         case 'b':
	   this->ofs_.write((const char*)&(event.*dm[i].value.b),
                            sizeof(bool));
	  wb += sizeof(bool);
	  break ;
         case 'd':
	   this->ofs_.write((const char*)&(event.*dm[i].value.d),
                            sizeof(double));
	  wb += sizeof(double);
	  break ;
         case 'i':
	   this->ofs_.write((const char*)&(event.*dm[i].value.i),
                            sizeof(int));
	  wb += sizeof(int);
	  break ;
         case 's':
	   this->ofs_.write((const char*)&(event.*dm[i].value.s),
                            sizeof(short));
	  wb += sizeof(short);
	  break ;
         case 'S':
	  {
	    unsigned int size;

	    size = (event.*dm[i].value.S).size();
	    this->ofs_.write((const char*)&size, sizeof(size));
	    wb += sizeof(size);
	    this->ofs_.write((event.*dm[i].value.S).c_str(), size);
	    wb += size;
	    break ;
	  }
         case 't':
	   this->ofs_.write((const char*)&(event.*dm[i].value.t),
                            sizeof(time_t));
	  wb += sizeof(time_t);
	  break ;
	}
    }
  return ;
}

/**
 *  Close the internal file handle.
 */
void FileOutput::FileClose()
{
  if (this->ofs_.is_open())
    this->ofs_.close();
  return ;
}

/**
 *  Callback called when an event occur. This will push the event in the
 *  internal list.
 *
 *  \brief e New event.
 */
void FileOutput::OnEvent(Events::Event* e) throw ()
{
  try
    {
      this->events_.Add(e);
    }
  catch (...)
    {
      logging.LogError("Exception while adding event to list. Dropping event.");
      e->RemoveReader(this);
    }
  return ;
}

/**
 *  Open the next file.
 */
void FileOutput::OpenNext()
{
  std::stringstream ss;

  ss << this->path_ << this->current_ + 1;
#ifndef NDEBUG
  logging.LogDebug("Opening file...");
  logging.LogDebug(ss.str().c_str());
#endif /* !NDEBUG */
  this->ofs_.open(ss.str().c_str());
  if (this->ofs_.fail())
    throw (Exception(0, "Could not open output file"));
  return ;
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  FileOutput default constructor.
 */
FileOutput::FileOutput() : exit_(true), max_size_(100000000) {}

/**
 *  FileOutput destructor.
 */
FileOutput::~FileOutput()
{
  this->Close();
  EventPublisher::GetInstance().Unsubscribe(this);
}

/**
 *  Thread start function.
 */
void FileOutput::operator()()
{
  try
    {
      unsigned int wb;

      wb = 0;
      // While thread has not been canceled
      while (!this->exit_)
	{
	  Events::Event* e;

	  // Wait for an event
	  e = this->events_.Wait();
	  if (e)
	    {
	      switch (e->GetType())
		{
                 case Events::Event::ACKNOWLEDGEMENT:
                  this->Dump<Events::Acknowledgement>(
                    *static_cast<Events::Acknowledgement*>(e),
                    acknowledgement_dm,
                    wb);
                  break ;
                 case Events::Event::COMMENT:
                  this->Dump<Events::Comment>(
                    *static_cast<Events::Comment*>(e),
                    comment_dm,
                    wb);
                  break ;
                 case Events::Event::CONNECTION:
                  this->Dump<Events::Connection>(
                    *static_cast<Events::Connection*>(e),
                    connection_dm,
                    wb);
                  break ;
                 case Events::Event::CONNECTIONSTATUS:
                  this->Dump<Events::ConnectionStatus>(
                    *static_cast<Events::ConnectionStatus*>(e),
                    connection_status_dm,
                    wb);
                  break ;
                 case Events::Event::DOWNTIME:
                  this->Dump<Events::Downtime>(
                    *static_cast<Events::Downtime*>(e),
                    downtime_dm,
                    wb);
                  break ;
                 case Events::Event::HOST:
                  this->Dump<Events::Host>(
                    *static_cast<Events::Host*>(e),
                    host_dm,
                    wb);
                  break ;
                 case Events::Event::HOSTGROUP:
                  this->Dump<Events::HostGroup>(
                    *static_cast<Events::HostGroup*>(e),
                    host_group_dm,
                    wb);
                  break ;
                 case Events::Event::HOSTSTATUS:
                  this->Dump<Events::HostStatus>(
                    *static_cast<Events::HostStatus*>(e),
                    host_status_dm,
                    wb);
                  break ;
                 case Events::Event::LOG:
                  this->Dump<Events::Log>(
                    *static_cast<Events::Log*>(e),
                    log_dm,
                    wb);
                  break ;
                 case Events::Event::PROGRAMSTATUS:
                  this->Dump<Events::ProgramStatus>(
                    *static_cast<Events::ProgramStatus*>(e),
                    program_status_dm,
                    wb);
                  break ;
                 case Events::Event::SERVICE:
                  this->Dump<Events::Service>(
                    *static_cast<Events::Service*>(e),
                    service_dm,
                    wb);
                  break ;
                 case Events::Event::SERVICESTATUS:
                  this->Dump<Events::ServiceStatus>(
                    *static_cast<Events::ServiceStatus*>(e),
                    service_status_dm,
                    wb);
                  break ;
                 default:
                  logging.LogInfo("Invalid type of event processed");
		}

	      // We're finished with the event
	      e->RemoveReader(this);

	      // Check if we have to move to the next file
	      if (wb >= this->max_size_)
		{
		  wb = 0;
		  this->FileClose();
		  this->OpenNext();
		}
	    }
	}
    }
  catch (std::exception& e)
    {
      logging.LogError("Caught exception while writing to file");
      logging.LogError(e.what());
    }
  catch (...)
    {
      logging.LogError("Caught unknown exception while writing to file");
    }
  return ;
}

/**
 *  Closes the file.
 */
void FileOutput::Close()
{
  if (this->thread_.get())
    {
#ifndef NDEBUG
      logging.LogDebug("Finishing file-writing thread...");
#endif /* !NDEBUG */
      this->exit_ = true;

      // In case the working thread is waiting on event, cancel his waiting
      this->events_.CancelWait();

#ifndef NDEBUG
      logging.LogDebug("Waiting for thread termination...");
#endif /* !NDEBUG */

      // Wait for thread termination
      this->thread_->join();
      this->thread_.reset();
    }

  // Close the file
  this->FileClose();

  return ;
}

/**
 *  \brief Lock the event list for exclusive use.
 *
 *  The event list will be locked so any call to OnEvent will be blocked until
 *  processing is finished and the Unlock() method is called. This method is
 *  used in the context of a database processing object failover.
 */
void FileOutput::Lock()
{
  this->events_.Lock();
  return ;
}

/**
 *  \brief Open a file.
 *
 *  Open the specified file and launch the writing thread.
 *
 *  \param[in] filename Path of the file to open.
 */
void FileOutput::Open(const std::string& base_path)
{
  std::string base_dir;
  std::string base_file;

  // Copy base path
  this->path_ = base_path;

  // Extract bases
  {
    size_t size;

    size = this->path_.find_last_of('/');
    if (size == std::string::npos)
      base_dir = "./";
    else
      {
	base_dir = this->path_;
	base_dir.resize(size);
	base_file = this->path_.substr(size + 1);
      }
  }

  // Browse directory
  this->current_ = 0;
  {
    DIR* dir;
    struct dirent* entry;

    dir = opendir(base_dir.c_str());
    if (!dir)
      throw (Exception(errno, strerror(errno)));
    while ((entry = readdir(dir)))
      {
	if (!strncmp(entry->d_name, base_file.c_str(), base_file.size()))
	  {
	    char* err;
	    unsigned int val;

	    val = strtoul(entry->d_name + base_file.size(), &err, 0);
	    if ('\0' == *err && val > this->current_)
	      this->current_ = val;
	  }
      }
    closedir(dir);
  }

  // Really open the proper file
  this->OpenNext();

  return ;
}

/**
 *  Launch the processing thread.
 */
void FileOutput::Run()
{
#ifndef NDEBUG
  logging.LogDebug("Launching file-writing thread...");
#endif /* !NDEBUG */

  // Create worker thread
  this->exit_ = false;
  this->thread_.reset(new boost::thread(boost::ref(*this)));

  return ;
}

/**
 *  Transfer events from the given list to the internal list.
 *
 *  \param[in] ev List to transfer objects from.
 */
void FileOutput::StoreEvents(WaitableList<Events::Event>& ev)
{
  std::list<Events::Event*>& internal(this->events_.List());

  while (!ev.Empty())
    internal.push_back(ev.Wait());
  return ;
}

/**
 *  Unlock the FileOutput object.
 *
 *  \see Lock
 */
void FileOutput::Unlock()
{
  this->events_.Unlock();
  return ;
}
