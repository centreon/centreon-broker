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

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <sstream>
#include <unistd.h>
#include "events/host_group.h"
#include "events/service_group.h"
#include "exception.h"
#include "file_input.h"
#include "mapping.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Open the next available file.
 */
void FileInput::OpenNext()
{
  std::stringstream ss;

  this->Close();
  ss << this->path_ << this->current_;
  this->ifs_.open(ss.str().c_str());
  if (!this->ifs_.fail())
    this->current_++;
  else
    throw (Exception(0, "Could not open dumped file."));
  return ;
}

/**
 *  \brief Read an event from the current file.
 *
 *  Read an event of type T from the current dump file.
 *
 *  \param[out] event Store data in this event.
 *  \param[in]  dm    Pointer to members.
 */
template <typename T>
bool FileInput::ReadEvent(T& event, const DB::DataMember<T> dm[])
{
  char buffer[1024];
  bool ret;
  unsigned int size;

  ret = false;
  try
    {
      this->ifs_.read((char*)&size, sizeof(size));
      if (size >= sizeof(buffer))
	size = sizeof(buffer) - 1;
      this->ifs_.read(buffer, size);
      buffer[size] = '\0';
      event.instance = buffer;
      for (unsigned int i = 0; dm[i].name; i++)
        {
          switch (dm[i].type)
            {
             case 'b':
              this->ifs_.read((char*)&(event.*dm[i].value.b), sizeof(bool));
              break ;
             case 'd':
              this->ifs_.read((char*)&(event.*dm[i].value.d), sizeof(double));
              break ;
             case 'i':
              this->ifs_.read((char*)&(event.*dm[i].value.i), sizeof(int));
              break ;
             case 's':
              this->ifs_.read((char*)&(event.*dm[i].value.s), sizeof(short));
              break ;
             case 'S':
              this->ifs_.read((char*)&size, sizeof(size));
              if (size >= sizeof(buffer))
                size = sizeof(buffer) - 1;
              this->ifs_.read(buffer, size);
              buffer[size] = '\0';
              event.*dm[i].value.S = buffer;
              break ;
             case 't':
              this->ifs_.read((char*)&(event.*dm[i].value.t), sizeof(time_t));
              break ;
            }
        }
    }
  // Caught on EOF
  catch (std::ifstream::failure& f)
    {
      try
	{
	  std::stringstream ss;

	  this->Close();
	  ss << this->path_ << this->current_ - 1;
	  unlink(ss.str().c_str());
	  this->OpenNext();
	  this->ReadEvent<T>(event, dm);
	}
      catch (...)
	{
	  ret = true;
	}
    }
  return (ret);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  FileInput constructor.
 */
FileInput::FileInput() : current_(0)
{
  this->ifs_.exceptions(std::ifstream::eofbit);
}

/**
 *  FileInput copy constructor.
 *
 *  \param[in] fi Object to copy data from.
 */
FileInput::FileInput(const FileInput& fi)
{
  this->operator=(fi);
}

/**
 *  FileInput destructor.
 */
FileInput::~FileInput()
{
  this->Close();
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] fi Object to copy data from.
 *
 *  \return *this
 */
FileInput& FileInput::operator=(const FileInput& fi)
{
  this->Close();
  this->path_ = fi.path_;
  if (!fi.ifs_.fail())
    {
      this->current_ = fi.current_ - 1;
      this->OpenNext();
    }
  else
    this->current_ = fi.current_;
  return (*this);
}

/**
 *  Close the internal handle if opened.
 */
void FileInput::Close()
{
  if (this->ifs_.is_open())
    this->ifs_.close();
  return ;
}

/**
 *  Read the next available event.
 */
Events::Event* FileInput::NextEvent()
{
  Events::Event* event;
  int type;

  try
    {
      this->ifs_.read((char*)&type, sizeof(type));
    }
  catch (std::ifstream::failure& f)
    {
      try
	{
	  std::stringstream ss;

	  this->Close();
	  ss << this->path_ << this->current_ - 1;
	  unlink(ss.str().c_str());
	  this->OpenNext();
	  return (this->NextEvent());
	}
      catch (...)
	{
          return (NULL);
	}
    }

  switch (type)
    {
     case Events::Event::ACKNOWLEDGEMENT:
      {
        std::auto_ptr<Events::Acknowledgement> ack(
          new Events::Acknowledgement);

        if (this->ReadEvent(*ack.get(), acknowledgement_dm))
          {
            ack.reset();
            event = NULL;
          }
        else
          {
            event = ack.get();
            ack.release();
          }
      }
      break ;
     case Events::Event::COMMENT:
      {
        std::auto_ptr<Events::Comment> comment(
          new Events::Comment);

        if (this->ReadEvent(*comment.get(), comment_dm))
          {
            comment.reset();
            event = NULL;
          }
        else
          {
            event = comment.get();
            comment.release();
          }
      }
      break ;
     case Events::Event::DOWNTIME:
      {
        std::auto_ptr<Events::Downtime> downtime(
          new Events::Downtime);

        if (this->ReadEvent(*downtime.get(), downtime_dm))
          {
            downtime.reset();
            event = NULL;
          }
        else
          {
            event = downtime.get();
            downtime.release();
          }
      }
      break ;
     case Events::Event::HOST:
      {
        std::auto_ptr<Events::Host> host(
          new Events::Host);

        if (this->ReadEvent(*host.get(), host_dm))
          {
            host.reset();
            event = NULL;
          }
        else
          {
            event = host.get();
            host.release();
          }
      }
      break ;
     case Events::Event::HOSTGROUP:
      {
        std::auto_ptr<Events::HostGroup> host_group(
          new Events::HostGroup);

        if (this->ReadEvent(*static_cast<Events::Group*>(host_group.get()),
                            group_dm))
          {
            host_group.reset();
            event = NULL;
          }
        else
          {
            event = host_group.get();
            host_group.release();
          }
      }
      break ;
     case Events::Event::HOSTSTATUS:
      {
        std::auto_ptr<Events::HostStatus> host_status(
          new Events::HostStatus);

        if (this->ReadEvent(*host_status.get(), host_status_dm))
          {
            host_status.reset();
            event = NULL;
          }
        else
          {
            event = host_status.get();
            host_status.release();
          }
      }
      break ;
     case Events::Event::LOG:
      {
        std::auto_ptr<Events::Log> log(
          new Events::Log);

        if (this->ReadEvent(*log.get(), log_dm))
          {
            log.reset();
            event = NULL;
          }
        else
          {
            event = log.get();
            log.release();
          }
      }
      break ;
     case Events::Event::PROGRAMSTATUS:
      {
        std::auto_ptr<Events::ProgramStatus> program_status(
          new Events::ProgramStatus);

        if (this->ReadEvent(*program_status.get(), program_status_dm))
          {
            program_status.reset();
            event = NULL;
          }
        else
          {
            event = program_status.get();
            program_status.release();
          }
      }
      break ;
     case Events::Event::SERVICE:
      {
        std::auto_ptr<Events::Service> service(
          new Events::Service);

        if (this->ReadEvent(*service.get(), service_dm))
          {
            service.reset();
            event = NULL;
          }
        else
          {
            event = service.get();
            service.release();
          }
      }
      break ;
     case Events::Event::SERVICEGROUP:
      {
	std::auto_ptr<Events::ServiceGroup> service_group(
          new Events::ServiceGroup);

	if (this->ReadEvent(*static_cast<Events::Group*>(service_group.get()),
                            group_dm))
          {
            service_group.reset();
            event = NULL;
          }
        else
          {
            event = service_group.get();
            service_group.release();
          }
      }
      break ;
     case Events::Event::SERVICESTATUS:
      {
        std::auto_ptr<Events::ServiceStatus> ss(
          new Events::ServiceStatus);

        if (this->ReadEvent(*ss.get(), service_status_dm))
          {
            ss.reset();
            event = NULL;
          }
        else
          {
            event = ss.get();
            ss.release();
          }
      }
      break ;
     default:
      event = NULL;
    }
  return (event);
}

/**
 *  Set the base path to search for files and open the first available file.
 */
void FileInput::Open(const std::string& base_path)
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
  this->current_ = UINT_MAX;
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
	    if ('\0' == *err && val < this->current_)
	      this->current_ = val;
	  }
      }
    closedir(dir);
  }

  // Really open the proper file
  this->OpenNext();

  return ;
}
