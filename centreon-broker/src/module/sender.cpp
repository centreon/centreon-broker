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

#include "dbuffer.h"
#include "io/io.h"
#include "sender.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy internal data of the given object to the current instance.
 *
 *  Copy data members defined within the Sender class from the given object to
 *  the current instance. This method is used internally by the copy
 *  constructor and the assignment operator.
 *
 *  \param[in] sender Object to copy data from.
 */
void Sender::InternalCopy(const Sender& sender)
{
  this->data_ = sender.data_;
  this->outputs_ = sender.outputs_;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Sender default constructor.
 */
Sender::Sender() : thread_(NULL) {}

/**
 *  \brief Sender copy constructor.
 *
 *  Copy internal data of the given object to the current instance. Resulting
 *  object is left unrun.
 *
 *  \param[in] sender Object to copy data from.
 */
Sender::Sender(const Sender& sender) : thread_(NULL)
{
  this->InternalCopy(sender);
}

/**
 *  Sender destructor.
 */
Sender::~Sender()
{
  this->Stop();
  for (std::list<IO::Stream*>::iterator it = this->outputs_.begin();
       it != this->outputs_.end();
       ++it)
    delete (*it);
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy internal data of the given object to the current instance. Resulting
 *  object is left unrun.
 *
 *  \param[in] sender Object to copy data from.
 *
 *  \return *this
 */
Sender& Sender::operator=(const Sender& sender)
{
  this->Stop();
  this->InternalCopy(sender);
  return (*this);
}

/**
 *  Thread entry point.
 */
void Sender::operator()()
{
  try
    {
      std::auto_ptr<DBuffer> dbuffer;

      while (!this->exit_)
	{
          dbuffer.reset(this->data_.Wait());
          if (dbuffer.get())
            for (std::list<IO::Stream*>::iterator it = this->outputs_.begin();
                 it != this->outputs_.end();
                 ++it)
              (*it)->Send(*dbuffer, dbuffer->Size());
	}
    }
  // Avoid exception propagation in C code.
  catch (...) {}

  return ;
}

/**
 *  Add data buffer.
 *
 *  \param[in] dbuffer Add data in the data pool.
 */
void Sender::AddData(DBuffer* dbuffer)
{
  this->data_.Add(dbuffer);
  return ;
}

/**
 *  Add an output.
 *
 *  \param[in] output Output to add.
 */
void Sender::AddOutput(IO::Stream* output)
{
  this->outputs_.push_back(output);
  return ;
}

/**
 *  Run the worker thread that will write data posted to the sender to the
 *  outputs.
 */
void Sender::Run()
{
  this->thread_ = new boost::thread(boost::ref(*this));
  return ;
}

/**
 *  Stop the running thread.
 */
void Sender::Stop()
{
  if (this->thread_)
    {
      this->exit_ = true;
      this->data_.CancelWait();
      this->thread_->interrupt();
      this->thread_->join();
      delete (this->thread_);
    }
  return ;
}
