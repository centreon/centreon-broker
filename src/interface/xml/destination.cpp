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
#include <memory>                      // for auto_ptr
#include <sstream>
#include <stdlib.h>                    // for abort
#include "events/events.h"
#include "interface/xml/destination.h"
#include "interface/xml/internal.h"
#include "interface/xml/tinyxml.h"
#include "io/stream.h"

using namespace Interface::XML;

/**************************************
*                                     *
*           Static Methods            *
*                                     *
**************************************/

/**
 *  Extract event parameters and send them to the data stream.
 */
template <typename T>
static void HandleEvent(const T& t,
                        const std::map<std::string, GetterSetter<T> >& members,
                        TiXmlElement& elem)
{
  typename std::map<std::string, GetterSetter<T> >::const_iterator end;
  typename std::map<std::string, GetterSetter<T> >::const_iterator it;

  for (it = members.begin(), end = members.end(); it != end; ++it)
    (it->second.getter)(t, it->first, *it->second.member, elem);
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
 *  Destination is not copyable. Therefore any attempt to use the copy
 *  constructor will result in a call to abort.
 *
 *  \param[in] dest Unused.
 */
Destination::Destination(const Destination& dest)
  : Interface::Destination(dest)
{
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  Destination is not copyable. Therefore any attempt to use the assignment
 *  operator will result in a call to abort.
 *
 *  \param[in] dest Unused.
 *
 *  \return *this
 */
Destination& Destination::operator=(const Destination& dest)
{
  (void)dest;
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
 *  Destination constructor.
 *
 *  \param[in] stream Stream on which data will be sent.
 */
Destination::Destination(IO::Stream* stream) : stream_(stream) {}

/**
 *  Destination destructor.
 */
Destination::~Destination() {}

/**
 *  Close the underlying stream.
 */
void Destination::Close()
{
  this->stream_->Close();
  return ;
}

/**
 *  Dump event to the stream.
 *
 *  \param[in] event Event to dump.
 */
void Destination::Event(Events::Event* event)
{
  try
    {
      std::auto_ptr<TiXmlDocument> doc(new TiXmlDocument);
      std::auto_ptr<TiXmlElement> xml_event;
      std::string str;

      switch (event->GetType())
        {
         case Events::Event::ACKNOWLEDGEMENT:
          xml_event.reset(new TiXmlElement("acknowledgement"));
          HandleEvent(*static_cast<Events::Acknowledgement*>(event),
            acknowledgement_map,
            *xml_event);
          break ;
         case Events::Event::COMMENT:
          xml_event.reset(new TiXmlElement("comment"));
          HandleEvent(*static_cast<Events::Comment*>(event),
            comment_map,
            *xml_event);
          break ;
         case Events::Event::DOWNTIME:
          xml_event.reset(new TiXmlElement("downtime"));
          HandleEvent(*static_cast<Events::Downtime*>(event),
            downtime_map,
            *xml_event);
          break ;
         case Events::Event::HOST:
          xml_event.reset(new TiXmlElement("host"));
          HandleEvent(*static_cast<Events::Host*>(event),
            host_map,
            *xml_event);
          break ;
         case Events::Event::HOSTDEPENDENCY:
          xml_event.reset(new TiXmlElement("host_dependency"));
          HandleEvent(*static_cast<Events::HostDependency*>(event),
            host_dependency_map,
            *xml_event);
          break ;
         case Events::Event::HOSTGROUP:
          xml_event.reset(new TiXmlElement("host_group"));
          HandleEvent(*static_cast<Events::HostGroup*>(event),
            host_group_map,
            *xml_event);
          break ;
         case Events::Event::HOSTGROUPMEMBER:
          xml_event.reset(new TiXmlElement("host_group_member"));
          HandleEvent(*static_cast<Events::HostGroupMember*>(event),
             host_group_member_map,
            *xml_event);
          break ;
         case Events::Event::HOSTPARENT:
          xml_event.reset(new TiXmlElement("host_parent"));
          HandleEvent(*static_cast<Events::HostParent*>(event),
            host_parent_map,
            *xml_event);
          break ;
         case Events::Event::HOSTSTATUS:
          xml_event.reset(new TiXmlElement("host_status"));
          HandleEvent(*static_cast<Events::HostStatus*>(event),
            host_status_map,
            *xml_event);
          break ;
         case Events::Event::LOG:
          xml_event.reset(new TiXmlElement("log"));
          HandleEvent(*static_cast<Events::Log*>(event),
            log_map,
            *xml_event);
          break ;
         case Events::Event::PROGRAMSTATUS:
          xml_event.reset(new TiXmlElement("program_status"));
          HandleEvent(*static_cast<Events::ProgramStatus*>(event),
            program_status_map,
            *xml_event);
          break ;
         case Events::Event::SERVICE:
          xml_event.reset(new TiXmlElement("service"));
          HandleEvent(*static_cast<Events::Service*>(event),
            service_map,
            *xml_event);
          break ;
         case Events::Event::SERVICEDEPENDENCY:
          xml_event.reset(new TiXmlElement("service_dependency"));
          HandleEvent(*static_cast<Events::ServiceDependency*>(event),
            service_dependency_map,
            *xml_event);
          break ;
         case Events::Event::SERVICEGROUP:
          xml_event.reset(new TiXmlElement("service_group"));
          HandleEvent(*static_cast<Events::ServiceGroup*>(event),
            service_group_map,
            *xml_event);
          break ;
         case Events::Event::SERVICEGROUPMEMBER:
          xml_event.reset(new TiXmlElement("service_group_member"));
          HandleEvent(*static_cast<Events::ServiceGroupMember*>(event),
            service_group_member_map,
            *xml_event);
          break ;
         case Events::Event::SERVICESTATUS:
          xml_event.reset(new TiXmlElement("service_status"));
          HandleEvent(*static_cast<Events::ServiceStatus*>(event),
            service_status_map,
            *xml_event);
          break ;
         default:
          return ;
        }

      // Send XML document.
      doc->LinkEndChild(xml_event.get());
      xml_event.release();
      str << *doc;
      str.append("\n");
      this->stream_->Send(str.c_str(), str.size());
    }
  catch (...) {}

  // Self event deregistration.
  event->RemoveReader();

  return ;
}
