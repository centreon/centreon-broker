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
*           Static Methods            *
*                                     *
**************************************/

/**
 *  Extract event parameters and send them to the data stream.
 */
template <typename T>
static void HandleEvent(const T& event,
                        const std::map<int, GetterSetter<T> >& member_map,
                        std::stringstream& buffer)
{
  typename std::map<int, GetterSetter<T> >::const_iterator end;

  end = member_map.end();
  for (typename std::map<int, GetterSetter<T> >::const_iterator
         it = member_map.begin();
       it != end;
       ++it)
    {
      buffer << it->first << "=";
      (it->second.getter)(event, *it->second.member, buffer);
      buffer << "\n";
    }
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
