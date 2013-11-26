/*
** Copyright 2013 Merethis
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

#include <QDomDocument>
#include <QDomElement>
#include <QString>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/stats/config.hh"
#include "com/centreon/broker/stats/metric.hh"
#include "com/centreon/broker/stats/parser.hh"

using namespace com::centreon::broker::stats;

/**
 *  Default constructor.
 */
parser::parser() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
parser::parser(parser const& right) {
  (void)right;
}

/**
 *  Destructor.
 */
parser::~parser() throw () {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
parser& parser::operator=(parser const& right) {
  (void)right;
  return (*this);
}

/**
 *  Parse a XML buffer.
 *
 *  @param[out] entries Parsed entries.
 *  @param[in]  content XML content.
 */
void parser::parse(
               config& cfg,
               std::string const& content) {
  // Parse XML.
  QDomDocument d;
  if (!d.setContent(QString(content.c_str())))
    return ;

  QDomElement root(d.documentElement());
  QDomElement fifo(root.lastChildElement("fifo"));
  if (!fifo.isNull())
    cfg.set_fifo(fifo.text().toStdString());

  QDomElement remote(root.lastChildElement("remote"));
  if (!remote.isNull()) {
    QDomElement tag(remote.lastChildElement("dumper_tag"));
    if (tag.isNull())
      throw (exceptions::msg()
             << "stats: invalid remote dumper tag");
    cfg.set_dumper_tag(tag.text().toStdString());

    QDomElement interval(remote.lastChildElement("interval"));
    if (!interval.isNull())
      cfg.set_interval(interval.text().toUInt());

    QDomElement metrics(remote.lastChildElement("metrics"));
    if (!metrics.isNull()) {
      QDomElement host(metrics.lastChildElement("host"));
      if (host.isNull())
        throw (exceptions::msg() << "stats: invalid remote host");
      unsigned int host_id(host.text().toUInt());

      QDomElement service(metrics.firstChildElement("service"));
      while (!service.isNull()) {
        QDomElement id(service.firstChildElement("id"));
        if (id.isNull())
          throw (exceptions::msg()
                 << "stats: invalid remote service id");
        QDomElement name(service.firstChildElement("name"));
        if (name.isNull())
          throw (exceptions::msg()
                 << "stats: invalid remote service name");

        metric m;
        m.set_host_id(host_id);
        m.set_service_id(id.text().toUInt());
        m.set_name(name.text().toStdString());
        cfg.metrics().push_back(m);

        service = metrics.nextSiblingElement("service");
      }
    }
  }

  return ;
}
