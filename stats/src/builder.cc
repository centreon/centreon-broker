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
 */
void builder::build() {
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
          _generate_stats_for_endpoint(it->second, _data, p);
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
 */
void builder::_generate_stats_for_endpoint(
                processing::thread* fo,
                std::string& buffer,
                io::properties& tree) {
  // Header.
  buffer.append("endpoint ");
  buffer.append(fo->get_name());
  buffer.append("\n");

  // Gather statistic.
  fo->stats(tree);

  // Serialize.
  _serialize(buffer, tree);
  // // Header.
  // buffer.append(is_out ? "output " : "input ");
  // buffer.append(fo->_name.toStdString());
  // buffer.append("\n");

  // // Choose stream we will work on.
  // QReadWriteLock* first_rwl;
  // misc::shared_ptr<io::stream>* first_s;
  // QReadWriteLock* second_rwl;
  // misc::shared_ptr<io::stream>* second_s;
  // if (is_out) {
  //   first_rwl = &fo->_tom;
  //   first_s = &fo->_to;
  //   second_rwl = &fo->_fromm;
  //   second_s = &fo->_from;
  // }
  // else {
  //   first_rwl = &fo->_fromm;
  //   first_s = &fo->_from;
  //   second_rwl = &fo->_tom;
  //   second_s = &fo->_to;
  // }

  // // Should we generate more stats ?
  // bool more_stats(true);
  // {
  //   // Get primary state.
  //   buffer.append("state=");
  //   bool locked(first_rwl->tryLockForRead(10));
  //   try {
  //     // Could lock RWL.
  //     if (locked) {
  //       if (first_s->isNull()) {
  //         if (!fo->_last_error.isEmpty()) {
  //           buffer.append("disconnected");
  //           buffer.append(" (");
  //           buffer.append(fo->_last_error.toStdString());
  //           buffer.append(")\n");
  //         }
  //         else if (!fo->isRunning()) {
  //           buffer.append("unused\n");
  //           more_stats = false;
  //         }
  //         else if (!fo->_endpoint.isNull()
  //                  && !fo->_endpoint->is_acceptor()) {
  //           buffer.append("connecting\n");
  //         }
  //         else {
  //           buffer.append("listening\n");
  //         }
  //       }
  //       else if (!fo->_failover.isNull() && fo->_failover->isRunning()) {
  //         buffer.append("replaying\n");
  //         io::properties p;
  //         (*first_s)->statistics(p);
  //         tree.merge(p);
  //         _serialize(buffer, p);
  //       }
  //       else {
  //         buffer.append("connected\n");
  //         io::properties p;
  //         (*first_s)->statistics(p);
  //         tree.merge(p);
  //         _serialize(buffer, p);
  //       }
  //     }
  //     // Could not lock RWL.
  //     else
  //       buffer.append("blocked\n");
  //   }
  //   catch (...) {
  //     if (locked)
  //       first_rwl->unlock();
  //     throw ;
  //   }
  //   if (locked)
  //     first_rwl->unlock();
  // }

  // // More statistics.
  // if (more_stats) {
  //   {
  //     // Get secondary state.
  //     QReadLocker rl(second_rwl);
  //     if (!second_s->isNull()) {
  //       io::properties p;
  //       (*second_s)->statistics(p);
  //       tree.merge(p);
  //       _serialize(buffer, p);
  //     }
  //   }

  //   {
  //     // Event processing stats.
  //     std::ostringstream oss;
  //     oss << "last event at=" << fo->get_last_event() << "\n"
  //       "event processing speed=" << std::fixed
  //         << std::setprecision(1) << fo->get_event_processing_speed()
  //         << " events/s\n";
  //     buffer.append(oss.str());
  //   }

  //   // Endpoint stats.
  //   if (!fo->_endpoint.isNull()) {
  //     io::properties p;
  //     fo->_endpoint->stats(p);
  //     tree.merge(p);
  //     _serialize(buffer, p);
  //   }

  //   {
  //     // Last connection times.
  //     std::ostringstream oss;
  //     oss << "last connection attempt=" << fo->_last_connect_attempt
  //         << "\n" << "last connection success="
  //         << fo->_last_connect_success << "\n";
  //     buffer.append(oss.str());
  //   }
  // }

  // // Failover.
  // if (!fo->_failover.isNull()) {
  //   buffer.append("failover\n");
  //   std::string subbuffer;
  //   io::properties p;
  //   _generate_stats_for_endpoint(
  //     fo->_failover.data(),
  //     subbuffer,
  //     p,
  //     is_out);
  //   tree.children().push_back(p);
  //   subbuffer.insert(0, "  ");
  //   size_t pos(subbuffer.find('\n'));
  //   while ((pos != subbuffer.size() - 1)
  //          && (pos != std::string::npos)) {
  //     subbuffer.replace(pos, 1, "\n  ");
  //     pos = subbuffer.find('\n', pos + 3);
  //   }
  //   buffer.append(subbuffer);
  // }

  return ;
}

/**
 *  Serialize some properties.
 *
 *  @param[out] buffer  Serialized data.
 *  @param[in]  tree    Properties tree.
 *  @param[in]  indent  The indentation to use.
 */
void builder::_serialize(
                std::string& buffer,
                io::properties const& tree,
                unsigned int indent) {
  std::string indent_string(indent * 2, ' ');
  for (io::properties::const_iterator
         it(tree.begin()),
         end(tree.end());
       it != end;
       ++it) {
    buffer.append(indent_string);
    buffer.append(it->second.get_name());
    buffer.append("=");
    buffer.append(it->second.get_value());
    buffer.append("\n");
  }
  if (tree.children().size() != 0) {
    for (io::properties::children_list::const_iterator
           it = tree.children().begin(),
           end = tree.children().end();
         it != end;
         ++it) {
      if (!it->first.empty())
        buffer.append(it->first).append("=").append("\n");
      _serialize(buffer, it->second, indent + 1);
    }
  }
  return ;
}
