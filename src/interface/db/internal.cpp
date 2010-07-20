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
#include <stdlib.h>                // for abort
#include "interface/db/internal.h"

using namespace Interface::DB;

/**************************************
*                                     *
*          Static Functions           *
*                                     *
**************************************/

/**
 *  Get a boolean from an object.
 */
template <typename T>
static void get_boolean(const T& t,
                        const std::string& name,
                        const DataMember<T>& member,
                        soci::values& v)
{
  v.set(name, (int)(t.*(member.b)));
  return ;
}

/**
 *  Get a double from an object.
 */
template <typename T>
static void get_double(const T& t,
                       const std::string& name,
                       const DataMember<T>& member,
                       soci::values& v)
{
  v.set(name, t.*(member.d));
  return ;
}

/**
 *  Get an integer from an object.
 */
template <typename T>
static void get_integer(const T& t,
                        const std::string& name,
                        const DataMember<T>& member,
                        soci::values& v)
{
  v.set(name, t.*(member.i));
  return ;
}

/**
 *  Get a short from an object.
 */
template <typename T>
static void get_short(const T& t,
                      const std::string& name,
                      const DataMember<T>& member,
                      soci::values& v)
{
  v.set(name, t.*(member.s));
  return ;
}

/**
 *  Get a string from an object.
 */
template <typename T>
static void get_string(const T& t,
                       const std::string& name,
                       const DataMember<T>& member,
                       soci::values& v)
{
  v.set(name, t.*(member.S));
  return ;
}

/**
 *  Get a time_t from an object.
 */
template <typename T>
static void get_timet(const T& t,
                      const std::string& name,
                      const DataMember<T>& member,
                      soci::values& v)
{
  v.set(name, (int)(t.*(member.t)));
  return ;
}

/**
 *  Static initialization template used by Initialize().
 */
template <typename T>
static void static_init()
{
  for (unsigned int i = 0; MappedType<T>::members[i].type; ++i)
    if (MappedType<T>::members[i].name)
      {
        DBMappedType<T>::list.push_back(
          std::make_pair(MappedType<T>::members[i].name,
                         GetterSetter<T>()));

        GetterSetter<T>& gs(DBMappedType<T>::list.back().second);

        gs.member = &MappedType<T>::members[i].member;
        // XXX : setters are not set.
        switch (MappedType<T>::members[i].type)
          {
           case MappedData<T>::BOOL:
            gs.getter = &get_boolean<T>;
            break ;
           case MappedData<T>::DOUBLE:
            gs.getter = &get_double<T>;
            break ;
           case MappedData<T>::INT:
            gs.getter = &get_integer<T>;
            break ;
           case MappedData<T>::SHORT:
            gs.getter = &get_short<T>;
            break ;
           case MappedData<T>::STRING:
            gs.getter = &get_string<T>;
            break ;
           case MappedData<T>::TIME_T:
            gs.getter = &get_timet<T>;
            break ;
           default: // Error in one of the mappings.
            assert(false);
            abort();
          }
      }
  return ;
}

/**
 *  Extract data from object to DB row.
 */
template <typename T>
static void ToBase(const T& t, soci::values& v)
{
  for (typename std::list<std::pair<std::string,
                                    GetterSetter<T> > >::const_iterator
         it = DBMappedType<T>::list.begin(),
         end = DBMappedType<T>::list.end();
       it != end;
       ++it)
    (it->second.getter)(t, it->first, *it->second.member, v);
  return ;
}

/**************************************
*                                     *
*           Global Objects            *
*                                     *
**************************************/

template <> std::list<std::pair<std::string, GetterSetter<Events::Acknowledgement> > >
  Interface::DB::DBMappedType<Events::Acknowledgement>::list =
    std::list<std::pair<std::string, GetterSetter<Events::Acknowledgement> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::Comment> > >
  Interface::DB::DBMappedType<Events::Comment>::list =
    std::list<std::pair<std::string, GetterSetter<Events::Comment> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::Downtime> > >
  Interface::DB::DBMappedType<Events::Downtime>::list =
    std::list<std::pair<std::string, GetterSetter<Events::Downtime> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::Host> > >
  Interface::DB::DBMappedType<Events::Host>::list =
    std::list<std::pair<std::string, GetterSetter<Events::Host> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::HostCheck> > >
  Interface::DB::DBMappedType<Events::HostCheck>::list =
    std::list<std::pair<std::string, GetterSetter<Events::HostCheck> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::HostDependency> > >
  Interface::DB::DBMappedType<Events::HostDependency>::list =
    std::list<std::pair<std::string, GetterSetter<Events::HostDependency> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::HostGroup> > >
  Interface::DB::DBMappedType<Events::HostGroup>::list =
    std::list<std::pair<std::string, GetterSetter<Events::HostGroup> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::HostGroupMember> > >
  Interface::DB::DBMappedType<Events::HostGroupMember>::list =
    std::list<std::pair<std::string, GetterSetter<Events::HostGroupMember> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::HostParent> > >
  Interface::DB::DBMappedType<Events::HostParent>::list =
    std::list<std::pair<std::string, GetterSetter<Events::HostParent> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::HostStatus> > >
  Interface::DB::DBMappedType<Events::HostStatus>::list =
    std::list<std::pair<std::string, GetterSetter<Events::HostStatus> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::Issue> > >
  Interface::DB::DBMappedType<Events::Issue>::list =
    std::list<std::pair<std::string, GetterSetter<Events::Issue> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::Log> > >
  Interface::DB::DBMappedType<Events::Log>::list =
    std::list<std::pair<std::string, GetterSetter<Events::Log> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::Program> > >
  Interface::DB::DBMappedType<Events::Program>::list =
    std::list<std::pair<std::string, GetterSetter<Events::Program> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::ProgramStatus> > >
  Interface::DB::DBMappedType<Events::ProgramStatus>::list =
    std::list<std::pair<std::string, GetterSetter<Events::ProgramStatus> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::Service> > >
  Interface::DB::DBMappedType<Events::Service>::list =
    std::list<std::pair<std::string, GetterSetter<Events::Service> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::ServiceCheck> > >
  Interface::DB::DBMappedType<Events::ServiceCheck>::list =
    std::list<std::pair<std::string, GetterSetter<Events::ServiceCheck> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::ServiceDependency> > >
  Interface::DB::DBMappedType<Events::ServiceDependency>::list =
    std::list<std::pair<std::string, GetterSetter<Events::ServiceDependency> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::ServiceGroup> > >
  Interface::DB::DBMappedType<Events::ServiceGroup>::list =
    std::list<std::pair<std::string, GetterSetter<Events::ServiceGroup> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::ServiceGroupMember> > >
  Interface::DB::DBMappedType<Events::ServiceGroupMember>::list =
    std::list<std::pair<std::string, GetterSetter<Events::ServiceGroupMember> > >();
template <> std::list<std::pair<std::string, GetterSetter<Events::ServiceStatus> > >
  Interface::DB::DBMappedType<Events::ServiceStatus>::list =
    std::list<std::pair<std::string, GetterSetter<Events::ServiceStatus> > >();

/**************************************
*                                     *
*          Global Functions           *
*                                     *
**************************************/

/**
 *  \brief Initialization routine.
 *
 *  Initialize DB mappings.
 */
void Interface::DB::Initialize()
{
  static_init<Events::Acknowledgement>();
  static_init<Events::Comment>();
  static_init<Events::Downtime>();
  static_init<Events::Host>();
  static_init<Events::HostCheck>();
  static_init<Events::HostDependency>();
  static_init<Events::HostGroup>();
  static_init<Events::HostGroupMember>();
  static_init<Events::HostParent>();
  static_init<Events::HostStatus>();
  static_init<Events::Issue>();
  static_init<Events::Log>();
  static_init<Events::Program>();
  static_init<Events::ProgramStatus>();
  static_init<Events::Service>();
  static_init<Events::ServiceCheck>();
  static_init<Events::ServiceDependency>();
  static_init<Events::ServiceGroup>();
  static_init<Events::ServiceGroupMember>();
  static_init<Events::ServiceStatus>();
  return ;
}

/**
 *  Extract Acknowledgement data from DB row.
 */
void soci::type_conversion<Events::Acknowledgement>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::Acknowledgement& a)
{
  (void)v;
  (void)ind;
  (void)a;
  return ;
}

/**
 *  Extract Acknowledgement data to DB row.
 */
void soci::type_conversion<Events::Acknowledgement>::to_base(
  const Events::Acknowledgement& a,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(a, v);
  return ;
}

/**
 *  Extract Comment data from DB row.
 */
void soci::type_conversion<Events::Comment>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::Comment& c)
{
  (void)v;
  (void)ind;
  (void)c;
  return ;
}

/**
 *  Extract Comment data to DB row.
 */
void soci::type_conversion<Events::Comment>::to_base(
  const Events::Comment& c,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(c, v);
  return ;
}

/**
 *  Extract Downtime data from DB row.
 */
void soci::type_conversion<Events::Downtime>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::Downtime& d)
{
  (void)v;
  (void)ind;
  (void)d;
  return ;
}

/**
 *  Extract Downtime data to DB row.
 */
void soci::type_conversion<Events::Downtime>::to_base(
  const Events::Downtime& d,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(d, v);
  return ;
}

/**
 *  Extract Host data from DB row.
 */
void soci::type_conversion<Events::Host>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::Host& h)
{
  (void)v;
  (void)ind;
  (void)h;
  return ;
}

/**
 *  Extract Host data to DB row.
 */
void soci::type_conversion<Events::Host>::to_base(
  const Events::Host& h,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(h, v);
  return ;
}

/**
 *  Extract HostCheck data from DB row.
 */
void soci::type_conversion<Events::HostCheck>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::HostCheck& hc)
{
  (void)v;
  (void)ind;
  (void)hc;
  return ;
}

/**
 *  Extract HostCheck data to DB row.
 */
void soci::type_conversion<Events::HostCheck>::to_base(
  const Events::HostCheck& hc,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(hc, v);
  return ;
}

/**
 *  Extract HostDependency data from DB row.
 */
void soci::type_conversion<Events::HostDependency>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::HostDependency& hd)
{
  (void)v;
  (void)ind;
  (void)hd;
  return ;
}

/**
 *  Extract HostDependency data to DB row.
 */
void soci::type_conversion<Events::HostDependency>::to_base(
  const Events::HostDependency& hd,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(hd, v);
  return ;
}

/**
 *  Extract HostGroup data from DB row.
 */
void soci::type_conversion<Events::HostGroup>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::HostGroup& hg)
{
  (void)v;
  (void)ind;
  (void)hg;
  return ;
}

/**
 *  Extract HostGroup data to DB row.
 */
void soci::type_conversion<Events::HostGroup>::to_base(
  const Events::HostGroup& hg,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(hg, v);
  return ;
}

/**
 *  Extract HostGroupMember data from DB row.
 */
void soci::type_conversion<Events::HostGroupMember>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::HostGroupMember& hgm)
{
  (void)v;
  (void)ind;
  (void)hgm;
  return ;
}

/**
 *  Extract HostGroupMember data to DB row.
 */
void soci::type_conversion<Events::HostGroupMember>::to_base(
  const Events::HostGroupMember& hgm,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(hgm, v);
  return ;
}

/**
 *  Extract HostParent data from DB row.
 */
void soci::type_conversion<Events::HostParent>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::HostParent& hp)
{
  (void)v;
  (void)ind;
  (void)hp;
  return ;
}

/**
 *  Extract HostParent data to DB row.
 */
void soci::type_conversion<Events::HostParent>::to_base(
  const Events::HostParent& hp,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(hp, v);
  return ;
}

/**
 *  Extract HostStatus data from DB row.
 */
void soci::type_conversion<Events::HostStatus>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::HostStatus& hs)
{
  (void)v;
  (void)ind;
  (void)hs;
  return ;
}

/**
 *  Extract HostStatus data to DB row.
 */
void soci::type_conversion<Events::HostStatus>::to_base(
  const Events::HostStatus& hs,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(hs, v);
  return ;
}

/**
 *  Extract Issue data from DB row.
 */
void soci::type_conversion<Events::Issue>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::Issue& i)
{
  (void)v;
  (void)ind;
  (void)i;
  return ;
}

/**
 *  Extract Issue data to DB row.
 */
void soci::type_conversion<Events::Issue>::to_base(
  const Events::Issue& i,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(i, v);
  return ;
}

/**
 *  Extract Log data from DB row.
 */
void soci::type_conversion<Events::Log>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::Log& l)
{
  (void)v;
  (void)ind;
  (void)l;
  return ;
}

/**
 *  Extract Log data to DB row.
 */
void soci::type_conversion<Events::Log>::to_base(
  const Events::Log& l,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(l, v);
  return ;
}

/**
 *  Extract Program data from DB row.
 */
void soci::type_conversion<Events::Program>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::Program& program)
{
  (void)v;
  (void)ind;
  (void)program;
  return ;
}

/**
 *  Extract Program data to DB row.
 */
void soci::type_conversion<Events::Program>::to_base(
  const Events::Program& program,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(program, v);
  return ;
}

/**
 *  Extract ProgramStatus data from DB row.
 */
void soci::type_conversion<Events::ProgramStatus>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::ProgramStatus& ps)
{
  (void)v;
  (void)ind;
  (void)ps;
  return ;
}

/**
 *  Extract ProgramStatus data to DB row.
 */
void soci::type_conversion<Events::ProgramStatus>::to_base(
  const Events::ProgramStatus& ps,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(ps, v);
  return ;
}

/**
 *  Extract Service data from DB row.
 */
void soci::type_conversion<Events::Service>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::Service& s)
{
  (void)v;
  (void)ind;
  (void)s;
  return ;
}

/**
 *  Extract Service data to DB row.
 */
void soci::type_conversion<Events::Service>::to_base(
  const Events::Service& s,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(s, v);
  return ;
}

/**
 *  Extract ServiceCheck data from DB row.
 */
void soci::type_conversion<Events::ServiceCheck>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::ServiceCheck& sc)
{
  (void)v;
  (void)ind;
  (void)sc;
  return ;
}

/**
 *  Extract ServiceCheck data to DB row.
 */
void soci::type_conversion<Events::ServiceCheck>::to_base(
  const Events::ServiceCheck& sc,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(sc, v);
  return ;
}

/**
 *  Extract ServiceDependency data from DB row.
 */
void soci::type_conversion<Events::ServiceDependency>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::ServiceDependency& sd)
{
  (void)v;
  (void)ind;
  (void)sd;
  return ;
}

/**
 *  Extract ServiceDependency data to DB row.
 */
void soci::type_conversion<Events::ServiceDependency>::to_base(
  const Events::ServiceDependency& sd,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(sd, v);
  return ;
}

/**
 *  Extract ServiceGroup data from DB row.
 */
void soci::type_conversion<Events::ServiceGroup>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::ServiceGroup& sg)
{
  (void)v;
  (void)ind;
  (void)sg;
  return ;
}

/**
 *  Extract ServiceGroup data to DB row.
 */
void soci::type_conversion<Events::ServiceGroup>::to_base(
  const Events::ServiceGroup& sg,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(sg, v);
  return ;
}

/**
 *  Extract ServiceGroupMember data from DB row.
 */
void soci::type_conversion<Events::ServiceGroupMember>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::ServiceGroupMember& sgm)
{
  (void)v;
  (void)ind;
  (void)sgm;
  return ;
}

/**
 *  Extract ServiceGroupMember data to DB row.
 */
void soci::type_conversion<Events::ServiceGroupMember>::to_base(
  const Events::ServiceGroupMember& sgm,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(sgm, v);
  return ;
}

/**
 *  Extract ServiceStatus data from DB row.
 */
void soci::type_conversion<Events::ServiceStatus>::from_base(
  const soci::values& v,
  soci::indicator ind,
  Events::ServiceStatus& ss)
{
  (void)v;
  (void)ind;
  (void)ss;
  return ;
}

/**
 *  Extract ServiceStatus data to DB row.
 */
void soci::type_conversion<Events::ServiceStatus>::to_base(
  const Events::ServiceStatus& ss,
  soci::values& v,
  soci::indicator& ind)
{
  (void)ind;
  ToBase(ss, v);
  return ;
}
