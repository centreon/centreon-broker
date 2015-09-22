/*
** Copyright 2013-2015 Centreon
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

#include <QFileInfo>
#include <QMutexLocker>
#include <iomanip>
#include <sstream>
#include <time.h>
#include <unistd.h>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/config/endpoint.hh"
#include "com/centreon/broker/io/properties.hh"
#include "com/centreon/broker/processing/thread.hh"
#include "com/centreon/broker/stats/builder.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::stats;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
builder::builder() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right The object to copy.
 */
builder::builder(builder const& right) {
  operator=(right);
}

/**
 *  Destructor.
 */
builder::~builder() throw () {}

/**
 *  Copy operator.
 *
 *  @param[in] right The object to copy.
 *
 *  @return This object.
 */
builder& builder::operator=(builder const& right) {
  if (this != &right) {
    _data = right._data;
    _root = right._root;
  }
  return (*this);
}

/**
 *  Get and build statistics.
 *
 *  @param[in,out] srz  The serializer to use to serialize data.
 */
void builder::build(serializer const& srz) {
  // Cleanup.
  _data.clear();
  _root = io::properties();

  // General.
  {
    std::ostringstream oss;
    oss << "broker\n"
      "version=" CENTREON_BROKER_VERSION "\n"
      "pid=" << getpid() << "\n"
      "now=" << time(NULL) << "\n"
      "compiled with qt=" << QT_VERSION_STR << "\n"
      "running with qt=" << qVersion() << "\n"
      "\n";
    _data.append(oss.str());
  }

  // Modules.
  config::applier::modules&
    mod_applier(config::applier::modules::instance());
  for (config::applier::modules::iterator
         it(mod_applier.begin()),
         end(mod_applier.end());
       it != end;
       ++it) {
    std::ostringstream oss;
    QFileInfo fi(it->first.c_str());
    oss << "module " << it->first << "\n"
      "state=loaded\n"
      "size=" << fi.size() << "B\n"
      "\n";
    _data.append(oss.str());
  }

  // Endpoint applier.
  config::applier::endpoint&
    endp_applier(config::applier::endpoint::instance());

  // Print endpoints.
  {
    bool locked(endp_applier.endpoints_mutex().tryLock(100));
    try {
      if (locked)
        for (config::applier::endpoint::iterator
               it(endp_applier.endpoints_begin()),
               end(endp_applier.endpoints_end());
             it != end;
             ++it) {
          io::properties p;
          _generate_stats_for_endpoint(it->second, _data, p, srz);
          _root.add_child(p);
          _data.append("\n");
        }
      else
        _data.append(
          "inputs=could not fetch list, configuration update in progress ?\n");
    }
    catch (...) {
      if (locked)
        endp_applier.endpoints_mutex().unlock();
      throw ;
    }
    if (locked)
      endp_applier.endpoints_mutex().unlock();
  }

  return ;
}

/**
 *  Get data buffer.
 *
 *  @return The statistics buffer.
 */
std::string const& builder::data() const throw () {
  return (_data);
}

/**
 *  Get the properties tree.
 *
 *  @return The statistics tree.
 */
io::properties const& builder::root() const throw () {
  return (_root);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Generate statistics for an endpoint.
 *
 *  @param[in]  fo     Failover thread of the endpoint.
 *  @param[out] buffer Buffer in which data will be printed.
 *  @param[out] tree   Properties for this tree.
 *  @param[in,out] srz The serializer to use to serialize data.
 */
void builder::_generate_stats_for_endpoint(
                processing::thread* fo,
                std::string& buffer,
                io::properties& tree,
                serializer const& srz) {
  // Header.
  buffer.append("endpoint ");
  buffer.append(fo->get_name());
  buffer.append("\n");

  // Gather statistic.
  fo->stats(tree);

  // Serialize.
  srz.serialize(buffer, tree);

  return ;
}

