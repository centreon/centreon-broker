/*
** Copyright 2011-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_STORAGE_PERFDATA_HH
#  define CCB_STORAGE_PERFDATA_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          storage {
  /**
   *  @class perfdata perfdata.hh "com/centreon/broker/storage/perfdata.hh"
   *  @brief Store perfdata values.
   *
   *  Store perfdata values.
   */
  class            perfdata {
  public:
    enum           data_type {
      gauge = 0,
      counter,
      derive,
      absolute,
      automatic
    };

                   perfdata();
                   perfdata(perfdata const& pd);
                   ~perfdata() throw ();
    perfdata&      operator=(perfdata const& pd);
    double         critical() const throw ();
    void           critical(double c) throw ();
    double         critical_low() const throw ();
    void           critical_low(double c) throw ();
    bool           critical_mode() const throw ();
    void           critical_mode(bool m) throw ();
    double         max() const throw ();
    void           max(double m) throw ();
    double         min() const throw ();
    void           min(double m) throw ();
    std::string const& name() const throw ();
    void           name(std::string const& n);
    std::string const& unit() const throw ();
    void           unit(std::string const& u);
    double         value() const throw ();
    void           value(double v) throw ();
    data_type      value_type() const throw ();
    void           value_type(data_type t) throw ();
    double         warning() const throw ();
    void           warning(double w) throw ();
    double         warning_low() const throw ();
    void           warning_low(double w) throw ();
    bool           warning_mode() const throw ();
    void           warning_mode(bool m) throw ();

  private:
    double         _critical;
    double         _critical_low;
    bool           _critical_mode;
    double         _max;
    double         _min;
    std::string        _name;
    std::string        _unit;
    double         _value;
    data_type      _value_type;
    double         _warning;
    double         _warning_low;
    bool           _warning_mode;
  };

  /**
   *  Get the value.
   *
   *  @return Metric value.
   */
  // Inlined after profiling for performance.
  inline double perfdata::value() const throw () {
    return (_value);
  }
}

CCB_END()

bool operator==(
       com::centreon::broker::storage::perfdata const& left,
       com::centreon::broker::storage::perfdata const& right);
bool operator!=(
       com::centreon::broker::storage::perfdata const& left,
       com::centreon::broker::storage::perfdata const& right);

#endif // !CCB_STORAGE_PERFDATA_HH
