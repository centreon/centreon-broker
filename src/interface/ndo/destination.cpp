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
#include <sstream>                     // for stringstream
#include <stdlib.h>                    // for abort
#include "events/events.h"
#include "interface/ndo/destination.h"
#include "interface/ndo/internal.h"
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
static void set_boolean(const T& t,
                        int id,
                        const typename KeyField<T>::FieldPointer& field,
                        std::stringstream& buffer)
{
  buffer << id << "=" << (t.*(field.field_bool) ? "1" : "0") << "\n";
  return ;
}

/**
 *  Set a double within an object.
 */
template <typename T>
static void set_double(const T& t,
                       int id,
                       const typename KeyField<T>::FieldPointer& field,
                       std::stringstream& buffer)
{
  buffer << id << "=" << t.*(field.field_double) << "\n";
  return ;
}

/**
 *  Set an integer within an object.
 */
template <typename T>
static void set_integer(const T& t,
                        int id,
                        const typename KeyField<T>::FieldPointer& field,
                        std::stringstream& buffer)
{
  buffer << id << "=" << t.*(field.field_int) << "\n";
  return ;
}

/**
 *  Set a short within an object.
 */
template <typename T>
static void set_short(const T& t,
                      int id,
                      const typename KeyField<T>::FieldPointer& field,
                      std::stringstream& buffer)
{
  buffer << id << "=" << t.*(field.field_short) << "\n";
  return ;
}

/**
 *  Set a string within an object.
 */
template <typename T>
static void set_string(const T& t,
                       int id,
                       const typename KeyField<T>::FieldPointer& field,
                       std::stringstream& buffer)
{
  buffer << id << "=" << t.*(field.field_string) << "\n";
  return ;
}

/**
 *  Set a time_t within an object.
 */
template <typename T>
static void set_timet(const T& t,
                      int id,
                      const typename KeyField<T>::FieldPointer& field,
                      std::stringstream& buffer)
{
  buffer << id << "=" << t.*(field.field_timet) << "\n";
  return ;
}

/**
 *  Execute an undefined setter.
 */
template <typename T>
static void set_undefined(const T& t,
                          int id,
                          const typename KeyField<T>::FieldPointer& field,
                          std::stringstream& buffer)
{
  (void)id;
  if (field.field_undefined.getter)
    buffer << field.field_undefined.getter(t);
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
  const typename KeyField<T>::FieldPointer* param;
  void (* ptr)(const T&,
               int id,
               const typename KeyField<T>::FieldPointer&,
               std::stringstream& buffer);
};

/**
 *  Static protocol maps.
 */
static std::map<int, Field<Events::Acknowledgement> >   acknowledgement_map;
static std::map<int, Field<Events::Comment> >           comment_map;
static std::map<int, Field<Events::Downtime> >          downtime_map;
static std::map<int, Field<Events::Host> >              host_map;
static std::map<int, Field<Events::HostDependency> >    host_dependency_map;
static std::map<int, Field<Events::HostGroup> >         host_group_map;
static std::map<int, Field<Events::HostGroupMember> >   host_group_member_map;
static std::map<int, Field<Events::HostParent> >        host_parent_map;
static std::map<int, Field<Events::HostStatus> >        host_status_map;
static std::map<int, Field<Events::Log> >               log_map;
static std::map<int, Field<Events::ProgramStatus> >     program_status_map;
static std::map<int, Field<Events::Service> >           service_map;
static std::map<int, Field<Events::ServiceDependency> > service_dependency_map;
static std::map<int, Field<Events::ServiceGroup> >      service_group_map;
static std::map<int, Field<Events::ServiceGroupMember> >
  service_group_member_map;
static std::map<int, Field<Events::ServiceStatus> >     service_status_map;

/**************************************
*                                     *
*          Maps Initializer           *
*                                     *
**************************************/

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
 *  Extract event parameters and send them to the data stream.
 */
template <typename T>
void HandleEvent(const T& event,
                 const std::map<int, Field<T> >& field_map,
                 std::stringstream& buffer)
{
  typename std::map<int, Field<T> >::const_iterator end;

  end = field_map.end();
  for (typename std::map<int, Field<T> >::const_iterator it = field_map.begin();
       it != end;
       ++it)
    (it->second.ptr)(event, it->first, *it->second.param, buffer);
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Destination copy constructor.
 *
 *  As Destination is not copyable, any attempt to use the copy constructor
 *  will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] destination Unused.
 */
Destination::Destination(const Destination& destination)
  : Base(NULL), Interface::Destination(destination)
{
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  As Destination is not copyable, any attempt to use the assignment operator
 *  will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] destination Unused.
 *
 *  \return *this
 */
Destination& Destination::operator=(const Destination& destination)
{
  (void)destination;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Destination constructor.
 *
 *  Build an NDO destination object that uses the stream object as its output.
 *  The stream object must not be NULL and is owned by the Destination object
 *  upon successful return from the constructor.
 *
 *  \param[in] stream Output stream object.
 */
Destination::Destination(IO::Stream* stream) : Base(stream) {}

/**
 *  Destination destructor.
 */
Destination::~Destination() {}

/**
 *  Close the destination object.
 */
void Destination::Close()
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
void Destination::Event(Events::Event* event)
{
  try
    {
      std::stringstream buffer;

      switch (event->GetType())
        {
         case Events::Event::ACKNOWLEDGEMENT:
          buffer << NDO_API_ACKNOWLEDGEMENTDATA << ":\n";
          HandleEvent<Events::Acknowledgement>(
            *static_cast<Events::Acknowledgement*>(event),
            acknowledgement_map,
            buffer);
          buffer << NDO_API_ENDDATA << "\n";
          break ;
         case Events::Event::COMMENT:
          buffer << NDO_API_COMMENTDATA << ":\n";
          HandleEvent<Events::Comment>(
            *static_cast<Events::Comment*>(event),
            comment_map,
            buffer);
          buffer << NDO_API_ENDDATA << "\n";
          break ;
         case Events::Event::DOWNTIME:
          buffer << NDO_API_DOWNTIMEDATA << ":\n";
          HandleEvent<Events::Downtime>(
            *static_cast<Events::Downtime*>(event),
            downtime_map,
            buffer);
          buffer << NDO_API_ENDDATA << "\n";
          break ;
         case Events::Event::HOST:
          buffer << NDO_API_HOSTDEFINITION << ":\n";
          HandleEvent<Events::Host>(
            *static_cast<Events::Host*>(event),
            host_map,
            buffer);
          buffer << NDO_API_ENDDATA << "\n";
          break ;
         case Events::Event::HOSTDEPENDENCY:
          buffer << NDO_API_HOSTDEPENDENCYDEFINITION << ":\n";
          HandleEvent<Events::HostDependency>(
            *static_cast<Events::HostDependency*>(event),
            host_dependency_map,
            buffer);
          buffer << NDO_API_ENDDATA << "\n";
          break ;
         case Events::Event::HOSTGROUP:
          buffer << NDO_API_HOSTGROUPDEFINITION << ":\n";
          HandleEvent<Events::HostGroup>(
            *static_cast<Events::HostGroup*>(event),
            host_group_map,
            buffer);
          buffer << NDO_API_ENDDATA << "\n";
          break ;
         case Events::Event::HOSTGROUPMEMBER:
          buffer << NDO_API_HOSTGROUPMEMBERDEFINITION << ":\n";
          HandleEvent<Events::HostGroupMember>(
            *static_cast<Events::HostGroupMember*>(event),
            host_group_member_map,
            buffer);
          buffer << NDO_API_ENDDATA << "\n";
          break ;
         case Events::Event::HOSTPARENT:
          buffer << NDO_API_HOSTPARENT << ":\n";
          HandleEvent<Events::HostParent>(
            *static_cast<Events::HostParent*>(event),
            host_parent_map,
            buffer);
          buffer << NDO_API_ENDDATA << "\n";
          break ;
         case Events::Event::HOSTSTATUS:
          buffer << NDO_API_HOSTSTATUSDATA << ":\n";
          HandleEvent<Events::HostStatus>(
            *static_cast<Events::HostStatus*>(event),
            host_status_map,
            buffer);
          buffer << NDO_API_ENDDATA << "\n";
          break ;
         case Events::Event::LOG:
          buffer << NDO_API_LOGDATA << ":\n";
          HandleEvent<Events::Log>(
            *static_cast<Events::Log*>(event),
            log_map,
            buffer);
          buffer << NDO_API_ENDDATA << "\n";
          break ;
         case Events::Event::PROGRAMSTATUS:
          buffer << NDO_API_PROGRAMSTATUSDATA << ":\n";
          HandleEvent<Events::ProgramStatus>(
            *static_cast<Events::ProgramStatus*>(event),
            program_status_map,
            buffer);
          buffer << NDO_API_ENDDATA << "\n";
          break ;
         case Events::Event::SERVICE:
          buffer << NDO_API_SERVICEDEFINITION << ":\n";
          HandleEvent<Events::Service>(
            *static_cast<Events::Service*>(event),
            service_map,
            buffer);
          buffer << NDO_API_ENDDATA << "\n";
          break ;
         case Events::Event::SERVICEDEPENDENCY:
          buffer << NDO_API_SERVICEDEPENDENCYDEFINITION << ":\n";
          HandleEvent<Events::ServiceDependency>(
            *static_cast<Events::ServiceDependency*>(event),
            service_dependency_map,
            buffer);
          buffer << NDO_API_ENDDATA << ":\n";
         case Events::Event::SERVICEGROUP:
          buffer << NDO_API_SERVICEGROUPDEFINITION << ":\n";
          HandleEvent<Events::ServiceGroup>(
            *static_cast<Events::ServiceGroup*>(event),
            service_group_map,
            buffer);
          buffer << NDO_API_ENDDATA << "\n";
          break ;
         case Events::Event::SERVICEGROUPMEMBER:
          buffer << NDO_API_SERVICEGROUPMEMBERDEFINITION << ":\n";
          HandleEvent<Events::ServiceGroupMember>(
            *static_cast<Events::ServiceGroupMember*>(event),
            service_group_member_map,
            buffer);
          buffer << NDO_API_ENDDATA << "\n";
          break ;
         case Events::Event::SERVICESTATUS:
          buffer << NDO_API_SERVICESTATUSDATA << ":\n";
          HandleEvent<Events::ServiceStatus>(
            *static_cast<Events::ServiceStatus*>(event),
            service_status_map,
            buffer);
          buffer << NDO_API_ENDDATA << "\n";
          break ;
        }
      buffer << "\n";

      // Send data.
      this->stream_.Send(buffer.str().c_str(), buffer.str().size());
    }
  catch (...) {}

  // Self event deregistration.
  event->RemoveReader();

  return ;
}

/**
 *  Initialize internal data structures that NDO::Destination uses.
 */
void Destination::Initialize()
{
  StaticInit<Events::Acknowledgement>(acknowledgement_fields,
                                      acknowledgement_map);
  StaticInit<Events::Comment>(comment_fields, comment_map);
  StaticInit<Events::Downtime>(downtime_fields, downtime_map);
  StaticInit<Events::Host>(host_fields, host_map);
  StaticInit<Events::HostDependency>(host_dependency_fields,
                                     host_dependency_map);
  StaticInit<Events::HostGroup>(host_group_fields, host_group_map);
  StaticInit<Events::HostGroupMember>(host_group_member_fields,
                                      host_group_member_map);
  StaticInit<Events::HostParent>(host_parent_fields, host_parent_map);
  StaticInit<Events::HostStatus>(host_status_fields, host_status_map);
  StaticInit<Events::Log>(log_fields, log_map);
  StaticInit<Events::ProgramStatus>(program_status_fields, program_status_map);
  StaticInit<Events::Service>(service_fields, service_map);
  StaticInit<Events::ServiceDependency>(service_dependency_fields,
                                        service_dependency_map);
  StaticInit<Events::ServiceGroup>(service_group_fields, service_group_map);
  StaticInit<Events::ServiceGroupMember>(service_group_member_fields,
                                         service_group_member_map);
  StaticInit<Events::ServiceStatus>(service_status_fields, service_status_map);
  return ;
}
