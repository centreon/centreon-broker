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
                        TiXmlElement& elem)
{
  typename std::map<std::string, GetterSetter<T> >::const_iterator end;
  typename std::map<std::string, GetterSetter<T> >::const_iterator it;

  for (it = XMLMappedType<T>::map.begin(), end = XMLMappedType<T>::map.end();
       it != end;
       ++it)
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
            *xml_event);
          break ;
         case Events::Event::COMMENT:
          xml_event.reset(new TiXmlElement("comment"));
          HandleEvent(*static_cast<Events::comment*>(event),
            *xml_event);
          break ;
         case Events::Event::CUSTOMVARIABLE:
          xml_event.reset(new TiXmlElement("custom_variable"));
          HandleEvent(*static_cast<Events::custom_variable*>(event),
            *xml_event);
          break ;
         case Events::Event::DOWNTIME:
          xml_event.reset(new TiXmlElement("downtime"));
          HandleEvent(*static_cast<Events::Downtime*>(event),
            *xml_event);
          break ;
         case Events::Event::EVENTHANDLER:
          xml_event.reset(new TiXmlElement("event_handler"));
          HandleEvent(*static_cast<Events::event_handler*>(event),
            *xml_event);
          break ;
         case Events::Event::HOST:
          xml_event.reset(new TiXmlElement("host"));
          HandleEvent(*static_cast<Events::Host*>(event),
            *xml_event);
          break ;
         case Events::Event::HOSTDEPENDENCY:
          xml_event.reset(new TiXmlElement("host_dependency"));
          HandleEvent(*static_cast<Events::HostDependency*>(event),
            *xml_event);
          break ;
         case Events::Event::HOSTGROUP:
          xml_event.reset(new TiXmlElement("host_group"));
          HandleEvent(*static_cast<Events::HostGroup*>(event),
            *xml_event);
          break ;
         case Events::Event::HOSTGROUPMEMBER:
          xml_event.reset(new TiXmlElement("host_group_member"));
          HandleEvent(*static_cast<Events::HostGroupMember*>(event),
            *xml_event);
          break ;
         case Events::Event::HOSTPARENT:
          xml_event.reset(new TiXmlElement("host_parent"));
          HandleEvent(*static_cast<Events::HostParent*>(event),
            *xml_event);
          break ;
         case Events::Event::HOSTSTATUS:
          xml_event.reset(new TiXmlElement("host_status"));
          HandleEvent(*static_cast<Events::HostStatus*>(event),
            *xml_event);
          break ;
         case Events::Event::LOG:
          xml_event.reset(new TiXmlElement("log"));
          HandleEvent(*static_cast<Events::Log*>(event),
            *xml_event);
          break ;
         case Events::Event::NOTIFICATION:
          xml_event.reset(new TiXmlElement("notification"));
          HandleEvent(*static_cast<Events::notification*>(event),
            *xml_event);
          break ;
         case Events::Event::PROGRAM:
          xml_event.reset(new TiXmlElement("program"));
          HandleEvent(*static_cast<Events::Program*>(event),
            *xml_event);
          break ;
         case Events::Event::PROGRAMSTATUS:
          xml_event.reset(new TiXmlElement("program_status"));
          HandleEvent(*static_cast<Events::ProgramStatus*>(event),
            *xml_event);
          break ;
         case Events::Event::SERVICE:
          xml_event.reset(new TiXmlElement("service"));
          HandleEvent(*static_cast<Events::Service*>(event),
            *xml_event);
          break ;
         case Events::Event::SERVICEDEPENDENCY:
          xml_event.reset(new TiXmlElement("service_dependency"));
          HandleEvent(*static_cast<Events::ServiceDependency*>(event),
            *xml_event);
          break ;
         case Events::Event::SERVICEGROUP:
          xml_event.reset(new TiXmlElement("service_group"));
          HandleEvent(*static_cast<Events::ServiceGroup*>(event),
            *xml_event);
          break ;
         case Events::Event::SERVICEGROUPMEMBER:
          xml_event.reset(new TiXmlElement("service_group_member"));
          HandleEvent(*static_cast<Events::ServiceGroupMember*>(event),
            *xml_event);
          break ;
         case Events::Event::SERVICESTATUS:
          xml_event.reset(new TiXmlElement("service_status"));
          HandleEvent(*static_cast<Events::ServiceStatus*>(event),
            *xml_event);
          break ;
         case Events::Event::STATE:
          xml_event.reset(new TiXmlElement("state"));
          HandleEvent(*static_cast<Events::state*>(event),
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
