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
void FileOutput::Dump(const T& event, const DB::DataMember<T> dm[])
{
  this->ofs << event.GetType() << std::endl
            << event.instance << std::endl;
  for (unsigned int i = 0; dm[i].name; i++)
    {
      switch (dm[i].type)
	{
         case 'b':
	  this->ofs << event.*dm[i].value.b << std::endl;
	  break ;
         case 'd':
	  this->ofs << event.*dm[i].value.d << std::endl;
	  break ;
         case 'i':
	  this->ofs << event.*dm[i].value.i << std::endl;
	  break ;
         case 's':
	  this->ofs << event.*dm[i].value.s << std::endl;
	  break ;
         case 'S':
	  this->ofs << event.*dm[i].value.S << std::endl;
	  break ;
         case 't':
	  this->ofs << event.*dm[i].value.t << std::endl;
	  break ;
	}
    }
  this->ofs << std::endl;
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

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  FileOutput default constructor.
 */
FileOutput::FileOutput() : exit_(true) {}

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
                    acknowledgement_dm);
                  break ;
                 case Events::Event::COMMENT:
                  this->Dump<Events::Comment>(
                    *static_cast<Events::Comment*>(e),
                    comment_dm);
                  break ;
                 case Events::Event::CONNECTION:
                  this->Dump<Events::Connection>(
                    *static_cast<Events::Connection*>(e),
                    connection_dm);
                  break ;
                 case Events::Event::CONNECTIONSTATUS:
                  this->Dump<Events::ConnectionStatus>(
                    *static_cast<Events::ConnectionStatus*>(e),
                    connection_status_dm);
                  break ;
                 case Events::Event::DOWNTIME:
                  this->Dump<Events::Downtime>(
                    *static_cast<Events::Downtime*>(e),
                    downtime_dm);
                  break ;
                 case Events::Event::HOST:
                  this->Dump<Events::Host>(
                    *static_cast<Events::Host*>(e),
                    host_dm);
                  break ;
                 case Events::Event::HOSTGROUP:
                  this->Dump<Events::HostGroup>(
                    *static_cast<Events::HostGroup*>(e),
                    host_group_dm);
                  break ;
                 case Events::Event::HOSTSTATUS:
                  this->Dump<Events::HostStatus>(
                    *static_cast<Events::HostStatus*>(e),
                    host_status_dm);
                  break ;
                 case Events::Event::LOG:
                  this->Dump<Events::Log>(
                    *static_cast<Events::Log*>(e),
                    log_dm);
                  break ;
                 case Events::Event::PROGRAMSTATUS:
                  this->Dump<Events::ProgramStatus>(
                    *static_cast<Events::ProgramStatus*>(e),
                    program_status_dm);
                  break ;
                 case Events::Event::SERVICE:
                  this->Dump<Events::Service>(
                    *static_cast<Events::Service*>(e),
                    service_dm);
                  break ;
                 case Events::Event::SERVICESTATUS:
                  this->Dump<Events::ServiceStatus>(
                    *static_cast<Events::ServiceStatus*>(e),
                    service_status_dm);
                  break ;
                 default:
                  logging.LogInfo("Invalid type of event processed");
		}

	      // We're finished with the event
	      e->RemoveReader(this);
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
  if (this->ofs.is_open())
    {
#ifndef NDEBUG
      logging.LogDebug("Closing file...");
#endif /* !NDEBUG */

      // Close file stream
      this->ofs.close();
    }
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
void FileOutput::Open(const char* filename)
{
  assert(filename);
#ifndef NDEBUG
  logging.LogDebug("Opening file...");
  logging.LogDebug(filename);
#endif /* !NDEBUG */

  // Open file
  this->ofs.open(filename);
  if (this->ofs.fail())
    throw (Exception(0, "Could not open output file."));

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
