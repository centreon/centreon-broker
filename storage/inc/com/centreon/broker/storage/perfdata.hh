/*
** Copyright 2011 Merethis
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_STORAGE_PERFDATA_HH_
# define CCB_STORAGE_PERFDATA_HH_

# include <QString>

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          storage {
        /**
         *  @class perfdata perfdata.hh "com/centreon/broker/storage/perfdata.hh"
         *  @brief Store perfdata values.
         *
         *  Store perfdata values.
         */
        class            perfdata {
         private:
          double         _critical;
          double         _max;
          double         _min;
          QString        _name;
          QString        _unit;
          double         _value;
          double         _warning;

         public:
                         perfdata();
                         perfdata(perfdata const& pd);
                         ~perfdata();
          perfdata&      operator=(perfdata const& pd);
          double         critical() const throw ();
          void           critical(double c);
          double         max() const throw ();
          void           max(double m);
          double         min() const throw ();
          void           min(double m);
          QString const& name() const throw ();
          void           name(QString const& n);
          QString const& unit() const throw ();
          void           unit(QString const& u);
          double         value() const throw ();
          void           value(double v);
          double         warning() const throw ();
          void           warning(double w);
        };
      }
    }
  }
}

#endif /* !CCB_STORAGE_PERFDATA_HH_ */
