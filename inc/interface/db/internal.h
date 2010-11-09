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

#ifndef INTERFACE_DB_INTERNAL_H_
# define INTERFACE_DB_INTERNAL_H_

# include <list>
# include <soci.h>
# include <string>
# include <utility>
# include "events/events.h"
# include "mapping.h"

namespace                  Interface
{
  namespace                DB
  {
    template               <typename T>
    struct                 GetterSetter
    {
      const DataMember<T>* member;
      void                 (* getter)(const T&,
                                      const std::string& name,
                                      const DataMember<T>&,
                                      soci::values&);
    };

    // DB mappings.
    template               <typename T>
    struct                 DBMappedType
    {
      static std::list<std::pair<std::string, GetterSetter<T> > > list;
    };

    // Mapping initialization routine.
    void Initialize();
  }
}

namespace                  soci
{
  // Acknowledgement O/R mapping.
  template                 <>
  struct                   type_conversion<Events::Acknowledgement>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::Acknowledgement& a);
    static void            to_base(const Events::Acknowledgement& a,
                                   values& v,
                                   indicator& ind);
  };

  // Comment O/R mapping.
  template                 <>
  struct                   type_conversion<Events::comment>
  {
    typedef values         base_type;
    static void            from_base(values const& v,
                                     indicator ind,
                                     Events::comment& c);
    static void            to_base(Events::comment const& c,
                                   values& v,
                                   indicator& ind);
  };

  // Downtime O/R mapping.
  template                 <>
  struct                   type_conversion<Events::Downtime>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::Downtime& d);
    static void            to_base(const Events::Downtime& d,
                                   values& v,
                                   indicator& ind);
  };

  // event_handler O/R mapping.
  template                 <>
  struct                   type_conversion<Events::event_handler> {
    typedef values         base_type;
    static void            from_base(values const& v,
                                     indicator ind,
                                     Events::event_handler& eh);
    static void            to_base(Events::event_handler const& eh,
                                   values& v,
                                   indicator& ind);
  };

  // flapping_status O/R mapping.
  template                 <>
  struct                   type_conversion<Events::flapping_status> {
    typedef values         base_type;
    static void            from_base(values const& v,
                                     indicator ind,
                                     Events::flapping_status& fs);
    static void            to_base(Events::flapping_status const& fs,
                                   values& v,
                                   indicator& ind);
  };

  // Host O/R mapping.
  template                 <>
  struct                   type_conversion<Events::Host>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::Host& h);
    static void            to_base(const Events::Host& h,
                                   values& v,
                                   indicator& ind);
  };

  // HostCheck O/R mapping.
  template                 <>
  struct                   type_conversion<Events::HostCheck>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::HostCheck& hc);
    static void            to_base(const Events::HostCheck& hc,
                                   values& v,
                                   indicator& ind);
  };

  // HostDependency O/R mapping.
  template                 <>
  struct                   type_conversion<Events::HostDependency>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::HostDependency& hd);
    static void            to_base(const Events::HostDependency& hd,
                                   values& v,
                                   indicator& ind);
  };

  // HostGroup O/R mapping.
  template                 <>
  struct                   type_conversion<Events::HostGroup>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::HostGroup& hg);
    static void            to_base(const Events::HostGroup& hg,
                                   values& v,
                                   indicator& ind);
  };

  // HostGroupMember O/R mapping.
  template                 <>
  struct                   type_conversion<Events::HostGroupMember>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::HostGroupMember& hgm);
    static void            to_base(const Events::HostGroupMember& hgm,
                                   values& v,
                                   indicator& ind);
  };

  // HostParent O/R mapping.
  template                 <>
  struct                   type_conversion<Events::HostParent>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::HostParent& hp);
    static void            to_base(const Events::HostParent& hp,
                                   values& v,
                                   indicator& ind);
  };

  // HostStatus O/R mapping.
  template                 <>
  struct                   type_conversion<Events::HostStatus>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::HostStatus& hs);
    static void            to_base(const Events::HostStatus& hs,
                                   values& v,
                                   indicator& ind);
  };

  // Issue O/R mapping.
  template                 <>
  struct                   type_conversion<Events::Issue>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::Issue& i);
    static void            to_base(const Events::Issue& i,
                                   values& v,
                                   indicator& ind);
  };

  // Log O/R mapping.
  template                 <>
  struct                   type_conversion<Events::Log>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::Log& l);
    static void            to_base(const Events::Log& l,
                                   values& v,
                                   indicator& ind);
  };

  // Notification O/R mapping.
  template                 <>
  struct                   type_conversion<Events::notification> {
    typedef values         base_type;
    static void            from_base(values const& v,
                                     indicator ind,
                                     Events::notification& n);
    static void            to_base(Events::notification const& n,
                                   values& v,
                                   indicator& ind);
  };

  // Program O/R mapping.
  template                 <>
  struct                   type_conversion<Events::Program>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::Program& program);
    static void            to_base(const Events::Program& program,
                                   values& v,
                                   indicator& ind);
  };

  // ProgramStatus O/R mapping.
  template                 <>
  struct                   type_conversion<Events::ProgramStatus>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::ProgramStatus& ps);
    static void            to_base(const Events::ProgramStatus& ps,
                                   values& v,
                                   indicator& ind);
  };

  // Service O/R mapping.
  template                 <>
  struct                   type_conversion<Events::Service>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::Service& s);
    static void            to_base(const Events::Service& s,
                                   values& v,
                                   indicator& ind);
  };

  // ServiceCheck O/R mapping.
  template                 <>
  struct                   type_conversion<Events::ServiceCheck>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::ServiceCheck& sc);
    static void            to_base(const Events::ServiceCheck& sc,
                                   values& v,
                                   indicator& ind);
  };

  // ServiceDependency O/R mapping.
  template                 <>
  struct                   type_conversion<Events::ServiceDependency>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::ServiceDependency& sd);
    static void            to_base(const Events::ServiceDependency& sd,
                                   values& v,
                                   indicator& ind);
  };

  // ServiceGroup O/R mapping.
  template                 <>
  struct                   type_conversion<Events::ServiceGroup>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::ServiceGroup& sg);
    static void            to_base(const Events::ServiceGroup& sg,
                                   values& v,
                                   indicator& ind);
  };

  // ServiceGroupMember O/R mapping.
  template                 <>
  struct                   type_conversion<Events::ServiceGroupMember>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::ServiceGroupMember& sgm);
    static void            to_base(const Events::ServiceGroupMember& sgm,
                                   values& v,
                                   indicator& ind);
  };

  // ServiceStatus O/R mapping.
  template                 <>
  struct                   type_conversion<Events::ServiceStatus>
  {
    typedef values         base_type;
    static void            from_base(const values& v,
                                     indicator ind,
                                     Events::ServiceStatus& ss);
    static void            to_base(const Events::ServiceStatus& ss,
                                   values& v,
                                   indicator& ind);
  };

  // State O/R mapping.
  template                 <>
  struct                   type_conversion<Events::state> {
    typedef values         base_type;
    static void            from_base(values const& v,
                             indicator ind,
                             Events::state& s);
    static void            to_base(Events::state const& s,
                             values& v,
                             indicator& ind);
  };
}

#endif /* !INTERFACE_DB_INTERNAL_H_ */
