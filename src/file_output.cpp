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
#include "events/event.h"
#include "file_output.h"
#include "logging.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  FileOutput copy constructor.
 */
FileOutput::FileOutput(const FileOutput& fo) : EventSubscriber()
{
  (void)fo;
  assert(false);
}

/**
 *  FileOutput operator= overload.
 */
FileOutput& FileOutput::operator=(const FileOutput& fo)
{
  (void)fo;
  assert(false);
  return (*this);
}

/**
 *  Callback called when an event occur. This will push the event into the list.
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
FileOutput::FileOutput() : exit_(true), thread_(NULL)
{
}

/**
 *  FileOutput destructor.
 */
FileOutput::~FileOutput()
{
  this->Close();
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
    }
  return ;
}

/**
 *  Closes the file.
 */
void FileOutput::Close()
{
  if (this->thread_)
    {
#ifndef NDEBUG
      logging.LogDebug("Finishing file-writing thread...");
      logging.LogDebug("Cancelling thread execution...");
#endif /* !NDEBUG */
      this->exit_ = true;

      // In case the working thread is waiting on event, cancel his waiting
      this->events_.CancelWait();

#ifndef NDEBUG
      logging.LogDebug("Waiting for thread termination...");
#endif /* !NDEBUG */

      // Wait for thread termination
      this->thread_->join();
      delete (this->thread_);
      this->thread_ = NULL;
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
 *  Opens the file.
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
  this->exit_ = false;

#ifndef NDEBUG
  logging.LogDebug("Launching file-writing thread...");
#endif /* !NDEBUG */

  // Create worker thread
  this->thread_ = new boost::thread(boost::ref(*this));
  return ;
}
