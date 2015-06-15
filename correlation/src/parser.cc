/*
** Copyright 2009-2014 Merethis
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
#include <QSet>
#include "com/centreon/broker/correlation/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

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
 *  @param[out] nodes        Node set.
 *  @param[in]  recursive    Recursion flag.
 */
void parser::parse(
               QString const& filename,
               QMap<QPair<unsigned int, unsigned int>, node>& nodes,
               bool recursive) {
  _in_include = false;
  _in_root = false;
  _include_file.clear();
  QXmlSimpleReader reader;
  _nodes = &nodes;
  try {
    reader.setContentHandler(this);
    reader.setErrorHandler(this);
    QFile qf(filename);
    if (!qf.open(QIODevice::ReadOnly))
      throw (exceptions::msg() << qf.errorString());
    QXmlInputSource source(&qf);
    reader.parse(&source);
    if (!recursive)
      _auto_services_dependencies();
    _sanity_circular_check(nodes);
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

/**
 *  Callback called when characters are available.
 *
 *  @param[in] ch Characters.
 *
 *  @return true.
 */
bool parser::characters(QString const& ch) {
  if (_in_include)
    _include_file.append(ch);
  return (true);
}

/**
 *  Callback called when an element terminates.
 *
 *  @param[in] uri       Unused.
 *  @param[in] localname Unused.
 *  @param[in] qname     Unused.
 *
 *  @return true.
 */
bool parser::endElement(
               QString const& uri,
               QString const& localname,
               QString const& qname) {
  (void)uri;
  (void)localname;
  (void)qname;
  if (_in_include) {
    parser p;
    p.parse(_include_file, *_nodes, true);
    _in_include = false;
    _include_file.clear();
  }
  return (true);
}

/**
 *  Callback called when a new XML element starts.
 *
 *  @param[in] uri       URI.
 *  @param[in] localname Local name.
 *  @param[in] qname     QName.
 *  @param[in] attrs     Element attributes.
 *
 *  @return true to continue parsing, false otherwise.
 */
bool parser::startElement(
               QString const& uri,
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
        throw (exceptions::msg() << "missing an host ID for an "
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
    else if (!strcmp(value, "host")) {
      // Get XML node attribute.
      QString i_attr(attrs.value("id"));
      if (!i_attr.size())
        throw (exceptions::msg() << "could not find an 'id' attribute" \
                 " on a 'host' definition");

      // Node.
      node* n;

      // Create new node.
      // Process attribute.
      node new_node;
      new_node.host_id = i_attr.toUInt();
      n = &(*_nodes)[qMakePair(new_node.host_id, 0u)];
      *n = new_node;
      logging::config(logging::medium)
        << "correlation: new host " << new_node.host_id;

      // Process optionnal arguments.
      /*i_attr = attrs.value("since");
      if (!i_attr.isEmpty())
        n->since = i_attr.toULongLong();
      else
        n->since = time(NULL);*/
      i_attr = attrs.value("state");
      if (!i_attr.isEmpty())
        n->state = i_attr.toUInt();
    }
    else if (!strcmp(value, "include"))
      _in_include = true;
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
      QString hi1;
      QString hi2;
      node* n1(NULL);
      node* n2(NULL);
      QString si1;
      QString si2;

      // Get XML node attributes.
      hi1 = attrs.value("parent_host");
      hi2 = attrs.value("host");
      si1 = attrs.value("parent_service");
      si2 = attrs.value("service");
      if (!hi1.size() || !hi2.size())
        throw (exceptions::msg() << "missing an host ID for an element "
                  "of a parent definition");

      // Process attributes.
      n1 = _find_node(
             qPrintable(hi1),
             (si1.size() ? qPrintable(si1) : NULL));
      n2 = _find_node(
             qPrintable(hi2),
             (si2.size() ? qPrintable(si2) : NULL));
      if (n1 && n2) {
        logging::config(logging::medium) << "correlation: host ("
          << n1->host_id << ", " << n1->service_id
          << ") is parent of host (" << n2->host_id << ", "
          << n2->service_id << ")";
        n2->add_parent(n1);
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

      // Process optionnal arguments.
      id_attr = attrs.value("since");
      /*if (!id_attr.isEmpty())
        n->since = id_attr.toULongLong();
      else
        n->since = time(NULL);*/
      id_attr = attrs.value("state");
      if (!id_attr.isEmpty())
        n->state = id_attr.toUInt();
    }
  }
  return (true);
}

/**
 *  Implementation for the circular check.
 *
 *  @param[in] n                          The node.
 *  @param[in] get_method                 Method used to get the list of children.
 *  @param[in] visited_node               Working set of visited nodes.
 *  @param[in,out] already_visited_nodes  Set of already visited nodes.
 */
static void circular_check_impl(
              node& n,
              node::node_map const& (node::*get_method)() const,
              QSet<node*>& visited_nodes,
              QSet<node*>& already_visited_nodes) {
  if (already_visited_nodes.contains(&n))
    return ;
  else if (visited_nodes.contains(&n))
    throw (exceptions::msg()
           << "correlation: circular check failed for node ("
           << n.host_id << ", " << n.service_id << ")");

  // Push the node to the set of nodes being visited.
  visited_nodes.insert(&n);

  // Get the children, do a DFS
  node::node_map const& list = (n.*get_method)();
  for (node::node_map::const_iterator it = list.begin(), end = list.end();
       it != end;
       ++it) {
    circular_check_impl(
      const_cast<node&>(**it),
      get_method,
      visited_nodes,
      already_visited_nodes);
  }

  // Remove the node to the set of nodes being visited, add it to the set
  // of nodes already visited.
  visited_nodes.remove(&n);
  already_visited_nodes.insert(&n);
}

/**
 *  Check for circular connections between nodes.
 *
 *  @param[in] nodes  The nodes.
 */
void parser::_sanity_circular_check(
       QMap<QPair<unsigned int, unsigned int>, node> const& nodes) {
  QSet<node*> visited_parent_nodes;
  QSet<node*> visited_child_nodes;
  QSet<node*> visited_depended_nodes;
  QSet<node*> visited_depend_nodes;

  QSet<node*> working_set;

  for (QMap<QPair<unsigned int, unsigned int>, node>::const_iterator
         it = nodes.begin(),
         end = nodes.end();
       it != end;
       ++it) {
    // Check parents.
    if (!visited_parent_nodes.contains(const_cast<node*>(&*it)))
      circular_check_impl(
        const_cast<node&>(*it),
        &node::get_parents,
        working_set,
        visited_parent_nodes);
    // Check children.
    if (!visited_child_nodes.contains(const_cast<node*>(&*it)))
      circular_check_impl(
        const_cast<node&>(*it),
        &node::get_children,
        working_set,
        visited_child_nodes);
    // Check dependeds.
    if (!visited_depended_nodes.contains(const_cast<node*>(&*it)))
      circular_check_impl(
        const_cast<node&>(*it),
        &node::get_dependeds,
        working_set,
        visited_depended_nodes);
    // Check dependencies
    if (!visited_depend_nodes.contains(const_cast<node*>(&*it)))
      circular_check_impl(
        const_cast<node&>(*it),
        &node::get_dependencies,
        working_set,
        visited_depend_nodes);
  }
}
