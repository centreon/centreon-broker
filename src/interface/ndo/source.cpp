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

#include <assert.h>
#include <memory>                   // for auto_ptr
#include <stdlib.h>                 // for abort, strtol
#include <string.h>                 // for strcmp, strncmp
#include "events/events.h"
#include "interface/ndo/internal.h"
#include "interface/ndo/source.h"
#include "io/stream.h"
#include "nagios/protoapi.h"

using namespace Interface::NDO;

/**************************************
*                                     *
*           Static Methods            *
*                                     *
**************************************/

/**
 *  Extract event parameters from the data stream.
 */
template <typename T>
static T* HandleEvent(IO::Text& stream)
{
  std::auto_ptr<T> event(new T);
  int key;
  const char* key_str;
  const char* value_str;

  while (1)
    {
      key_str = stream.Line();
      if (key_str)
        {
          typename std::map<int, GetterSetter<T> >::const_iterator it;

          key = strtol(key_str, NULL, 10);
          if (NDO_API_ENDDATA == key)
            break ;
          value_str = strchr(key_str, '=');
          value_str = (value_str ? value_str + 1 : "");
          it = NDOMappedType<T>::map.find(key);
          if (it != NDOMappedType<T>::map.end())
            (*it->second.setter)(*event.get(), *it->second.member, value_str);
        }
      else
        {
          event.reset();
          break ;
        }
    }
  return (event.release());
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Source copy constructor.
 *
 *  As Source is not copyable, any attempt to use the copy constructor will
 *  result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] source Unused.
 */
Source::Source(const Source& source) : Base(NULL), Interface::Source(source)
{
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  As Source is not copyable, any attempt to use the assignment operator will
 *  result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] source Unused.
 *
 *  \return *this
 */
Source& Source::operator=(const Source& source)
{
  (void)source;
  assert(false);
  abort();
  return (*this);
}

/**
 *  Parse the NDO header.
 *
 *  \return The initial event.
 */
Events::Event* Source::Header()
{
  const char* line;
  std::auto_ptr<Events::ProgramStatus> pstatus(new Events::ProgramStatus);

  while ((line = this->stream_.Line()) && strcmp(line, NDO_API_STARTDATADUMP))
    {
      const char* value;

      value = strchr(line, ' ');
      if (value)
        {
          ++value;
          if (!strncmp(line,
                       NDO_API_INSTANCENAME,
                       sizeof(NDO_API_INSTANCENAME) - 1))
            {
              pstatus->is_running = true;
              this->instance_ = value;
            }
          else if (!strncmp(line,
                            NDO_API_STARTTIME,
                            sizeof(NDO_API_STARTTIME) - 1))
            pstatus->program_start = strtol(value, NULL, 0);
        }
    }
  return (pstatus.release());
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Source constructor.
 *
 *  Build an NDO input source that uses the stream object as raw binary input.
 *  The stream object must not be NULL and is owned by the Source object upon
 *  successful return from the constructor.
 *
 *  \param[in] stream Input stream object.
 */
Source::Source(IO::Stream* stream) : Base(stream) {}

/**
 *  Source destructor.
 */
Source::~Source() {}

/**
 *  Close the source object.
 */
void Source::Close()
{
  this->stream_.Close();
  return ;
}

/**
 *  \brief Get the next available event.
 *
 *  Extract the next available event on the input stream, NULL if the stream is
 *  closed.
 *
 *  \return Next available event, NULL is stream is closed.
 */
Events::Event* Source::Event()
{
  std::auto_ptr<Events::Event> event;
  const char* line;

  // Get the next non-empty line.
  do
    {
      line = this->stream_.Line();
    } while (line && !line[0]);

  if (line)
    {
      // Parse initial header.
      if (!strcmp(line, NDO_API_HELLO))
        event.reset(this->Header());
      else
        {
          int id;

          id = strtol(line, NULL, 10);
          switch (id)
            {
             case NDO_API_ACKNOWLEDGEMENTDATA:
              event.reset(HandleEvent<Events::Acknowledgement>(this->stream_));
              break ;
             case NDO_API_COMMENTDATA:
              event.reset(HandleEvent<Events::Comment>(this->stream_));
              break ;
             case NDO_API_DOWNTIMEDATA:
              event.reset(HandleEvent<Events::Downtime>(this->stream_));
              break ;
             case NDO_API_HOSTCHECKDATA:
              event.reset(HandleEvent<Events::HostCheck>(this->stream_));
              break ;
             case NDO_API_HOSTDEFINITION:
              event.reset(HandleEvent<Events::Host>(this->stream_));
              break ;
             case NDO_API_HOSTDEPENDENCYDEFINITION:
              event.reset(HandleEvent<Events::HostDependency>(this->stream_));
              break ;
             case NDO_API_HOSTGROUPDEFINITION:
              event.reset(HandleEvent<Events::HostGroup>(this->stream_));
              break ;
             case NDO_API_HOSTGROUPMEMBERDEFINITION:
              event.reset(HandleEvent<Events::HostGroupMember>(this->stream_));
              break ;
             case NDO_API_HOSTPARENT:
              event.reset(HandleEvent<Events::HostParent>(this->stream_));
              break ;
             case NDO_API_HOSTSTATUSDATA:
              event.reset(HandleEvent<Events::HostStatus>(this->stream_));
              break ;
             case NDO_API_LOGDATA:
              event.reset(HandleEvent<Events::Log>(this->stream_));
              break ;
             case NDO_API_PROGRAMSTATUSDATA:
              event.reset(HandleEvent<Events::ProgramStatus>(this->stream_));
              break ;
             case NDO_API_SERVICECHECKDATA:
              event.reset(HandleEvent<Events::ServiceCheck>(this->stream_));
              break ;
             case NDO_API_SERVICEDEFINITION:
              event.reset(HandleEvent<Events::Service>(this->stream_));
              break ;
             case NDO_API_SERVICEDEPENDENCYDEFINITION:
              event.reset(HandleEvent<Events::ServiceDependency>(this->stream_));
              break ;
             case NDO_API_SERVICEGROUPDEFINITION:
              event.reset(HandleEvent<Events::ServiceGroup>(this->stream_));
              break ;
             case NDO_API_SERVICEGROUPMEMBERDEFINITION:
              event.reset(HandleEvent<Events::ServiceGroupMember>(this->stream_));
              break ;
             case NDO_API_SERVICESTATUSDATA:
              event.reset(HandleEvent<Events::ServiceStatus>(this->stream_));
              break ;
             default:
              // Skip this event.
              while (id != NDO_API_ENDDATA)
                {
                  line = this->stream_.Line();
                  if (line)
                    id = strtol(line, NULL, 10);
                  else
                    break ;
                }
              if (line)
                return (this->Event());
            }
        }
    }
  if (event.get())
    {
      event->instance = this->instance_;
      event->AddReader();
    }
  return (event.release());
}
