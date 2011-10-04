/*
** Copyright 2011 Merethis
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

#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "test/correlator/common.hh"

using namespace com::centreon::broker;

/**
 *  Add an issue to a content.
 *
 *  @param[out] content    Content.
 *  @param[in]  ack_time   Issue acknowledgement time.
 *  @param[in]  end_time   Issue end time.
 *  @param[in]  host_id    Issue host ID.
 *  @param[in]  service_id Issue service ID.
 *  @param[in]  start_time Issue start time.
 */
void add_issue(QList<QSharedPointer<io::data> >& content,
               time_t ack_time,
               time_t end_time,
               unsigned int host_id,
               unsigned int service_id,
               time_t start_time) {
  QSharedPointer<correlation::issue> i(new correlation::issue);
  i->ack_time = ack_time;
  i->end_time = end_time;
  i->host_id = host_id;
  i->service_id = service_id;
  i->start_time = start_time;
  content.push_back(i.staticCast<io::data>());
  return ;
}

/**
 *  Add an issue parenting to a content.
 *
 *  @param[out] content           Content.
 *  @param[in]  child_host_id     Child host ID.
 *  @param[in]  child_service_id  Child service ID.
 *  @param[in]  child_start_time  Child start time.
 *  @param[in]  end_time          Parenting end time.
 *  @param[in]  parent_host_id    Parent host ID.
 *  @param[in]  parent_service_id Parent service ID.
 *  @param[in]  parent_start_time Parent start time.
 *  @param[in]  start_time        Parenting start time.
 */
void add_issue_parent(QList<QSharedPointer<io::data> >& content,
                      unsigned int child_host_id,
                      unsigned int child_service_id,
                      time_t child_start_time,
                      time_t end_time,
                      unsigned int parent_host_id,
                      unsigned int parent_service_id,
                      time_t parent_start_time,
                      time_t start_time) {
  QSharedPointer<correlation::issue_parent>
    ip(new correlation::issue_parent);
  ip->child_host_id = child_host_id;
  ip->child_service_id = child_service_id;
  ip->child_start_time = child_start_time;
  ip->end_time = end_time;
  ip->parent_host_id = parent_host_id;
  ip->parent_service_id = parent_service_id;
  ip->parent_start_time = parent_start_time;
  ip->start_time = start_time;
  content.push_back(ip.staticCast<io::data>());
  return ;
}

/**
 *  Check the content read from a stream.
 *
 *  @param[in] s       Stream.
 *  @param[in] content Content to match against stream.
 *
 *  @return true if all content was found.
 */
bool check_content(io::stream& s,
                   QList<QSharedPointer<io::data> > const& content) {
  bool retval(true);
  for (QList<QSharedPointer<io::data> >::const_iterator
         it = content.begin(),
         end = content.end();
       retval && (it != end);) {
    QSharedPointer<io::data> d(s.read());
    if (d.isNull())
      retval = false;
    else if (d->type() == (*it)->type()) {
      if (d->type() == "com::centreon::broker::correlation::issue") {
        QSharedPointer<correlation::issue>
          i1(d.staticCast<correlation::issue>());
        QSharedPointer<correlation::issue>
          i2(it->staticCast<correlation::issue>());
        retval = (((i1->ack_time && i2->ack_time)
                   || (!i1->ack_time && !i2->ack_time))
                  && ((i1->end_time && i2->end_time)
                      || (!i1->end_time && !i2->end_time))
                  && (i1->host_id == i2->host_id)
                  && (i1->service_id == i2->service_id)
                  && ((i1->start_time && i2->start_time)
                      || (!i1->start_time && !i2->start_time)));
      }
      else if (d->type()
               == "com::centreon::broker::correlation::issue_parent") {
        QSharedPointer<correlation::issue_parent>
          ip1(d.staticCast<correlation::issue_parent>());
        QSharedPointer<correlation::issue_parent>
          ip2(it->staticCast<correlation::issue_parent>());
        retval = ((ip1->child_host_id == ip2->child_host_id)
                  && (ip1->child_service_id == ip2->child_service_id)
                  && ((ip1->child_start_time && ip2->child_start_time)
                      || (!ip1->child_start_time
                          && !ip2->child_start_time))
                  && ((ip1->end_time && ip2->end_time)
                      || (!ip1->end_time && !ip2->end_time))
                  && (ip1->parent_host_id == ip2->parent_host_id)
                  && (ip1->parent_service_id == ip2->parent_service_id)
                  && ((ip1->parent_start_time && ip2->parent_start_time)
                      || (!ip1->parent_start_time
                          && !ip2->parent_start_time))
                  && ((ip1->start_time && ip2->start_time)
                      || (!ip1->start_time && !ip2->start_time)));
      }
      ++it;
    }
  }
  return (retval);
}
