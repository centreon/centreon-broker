/*
** mapping.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/01/09 Matthieu Kermagoret
** Last update 06/03/09 Matthieu Kermagoret
*/

#ifndef MAPPING_H_
# define MAPPING_H_

# include "host.h"
# include "host_status.h"

namespace                               CentreonBroker
{
  template                              <typename ObjectType>
  struct                                FieldGetter
  {
    union
    {
      double (ObjectType::*             get_double)() const throw ();
      int (ObjectType::*                get_int)() const throw ();
      short (ObjectType::*              get_short)() const throw ();
      const std::string& (ObjectType::* get_string)() const throw ();
      time_t (ObjectType::*             get_timet)() const throw ();

    }                                   getter_;
    char                                type_;

    FieldGetter(int zero) : type_(0)
    {
      (void)zero;
    }
    FieldGetter(double (ObjectType::* gd)() const throw ())
      : type_('d')
    {
      this->getter_.get_double = gd;
    }
    FieldGetter(int (ObjectType::* gi)() const throw ())
      : type_('i')
    {
      this->getter_.get_int = gi;
    }
    FieldGetter(short (ObjectType::* gs)() const throw ())
      : type_('s')
    {
      this->getter_.get_short = gs;
    }
    FieldGetter(const std::string& (ObjectType::* gs)() const throw ())
      : type_('S')
    {
      this->getter_.get_string = gs;
    }
    FieldGetter(time_t (ObjectType::* gt)() const throw ())
      : type_('t')
    {
      this->getter_.get_timet = gt;
    }
  };

  // Host
  extern const char*                    host_fields[];
  extern FieldGetter<Host>              host_getters[];
  // HostStatus
  extern const char*                    host_status_fields[];
  extern FieldGetter<HostStatus>        host_status_getters[];
  extern const char*                    host_status_uniques[];
}

#endif /* !MAPPING_H_ */
