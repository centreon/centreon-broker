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
#include <map>
#include <memory>                   // for auto_ptr
#include <stdlib.h>                 // for abort, strtod, strtol
#include <string.h>                 // for strcmp, strncmp
#include "events/acknowledgement.h"
#include "events/comment.h"
#include "events/downtime.h"
#include "events/host.h"
#include "events/host_group.h"
#include "events/host_status.h"
#include "events/program_status.h"
#include "events/service.h"
#include "events/service_status.h"
#include "interface/ndo/internal.h"
#include "interface/ndo/source.h"
#include "io/stream.h"
#include "nagios/protoapi.h"

using namespace Interface::NDO;

/**************************************
*                                     *
*          Static Functions           *
*                                     *
**************************************/

/**
 *  Set a boolean within an object.
 */
template <typename T>
static void set_boolean(T& t,
                        const typename KeyField<T>::UHandler& field,
                        const char* str)
{
  t.*(field.field_bool) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Set a double within an object.
 */
template <typename T>
static void set_double(T& t,
                       const typename KeyField<T>::UHandler& field,
                       const char* str)
{
  t.*(field.field_double) = strtod(str, NULL);
  return ;
}

/**
 *  Set an integer within an object.
 */
template <typename T>
static void set_integer(T& t,
                        const typename KeyField<T>::UHandler& field,
                        const char* str)
{
  t.*(field.field_int) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Set a short within an object.
 */
template <typename T>
static void set_short(T& t,
                      const typename KeyField<T>::UHandler& field,
                      const char* str)
{
  t.*(field.field_short) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Set a string within an object.
 */
template <typename T>
static void set_string(T& t,
                       const typename KeyField<T>::UHandler& field,
                       const char* str)
{
  t.*(field.field_string) = str;
  return ;
}

/**
 *  Set a time_t within an object.
 */
template <typename T>
static void set_timet(T& t,
                      const typename KeyField<T>::UHandler& field,
                      const char* str)
{
  t.*(field.field_timet) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Execute an undefined setter.
 */
template <typename T>
static void set_undefined(T& t,
                          const typename KeyField<T>::UHandler& field,
                          const char* str)
{
  field.field_undefined.setter(t, str);
  return ;
}

/**************************************
*                                     *
*             Field Maps              *
*                                     *
**************************************/

/**
 *  Associate a static function to a field that should be set.
 */
template <typename T>
struct   Field
{
  const typename KeyField<T>::UHandler* param;
  void (* ptr)(T&, const typename KeyField<T>::UHandler&, const char*);
};

/**
 *  Static protocol maps.
 */
static std::map<int, Field<Events::Acknowledgement> > acknowledgement_map;
static std::map<int, Field<Events::Comment> >         comment_map;
static std::map<int, Field<Events::Downtime> >        downtime_map;
static std::map<int, Field<Events::Host> >            host_map;
static std::map<int, Field<Events::HostGroup> >       host_group_map;
static std::map<int, Field<Events::HostStatus> >      host_status_map;
static std::map<int, Field<Events::ProgramStatus> >   program_status_map;
static std::map<int, Field<Events::Service> >         service_map;
static std::map<int, Field<Events::ServiceStatus> >   service_status_map;

/**************************************
*                                     *
*          Maps Initializer           *
*                                     *
**************************************/

/**
 *  This class only holds a constructor which is used with the help of a static
 *  object to build the protocol maps.
 */
template <typename T>
static void StaticInit(const KeyField<T> fields[],
                       std::map<int, Field<T> >& map)
{
  for (unsigned int i = 0; fields[i].type; ++i)
    {
      Field<T>& field(map[fields[i].key]);

      field.param = &fields[i].field;
      switch (fields[i].type)
        {
         case 'b':
          field.ptr = &set_boolean<T>;
          break ;
         case 'd':
          field.ptr = &set_double<T>;
          break ;
         case 'i':
          field.ptr = &set_integer<T>;
          break ;
         case 's':
          field.ptr = &set_short<T>;
          break ;
         case 'S':
          field.ptr = &set_string<T>;
          break ;
         case 't':
          field.ptr = &set_timet<T>;
          break ;
         case 'u':
          field.ptr = &set_undefined<T>;
          break ;
         default:
          assert(false);
          abort();
        }
    }
  return ;
}

/**************************************
*                                     *
*           Static Methods            *
*                                     *
**************************************/

/**
 *  Extract event parameters from the data stream.
 */
template <typename T>
T* HandleEvent(IO::Text& stream, const std::map<int, Field<T> >& field_map)
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
          typename std::map<int, Field<T> >::const_iterator it;

          key = strtol(key_str, NULL, 10);
          if (NDO_API_ENDDATA == key)
            break ;
          value_str = strchr(key_str, '=');
          value_str = (value_str ? value_str + 1 : "");
          it = field_map.find(key);
          if (it != field_map.end())
            (*it->second.ptr)(*event.get(), *it->second.param, value_str);
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
Source::Source(const Source& source) : Interface::Source(source), stream_(NULL)
{
  (void)source;
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
                       sizeof(NDO_API_INSTANCENAME)))
            {
              pstatus->is_running = true;
              this->instance_ = value;
            }
          else if (!strncmp(line,
                            NDO_API_STARTTIME,
                            sizeof(NDO_API_STARTTIME)))
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
Source::Source(IO::Stream* stream) : stream_(stream) {}

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
              event.reset(HandleEvent<Events::Acknowledgement>(this->stream_,
                            acknowledgement_map));
              break ;
             case NDO_API_COMMENTDATA:
              event.reset(HandleEvent<Events::Comment>(this->stream_, comment_map));
              break ;
             case NDO_API_DOWNTIMEDATA:
              event.reset(HandleEvent<Events::Downtime>(this->stream_,
                            downtime_map));
              break ;
             case NDO_API_HOSTDEFINITION:
              event.reset(HandleEvent<Events::Host>(this->stream_, host_map));
              break ;
             case NDO_API_HOSTGROUPDEFINITION:
              event.reset(HandleEvent<Events::HostGroup>(this->stream_,
                            host_group_map));
              break ;
             case NDO_API_HOSTSTATUSDATA:
              event.reset(HandleEvent<Events::HostStatus>(this->stream_,
                            host_status_map));
              break ;
             case NDO_API_LOGDATA:
              // XXX
              break ;
             case NDO_API_PROGRAMSTATUSDATA:
              event.reset(HandleEvent<Events::ProgramStatus>(this->stream_,
                            program_status_map));
              break ;
             case NDO_API_SERVICEDEFINITION:
              event.reset(HandleEvent<Events::Service>(this->stream_,
                            service_map));
              break ;
             case NDO_API_SERVICESTATUSDATA:
              event.reset(HandleEvent<Events::ServiceStatus>(this->stream_,
                            service_status_map));
              break ;
            }
        }
    }
  if (event.get())
    event->instance = this->instance_;
  return (event.release());
}

/**
 *  Initialize internal data structures that NDO::Source uses.
 */
void Source::Initialize()
{
  StaticInit<Events::Acknowledgement>(acknowledgement_fields,
                                      acknowledgement_map);
  StaticInit<Events::Comment>(comment_fields, comment_map);
  StaticInit<Events::Downtime>(downtime_fields, downtime_map);
  StaticInit<Events::Host>(host_fields, host_map);
  StaticInit<Events::HostGroup>(host_group_fields, host_group_map);
  StaticInit<Events::HostStatus>(host_status_fields, host_status_map);
  StaticInit<Events::ProgramStatus>(program_status_fields, program_status_map);
  StaticInit<Events::Service>(service_fields, service_map);
  StaticInit<Events::ServiceStatus>(service_status_fields, service_status_map);
  return ;
}

