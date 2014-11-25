/*
** Copyright 2011-2014 Merethis
**
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

#ifndef CCB_NOTIFICATION_MACRO_GETTERS_HH
#  define CCB_NOTIFICATION_MACRO_GETTERS_HH

#  include <QString>
#  include <QHash>
#  include <string>
#  include <sstream>
#  include <iomanip>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"
#  include "com/centreon/broker/notification/state.hh"
#  include "com/centreon/broker/notification/objects/contact.hh"
#  include "com/centreon/broker/notification/node_cache.hh"
#  include "com/centreon/broker/notification/utilities/get_datetime_string.hh"

CCB_BEGIN()

namespace        notification {

  /**
   *  Set the precision of a stringstream.
   *
   *  @tparam precision  The precision.
   *
   *  @param[out] ost    The stringstream to modify.
   *
   *  @return  The value of the macro.
   */
  template <int precision>
  void set_precision(std::ostringstream& ost) {
     ost << std::fixed << std::setprecision(precision);
  }

  /**
   *  @brief Does not set the precision of a stringstream.
   *
   *  Specialization of set_precision for precision = 0;
   *
   *  @param[out] ost    The stringstream to modify.
   *
   *  @return  The value of the macro.
   */
  template <> void inline set_precision<0>(std::ostringstream& ost) {}

  /**
   *  Write a type into string.
   *
   *  @tparam T           The type.
   *  @tparam precision   The precision one wants, 0 for none.
   *
   *  @param[in] value    The value.
   *
   *  @return  The value of the macro.
   */
  template <typename T, int precision>
  std::string to_string(T const& value) {
    std::ostringstream ost;
    set_precision<precision>(ost);
    ost << value;
    return (ost.str());
  }

  /**
   *  @brief Write a type into a string.
   *
   *  Specialization for std::string and 0 precision.
   *
   *  @param[in] value  The value of the string.
   *
   *  @return  The value of the macro.
   */
  template <> std::string inline to_string <std::string, 0>(
                                            std::string const& value) {
    return (value);
  }

  /**
   *  @brief Write a type into a string.
   *
   *  Specialization for QString and 0 precision.
   *
   *  @param[in] value  The value of the string.
   *
   *  @return  The value of the macro.
   */
  template <> std::string inline to_string <QString, 0>(
                                            QString const& value) {
    return (value.toStdString());
  }

  /**
   *  Get the member of a host as a string.
   *
   *  @tparam T         The type of the host object.
   *  @tparam U         The type of the member.
   *  @tparam Member    The member.
   *  @tparam precision The precision of the string, 0 for none.
   *
   *  @param[in] id     The id of the host.
   *  @param[in] st     The state of the conf.
   *  @param[in] cache  A node cache.
   *
   *  @return  The value of the macro.
   */
  template <typename T, typename U, U (T::* member), int precision>
    std::string get_host_member_as_string(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache) {
    return (to_string<U, precision>(
              cache.get_host(id.get_host_id()).get_node().*member));
  }

  /**
   *  Get the member of a host status as a string.
   *
   *  @tparam T         The type of the host status object.
   *  @tparam U         The type of the member.
   *  @tparam Member    The member.
   *  @tparam precision The precision of the string, 0 for none.
   *
   *  @param[in] id     The id of the host.
   *  @param[in] st     The state of the conf.
   *  @param[in] cache  A node cache.
   *
   *  @return  The value of the macro.
   */
  template <typename T, typename U, U (T::* member), int precision>
    std::string get_host_status_member_as_string(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache) {
      return (to_string<U, precision>(
                cache.get_host(id.get_host_id()).get_status().*member));
  }

  /**
   *  Get the member of a previous host status as a string.
   *
   *  @tparam T         The type of the previous host status object.
   *  @tparam U         The type of the member.
   *  @tparam Member    The member.
   *  @tparam precision The precision of the string, 0 for none.
   *
   *  @param[in] id     The id of the host.
   *  @param[in] st     The state of the conf.
   *  @param[in] cache  A node cache.
   *
   *  @return  The value of the macro.
   */
  template <typename T, typename U, U (T::* member), int precision>
    std::string get_host_prevstatus_member_as_string(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache) {
      return (to_string<U, precision>(
                cache.get_host(id.get_host_id()).get_prev_status().*member));
  }

  /**
   *  Get the groups of a host.
   *
   *  @tparam get_all  True if we want all the groups, false if only one.
   *
   *  @param[in] id     The id of the host.
   *  @param[in] st     The state of the conf.
   *  @param[in] cache  A node cache.
   *
   *  @return  The value of the macro.
   */
  template <bool get_all>
  std::string get_host_groups(
                 objects::node_id id,
                 state const& st,
                 node_cache const& cache) {}

  template <> std::string get_host_groups<true>(
                            objects::node_id id,
                            state const& st,
                            node_cache const& cache);

  template <> std::string get_host_groups<false>(
                            objects::node_id id,
                            state const& st,
                            node_cache const& cache);

  /**
   *  Get the output of a host.
   *
   *  @tparam get_long_output  True if we want the long output, false if we want the short one.
   *
   *  @param[in] id            The id of the host.
   *  @param[in] st            The state of the conf.
   *  @param[in] cache         A node cache.
   *
   *  @return  The value of the macro.
   */
  template <bool get_long_output>
  std::string get_host_output(
                objects::node_id id,
                state const& st,
                node_cache const& cache) {}

  template <> std::string get_host_output<false>(
                            objects::node_id id,
                            state const& st,
                            node_cache const& cache);

  template <> std::string get_host_output<true>(
                            objects::node_id id,
                            state const& st,
                            node_cache const& cache);

  template <int date_type>
  std::string get_datetime_string(
                objects::node_id id,
                state const& st,
                node_cache const& cache) {
    (void)id; (void)cache;
    utilities::get_datetime_string(
                 ::time(NULL),
                 48,
                 date_type,
                 st.get_date_format());
  }

  // Static, non template getters.
  std::string   get_host_state(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache);

  std::string   get_last_host_state(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache);

  std::string   get_host_state_type(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache);

  std::string   null_getter(objects::node_id id,
                            state const& st,
                            node_cache const& cache);

  std::string   get_host_duration(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache);

  std::string   get_host_duration_sec(
                  objects::node_id id,
                  state const& st,
                  node_cache const& cache);

  std::string  get_timet_string(
                 objects::node_id id,
                 state const& st,
                 node_cache const& cache);
}

CCB_END()

#endif // !CCB_NOTIFICATION_MACRO_GETTERS_HH
