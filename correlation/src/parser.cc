/*
** Copyright 2009-2011 Merethis
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

#include <memory>
#include <QtXml>
#include <stdlib.h>
#include <string.h>
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
  for (std::map<std::pair<unsigned int, unsigned int>, node>::iterator
         it = _services->begin(),
         end = _services->end();
       it != end;
       ++it) {
    std::map<unsigned int, node>::iterator it2(_hosts->find(it->first.first));
    if (it2 == _hosts->end())
      throw (exceptions::msg() << "could not find host "
               << it->first.first << " for service "
               << it->first.second);
    it->second.depends_on.push_back(&(it2->second));
    it2->second.depended_by.push_back(&(it->second));
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
  if (!service_id) {
    std::map<unsigned int, node>::iterator it(
      _hosts->find(strtoul(host_id, NULL, 0)));
    if (it != _hosts->end())
      n = &it->second;
  }
  else {
    std::map<std::pair<unsigned int, unsigned int>, node>::iterator it(
      _services->find(std::make_pair(strtoul(host_id, NULL, 0),
        strtoul(service_id, NULL, 0))));
    if (it != _services->end())
      n = &it->second;
  }
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
    char const* value(localname.toStdString().c_str());
    if (!strcmp(value, "dependency")) {
      QString hi1;
      QString hi2;
      char* host_id1 = NULL;
      char* host_id2 = NULL;
      node* n1 = NULL;
      node* n2 = NULL;
      QString si1;
      QString si2;
      char const* service_id1 = NULL;
      char const* service_id2 = NULL;

      // Fetch attributes of the XML node.
      hi1 = attrs.value("dependent_host_id");
      hi2 = attrs.value("host_id");
      si1 = attrs.value("dependent_service_id");
      si2 = attrs.value("service_id");
      if (!hi1.size() || !hi2.size())
        throw (exceptions::msg() << "missing an host id for an " \
                 "element of a dependency definition");
      if (si1.size())
        service_id1 = si1.toStdString().c_str();
      if (si2.size())
        service_id2 = si2.toStdString().c_str();

      // Process these attributes.
      n1 = _find_node(host_id1, service_id1);
      n2 = _find_node(host_id2, service_id2);
      if (n1 && n2) {
        n1->depends_on.push_back(n2);
        n2->depended_by.push_back(n1);
      }
    }
    else if (!strcmp(value, "host")) {
      QString i_attr;
      node n;

      // Get XML node attribute.
      i_attr = attrs.value("id");
      if (!i_attr.size())
        throw (exceptions::msg() << "could not find an 'id' attribute" \
                 " on a 'host' definition");

      // Process attribute.
      n.host_id = strtoul(i_attr.toStdString().c_str(), NULL, 0);
      (*_hosts)[n.host_id] = n;
    }
    else if (!strcmp(value, "parent")) {
      QString host_attr;
      QString parent_attr;
      std::map<unsigned int, node>::iterator it1;
      std::map<unsigned int, node>::iterator it2;

      // Get XML node attributes.
      host_attr = attrs.value("host");
      parent_attr = attrs.value("parent");
      if (!host_attr.size() || !parent_attr.size())
        throw (exceptions::msg() << "could not find 'host' or " \
                 "'parent' attribute of a parenting definition");

      // Process attributes.
      it1 = (*_hosts).find(strtoul(host_attr.toStdString().c_str(),
        NULL,
        0));
      it2 = (*_hosts).find(strtoul(parent_attr.toStdString().c_str(),
        NULL,
        0));
      if ((it1 != (*_hosts).end()) && (it2 != (*_hosts).end())) {
        it1->second.parents.push_back(&it2->second);
        it2->second.children.push_back(&it1->second);
      }
    }
    else if (!strcmp(value, "service")) {
      QString host_attr;
      QString id_attr;
      node n;

      // Get XML node attributes.
      host_attr = attrs.value("host");
      id_attr = attrs.value("id");
      if (!host_attr.size() || !id_attr.size())
        throw (exceptions::msg() << "could not find 'host' or 'id' " \
                 "attribute of a 'service' definition");

      // Process attributes.
      n.host_id = strtoul(host_attr.toStdString().c_str(), NULL, 0);
      n.service_id = strtoul(id_attr.toStdString().c_str(), NULL, 0);
      (*_services)[std::make_pair(n.host_id, n.service_id)] = n;
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
 *  @param[in]  filename Path to the correlation file.
 *  @param[out] hosts    List of hosts.
 *  @param[out] services List of services.
 */
void parser::parse(QString const& filename,
                   std::map<unsigned int, node>& hosts,
                   std::map<std::pair<unsigned int, unsigned int>, node>& services) {
  QXmlSimpleReader reader;
  _hosts = &hosts;
  _services = &services;
  try {
    reader.setContentHandler(this);
    reader.setErrorHandler(this);
    QFile qf(filename);
    if (!qf.open(QIODevice::ReadOnly))
      throw (exceptions::msg() << qf.errorString());
    std::auto_ptr<QXmlInputSource> source(new QXmlInputSource(&qf));
    reader.parse(source.get());
    _auto_services_dependencies();
  }
  catch (QXmlParseException const& e) {
    logging::config << logging::HIGH << "correlation: parsing error " \
         "on \"" << filename << "\" (line "
      << (unsigned int)e.lineNumber() << ", character "
      << (unsigned int)e.columnNumber() << "): " << e.message();
  }
  catch (exceptions::msg const& e) {
    logging::config << logging::HIGH << "correlation: error while " \
      "parsing correlation file \"" << filename << "\": " << e.what();
  }
  return ;
}
