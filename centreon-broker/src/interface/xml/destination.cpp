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
*          Static Functions           *
*                                     *
**************************************/

/**
 *  Set a boolean on the XML document.
 */
template <typename T>
static void set_boolean(const T& t,
                        const NameField<T>& nf,
                        TiXmlElement& elem)
{
  if (t.*(nf.field.field_bool))
    elem.SetAttribute(nf.name, "true");
  else
    elem.SetAttribute(nf.name, "false");
  return ;
}

/**
 *  Set a double on the XML document.
 */
template <typename T>
static void set_double(const T& t,
                       const NameField<T>& nf,
                       TiXmlElement& elem)
{
  elem.SetAttribute(nf.name, t.*(nf.field.field_double));
  return ;
}

/**
 *  Set an integer on the XML document.
 */
template <typename T>
static void set_integer(const T& t,
                        const NameField<T>& nf,
                        TiXmlElement& elem)
{
  elem.SetAttribute(nf.name, t.*(nf.field.field_int));
  return ;
}

/**
 *  Set a short on the XML document.
 */
template <typename T>
static void set_short(const T& t,
                      const NameField<T>& nf,
                      TiXmlElement& elem)
{
  elem.SetAttribute(nf.name, t.*(nf.field.field_short));
  return ;
}

/**
 *  Set a string on the XML document.
 */
template <typename T>
static void set_string(const T& t,
                       const NameField<T>& nf,
                       TiXmlElement& elem)
{
  elem.SetAttribute(nf.name, t.*(nf.field.field_string));
  return ;
}

/**
 *  Set a time_t on the XML document.
 */
template <typename T>
static void set_timet(const T& t,
                      const NameField<T>& nf,
                      TiXmlElement& elem)
{
  elem.SetAttribute(nf.name, (int)(t.*(nf.field.field_timet)));
  return ;
}

/**************************************
*                                     *
*             Field Lists             *
*                                     *
**************************************/

/**
 *  Associate a static function to a field that should be XML converted.
 */
template <typename T>
struct   Field
{
  const NameField<T>* param;
  void (* ptr)(const T&, const NameField<T>&, TiXmlElement&);
};

/**
 *  Static lists.
 */
static std::list<Field<Events::Acknowledgement> >    acknowledgement_list;
static std::list<Field<Events::Comment> >            comment_list;
static std::list<Field<Events::Downtime> >           downtime_list;
static std::list<Field<Events::Host> >               host_list;
static std::list<Field<Events::HostDependency> >     host_dependency_list;
static std::list<Field<Events::HostGroup> >          host_group_list;
static std::list<Field<Events::HostGroupMember> >    host_group_member_list;
static std::list<Field<Events::HostParent> >         host_parent_list;
static std::list<Field<Events::HostStatus> >         host_status_list;
static std::list<Field<Events::Log> >                log_list;
static std::list<Field<Events::ProgramStatus> >      program_status_list;
static std::list<Field<Events::Service> >            service_list;
static std::list<Field<Events::ServiceDependency> >  service_dependency_list;
static std::list<Field<Events::ServiceGroup> >       service_group_list;
static std::list<Field<Events::ServiceGroupMember> > service_group_member_list;
static std::list<Field<Events::ServiceStatus> >      service_status_list;

/**************************************
*                                     *
*           Static Methods            *
*                                     *
**************************************/

template <typename T>
static void HandleEvent(const T& t,
                        const std::list<Field<T> >& list,
                        TiXmlElement& elem)
{
  typename std::list<Field<T> >::const_iterator end;
  typename std::list<Field<T> >::const_iterator it;

  for (it = list.begin(); it != list.end(); ++it)
    (*it->ptr)(t, *it->param, elem);
  return ;
}

template <typename T>
static void StaticInit(const NameField<T> fields[],
                       std::list<Field<T> >& list)
{
  for (unsigned int i = 0; fields[i].name; ++i)
    {
      list.push_back(Field<T>());

      Field<T>& field(list.back());

      field.param = fields + i;
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
         default:
          assert(false);
          abort();
        }
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
            acknowledgement_list,
            *xml_event);
          break ;
         case Events::Event::COMMENT:
          xml_event.reset(new TiXmlElement("comment"));
          HandleEvent(*static_cast<Events::Comment*>(event),
            comment_list,
            *xml_event);
          break ;
         case Events::Event::DOWNTIME:
          xml_event.reset(new TiXmlElement("downtime"));
          HandleEvent(*static_cast<Events::Downtime*>(event),
            downtime_list,
            *xml_event);
          break ;
         case Events::Event::HOST:
          xml_event.reset(new TiXmlElement("host"));
          HandleEvent(*static_cast<Events::Host*>(event),
            host_list,
            *xml_event);
          break ;
         case Events::Event::HOSTDEPENDENCY:
          xml_event.reset(new TiXmlElement("host_dependency"));
          HandleEvent(*static_cast<Events::HostDependency*>(event),
            host_dependency_list,
            *xml_event);
          break ;
         case Events::Event::HOSTGROUP:
          xml_event.reset(new TiXmlElement("host_group"));
          HandleEvent(*static_cast<Events::HostGroup*>(event),
            host_group_list,
            *xml_event);
          break ;
         case Events::Event::HOSTGROUPMEMBER:
          xml_event.reset(new TiXmlElement("host_group_member"));
          HandleEvent(*static_cast<Events::HostGroupMember*>(event),
             host_group_member_list,
            *xml_event);
          break ;
         case Events::Event::HOSTPARENT:
          xml_event.reset(new TiXmlElement("host_parent"));
          HandleEvent(*static_cast<Events::HostParent*>(event),
            host_parent_list,
            *xml_event);
          break ;
         case Events::Event::HOSTSTATUS:
          xml_event.reset(new TiXmlElement("host_status"));
          HandleEvent(*static_cast<Events::HostStatus*>(event),
            host_status_list,
            *xml_event);
          break ;
         case Events::Event::LOG:
          xml_event.reset(new TiXmlElement("log"));
          HandleEvent(*static_cast<Events::Log*>(event),
            log_list,
            *xml_event);
          break ;
         case Events::Event::PROGRAMSTATUS:
          xml_event.reset(new TiXmlElement("program_status"));
          HandleEvent(*static_cast<Events::ProgramStatus*>(event),
            program_status_list,
            *xml_event);
          break ;
         case Events::Event::SERVICE:
          xml_event.reset(new TiXmlElement("service"));
          HandleEvent(*static_cast<Events::Service*>(event),
            service_list,
            *xml_event);
          break ;
         case Events::Event::SERVICEDEPENDENCY:
          xml_event.reset(new TiXmlElement("service_dependency"));
          HandleEvent(*static_cast<Events::ServiceDependency*>(event),
            service_dependency_list,
            *xml_event);
          break ;
         case Events::Event::SERVICEGROUP:
          xml_event.reset(new TiXmlElement("service_group"));
          HandleEvent(*static_cast<Events::ServiceGroup*>(event),
            service_group_list,
            *xml_event);
          break ;
         case Events::Event::SERVICEGROUPMEMBER:
          xml_event.reset(new TiXmlElement("service_group_member"));
          HandleEvent(*static_cast<Events::ServiceGroupMember*>(event),
            service_group_member_list,
            *xml_event);
          break ;
         case Events::Event::SERVICESTATUS:
          xml_event.reset(new TiXmlElement("service_status"));
          HandleEvent(*static_cast<Events::ServiceStatus*>(event),
            service_status_list,
            *xml_event);
          break ;
         default:
          return ;
        }

      // Send XML document.
      doc->LinkEndChild(xml_event.get());
      xml_event.release();
      str << *doc;
      this->stream_->Send(str.c_str(), str.size());
    }
  catch (...) {}

  // Self event deregistration.
  event->RemoveReader();
  return ;
}

/**
 *  Initialize necessary data structures.
 */
void Destination::Initialize()
{
  StaticInit(acknowledgement_fields, acknowledgement_list);
  StaticInit(comment_fields, comment_list);
  StaticInit(downtime_fields, downtime_list);
  StaticInit(host_fields, host_list);
  StaticInit(host_dependency_fields, host_dependency_list);
  StaticInit(host_group_fields, host_group_list);
  StaticInit(host_group_member_fields, host_group_member_list);
  StaticInit(host_parent_fields, host_parent_list);
  StaticInit(host_status_fields, host_status_list);
  StaticInit(log_fields, log_list);
  StaticInit(program_status_fields, program_status_list);
  StaticInit(service_fields, service_list);
  StaticInit(service_dependency_fields, service_dependency_list);
  StaticInit(service_group_fields, service_group_list);
  StaticInit(service_group_member_fields, service_group_member_list);
  StaticInit(service_status_fields, service_status_list);
  return ;
}
