/*
** Copyright 2009-2012 Merethis
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

#include <cstdlib>
#include <cstring>
#include <QtXml>
#include "com/centreon/broker/correlation/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Generate automatic services dependencies.
 */
void parser::_auto_services_dependencies() {
  for (QMap<QPair<unsigned int, unsigned int>, node>::iterator
         it = _nodes->begin(),
         end = _nodes->end();
       it != end;
       ++it)
    if (it.key().second) {
      QMap<QPair<unsigned int, unsigned int>, node>::iterator
        it2(_nodes->find(qMakePair(it.key().first, 0u)));
      if (it2 == _nodes->end())
        throw (exceptions::msg() << "could not find host "
                 << it.key().first << " for service "
                 << it.key().second);
      logging::config(logging::medium) << "correlation: service "
        << it.key().second << " automatically depends on host "
        << it.key().first;
      it->add_dependency(&*it2);
    }
  return ;
}

/**
 *  Find a node with its host_id and optionally its service_id.
 *
 *  @param[in] host_id    String of the host_id.
 *  @param[in] service_id Potentially NULL string of the service_id.
 *
 *  @return Node matching host_id/service_id.
 */
node* parser::_find_node(char const* host_id, char const* service_id) {
  node* n(NULL);
  QMap<QPair<unsigned int, unsigned int>, node>::iterator it(
    _nodes->find(qMakePair(
      static_cast<unsigned int>(strtoul(host_id, NULL, 0)),
      static_cast<unsigned int>(service_id
        ? strtoul(service_id, NULL, 0)
        : 0))));
  if (it != _nodes->end())
    n = &*it;
  return (n);
}

bool parser::startElement(QString const& uri,
                          QString const& localname,
                          QString const& qname,
                          QXmlAttributes const& attrs) {
  (void)uri;
  (void)qname;
  if (!_in_root)
    _in_root = true;
  else {
    std::string localname_str(localname.toStdString());
    char const* value(localname_str.c_str());
    if (!strcmp(value, "dependency")) {
      if (!_is_retention) {
        QString hi1;
        QString hi2;
        node* n1 = NULL;
        node* n2 = NULL;
        QString si1;
        QString si2;

        // Fetch attributes of the XML node.
        hi1 = attrs.value("dependent_host");
        hi2 = attrs.value("host");
        si1 = attrs.value("dependent_service");
        si2 = attrs.value("service");
        if (!hi1.size() || !hi2.size())
          throw (exceptions::msg() << "missing an host id for an " \
                   "element of a dependency definition");

        // Process these attributes.
        n1 = _find_node(
          qPrintable(hi1),
          (si1.size() ? qPrintable(si1) : NULL));
        n2 = _find_node(
          qPrintable(hi2),
          (si2.size() ? qPrintable(si2) : NULL));
        if (n1 && n2) {
          logging::config(logging::medium) << "correlation: node ("
            << n1->host_id << ", " << n1->service_id
            << ") depends on node (" << n2->host_id << ", "
            << n2->service_id << ")";
          n1->add_dependency(n2);
        }
      }
    }
    else if (!strcmp(value, "host")) {
      // Get XML node attribute.
      QString i_attr(attrs.value("id"));
      if (!i_attr.size())
        throw (exceptions::msg() << "could not find an 'id' attribute" \
                 " on a 'host' definition");

      // Node.
      node* n;

      // Create new node.
      if (!_is_retention) {
        // Process attribute.
        node new_node;
        new_node.host_id = i_attr.toUInt();
        n = &(*_nodes)[qMakePair(new_node.host_id, 0u)];
        *n = new_node;
        logging::config(logging::medium)
          << "correlation: new host " << new_node.host_id;
      }
      // Get node.
      else {
        QMap<QPair<unsigned int, unsigned int>, node>::iterator
          it(_nodes->find(qMakePair(i_attr.toUInt(), 0u)));
        if (it == _nodes->end()) {
          logging::config(logging::medium)
            << "correlation: could not find host " << i_attr.toUInt();
          return (true);
        }
        n = &*it;
      }

      // Process optionnal arguments.
      i_attr = attrs.value("since");
      if (!i_attr.isEmpty())
        n->since = i_attr.toULongLong();
      i_attr = attrs.value("state");
      if (!i_attr.isEmpty())
        n->state = i_attr.toUInt();
    }
    else if (!strcmp(value, "issue")) {
      QString ack_attr;
      QString host_attr;
      QString service_attr;
      QString start_attr;
      QMap<QPair<unsigned int, unsigned int>, node>::iterator it;

      // Get XML node attributes.
      ack_attr = attrs.value("ack_time");
      host_attr = attrs.value("host");
      service_attr = attrs.value("service");
      start_attr = attrs.value("start_time");
      if (!host_attr.size() || !start_attr.size())
        throw (exceptions::msg() << "could not find 'host' or " \
                 "'start_time' attribute of an issue definition");

      // Process attributes.
      it = (*_nodes).find(qMakePair(
        host_attr.toUInt(),
        (service_attr.isEmpty() ? 0u : service_attr.toUInt())));
      if (it == _nodes->end())
        logging::config(logging::medium) << "correlation: discarding " \
             "issue of node (" << host_attr.toUInt() << ", "
          << service_attr.toUInt() << ") due to non-existent node";
      else {
        if (!it->my_issue.get()) {
          it->my_issue.reset(new issue);
          it->my_issue->host_id = it->host_id;
          it->my_issue->service_id = it->service_id;
        }
        if (!ack_attr.isEmpty())
          it->my_issue->ack_time = ack_attr.toUInt();
        it->my_issue->start_time = start_attr.toUInt();
      }
    }
    else if (!strcmp(value, "parent")) {
      if (!_is_retention) {
        QString host_attr;
        QString parent_attr;
        QMap<QPair<unsigned int, unsigned int>, node>::iterator it1;
        QMap<QPair<unsigned int, unsigned int>, node>::iterator it2;

        // Get XML node attributes.
        host_attr = attrs.value("host");
        parent_attr = attrs.value("parent");
        if (!host_attr.size() || !parent_attr.size())
          throw (exceptions::msg() << "could not find 'host' or " \
                   "'parent' attribute of a parenting definition");

        // Process attributes.
        it1 = (*_nodes).find(qMakePair(host_attr.toUInt(), 0u));
        it2 = (*_nodes).find(qMakePair(parent_attr.toUInt(), 0u));
        if ((it1 != (*_nodes).end()) && (it2 != (*_nodes).end())) {
          logging::config(logging::medium) << "correlation: host "
            << it2->host_id << " is parent of host "
            << it1->host_id;
          it1->add_parent(&*it2);
        }
      }
    }
    else if (!strcmp(value, "service")) {
      // Get XML node attributes.
      QString host_attr(attrs.value("host"));
      QString id_attr(attrs.value("id"));
      if (!host_attr.size() || !id_attr.size())
        throw (exceptions::msg() << "could not find 'host' or 'id' " \
                 "attribute of a 'service' definition");

      // Node.
      node* n;

      // Create new node.
      if (!_is_retention) {
        // Process attributes.
        node new_node;
        new_node.host_id = host_attr.toUInt();
        new_node.service_id = id_attr.toUInt();
        (*_nodes)[qMakePair(new_node.host_id, new_node.service_id)]
          = new_node;
        n = &(*_nodes)[qMakePair(new_node.host_id, new_node.service_id)];
        *n = new_node;
        logging::config(logging::medium) << "correlation: new service ("
          << new_node.host_id << ", " << new_node.service_id << ")";
      }
      // Get node.
      else {
        QMap<QPair<unsigned int, unsigned int>, node>::iterator
          it(_nodes->find(
               qMakePair(host_attr.toUInt(), id_attr.toUInt())));
        if (it == _nodes->end()) {
          logging::config(logging::medium) << "could not find service ("
            << host_attr.toUInt() << ", " << id_attr.toUInt() << ")";
          return (true);
        }
        n = &*it;
      }

      // Process optionnal arguments.
      id_attr = attrs.value("since");
      if (!id_attr.isEmpty())
        n->since = id_attr.toULongLong();
      id_attr = attrs.value("state");
      if (!id_attr.isEmpty())
        n->state = id_attr.toUInt();
    }
  }
  return (true);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
parser::parser() {}

/**
 *  Copy constructor.
 *
 *  @param[in] p Unused.
 */
parser::parser(parser const& p) : QXmlDefaultHandler() {
  (void)p;
}

/**
 *  Destructor.
 */
parser::~parser() {}

/**
 *  Assignment operator.
 *
 *  @param[in] p Unused.
 *
 *  @return *this
 */
parser& parser::operator=(parser const& p) {
  (void)p;
  return (*this);
}

/**
 *  Parse a configuration file.
 *
 *  @param[in]  filename     Path to the correlation file.
 *  @param[in]  is_retention Set to true if filename is a retention
 *                           file (non-authoritative content).
 *  @param[out] nodes        Node set.
 */
void parser::parse(QString const& filename,
                   bool is_retention,
                   QMap<QPair<unsigned int, unsigned int>, node>& nodes) {
  QXmlSimpleReader reader;
  _is_retention = is_retention;
  _nodes = &nodes;
  try {
    reader.setContentHandler(this);
    reader.setErrorHandler(this);
    QFile qf(filename);
    if (!qf.open(QIODevice::ReadOnly))
      throw (exceptions::msg() << qf.errorString());
    QXmlInputSource source(&qf);
    reader.parse(&source);
    if (!_is_retention)
      _auto_services_dependencies();
  }
  catch (QXmlParseException const& e) {
    throw (exceptions::msg() << "parsing error on '" << filename
             << "' at line "
             << static_cast<unsigned int>(e.lineNumber())
             << ", character "
             << static_cast<unsigned int>(e.columnNumber())
             << ": " << e.message());
  }
  catch (exceptions::msg const& e) {
    throw (exceptions::msg() << "parsing error on '" << filename
           << "': " << e.what());
  }
  return ;
}
