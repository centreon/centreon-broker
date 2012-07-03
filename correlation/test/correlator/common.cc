/*
** Copyright 2011-2012 Merethis
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

#include "com/centreon/broker/correlation/engine_state.hh"
#include "com/centreon/broker/correlation/host_state.hh"
#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "com/centreon/broker/correlation/service_state.hh"
#include "test/correlator/common.hh"

using namespace com::centreon::broker;

/**
 *  Add an engine state to a content.
 *
 *  @param[out] content Content.
 *  @param[in]  started true if correlation engine is started, false
 *                      otherwise.
 */
void add_engine_state(
       QList<misc::shared_ptr<io::data> >& content,
       bool started) {
  misc::shared_ptr<correlation::engine_state>
    es(new correlation::engine_state);
  es->started = started;
  content.push_back(es.staticCast<io::data>());
  return ;
}

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
void add_issue(
       QList<misc::shared_ptr<io::data> >& content,
       time_t ack_time,
       time_t end_time,
       unsigned int host_id,
       unsigned int service_id,
       time_t start_time) {
  misc::shared_ptr<correlation::issue> i(new correlation::issue);
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
void add_issue_parent(
       QList<misc::shared_ptr<io::data> >& content,
       unsigned int child_host_id,
       unsigned int child_service_id,
       time_t child_start_time,
       time_t end_time,
       unsigned int parent_host_id,
       unsigned int parent_service_id,
       time_t parent_start_time,
       time_t start_time) {
  misc::shared_ptr<correlation::issue_parent>
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
 *  Add a host state to a content.
 *
 *  @param[out] content       Content.
 *  @param[in]  ack_time      Acknowledgement time.
 *  @param[in]  current_state Current service state.
 *  @param[in]  end_time      State end time.
 *  @param[in]  host_id       Host ID.
 *  @param[in]  in_downtime   Is in downtime ?
 *  @param[in]  start_time    State start time.
 */
void add_state_host(
       QList<misc::shared_ptr<io::data> >& content,
       time_t ack_time,
       int current_state,
       time_t end_time,
       unsigned int host_id,
       bool in_downtime,
       time_t start_time) {
  misc::shared_ptr<correlation::host_state>
    s(new correlation::host_state);
  s->ack_time = ack_time;
  s->current_state = current_state;
  s->end_time = end_time;
  s->host_id = host_id;
  s->in_downtime = in_downtime;
  s->start_time = start_time;
  content.push_back(s.staticCast<io::data>());
  return ;
}

/**
 *  Add a service state to a content.
 *
 *  @param[out] content       Content.
 *  @param[in]  ack_time      Acknowledgement time.
 *  @param[in]  current_state Current service state.
 *  @param[in]  end_time      State end time.
 *  @param[in]  host_id       Host ID.
 *  @param[in]  in_downtime   Is in downtime ?
 *  @param[in]  service_id    Service ID.
 *  @param[in]  start_time    State start time.
 */
void add_state_service(
       QList<misc::shared_ptr<io::data> >& content,
       time_t ack_time,
       int current_state,
       time_t end_time,
       unsigned int host_id,
       bool in_downtime,
       unsigned int service_id,
       time_t start_time) {
  misc::shared_ptr<correlation::service_state>
    s(new correlation::service_state);
  s->ack_time = ack_time;
  s->current_state = current_state;
  s->end_time = end_time;
  s->host_id = host_id;
  s->in_downtime = in_downtime;
  s->service_id = service_id;
  s->start_time = start_time;
  content.push_back(s.staticCast<io::data>());
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
bool check_content(
       io::stream& s,
       QList<misc::shared_ptr<io::data> > const& content) {
  bool retval(true);
  for (QList<misc::shared_ptr<io::data> >::const_iterator
         it(content.begin()),
         end(content.end());
       retval && (it != end);) {
    misc::shared_ptr<io::data> d;
    s.read(d);
    if (d.isNull())
      retval = false;
    else if (d->type() == (*it)->type()) {
      if (d->type() == "com::centreon::broker::correlation::engine_state") {
        misc::shared_ptr<correlation::engine_state>
          es1(d.staticCast<correlation::engine_state>());
        misc::shared_ptr<correlation::engine_state>
          es2(it->staticCast<correlation::engine_state>());
        retval = (es1->started == es2->started);
      }
      else if (d->type() == "com::centreon::broker::correlation::issue") {
        misc::shared_ptr<correlation::issue>
          i1(d.staticCast<correlation::issue>());
        misc::shared_ptr<correlation::issue>
          i2(it->staticCast<correlation::issue>());
        retval = ((i1->ack_time == i2->ack_time)
                  && ((i1->end_time && i2->end_time)
                      || (!i1->end_time && !i2->end_time))
                  && (i1->host_id == i2->host_id)
                  && (i1->service_id == i2->service_id)
                  && ((i1->start_time && i2->start_time)
                      || (!i1->start_time && !i2->start_time)));
      }
      else if (d->type()
               == "com::centreon::broker::correlation::issue_parent") {
        misc::shared_ptr<correlation::issue_parent>
          ip1(d.staticCast<correlation::issue_parent>());
        misc::shared_ptr<correlation::issue_parent>
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
      else if (d->type()
               == "com::centreon::broker::correlation::host_state") {
        misc::shared_ptr<correlation::host_state>
          s1(d.staticCast<correlation::host_state>());
        misc::shared_ptr<correlation::host_state>
          s2(it->staticCast<correlation::host_state>());
        retval = ((s1->ack_time == s2->ack_time)
                  && (s1->current_state == s2->current_state)
                  && ((s1->end_time && s2->end_time)
                      || (!s2->end_time && !s2->end_time))
                  && (s1->host_id == s2->host_id)
                  && (s1->in_downtime == s2->in_downtime)
                  && ((s1->start_time && s2->start_time)
                      || (!s1->start_time && !s2->start_time)));
      }
      else if (d->type()
               == "com::centreon::broker::correlation::service_state") {
        misc::shared_ptr<correlation::service_state>
          s1(d.staticCast<correlation::service_state>());
        misc::shared_ptr<correlation::service_state>
          s2(it->staticCast<correlation::service_state>());
        retval = ((s1->ack_time == s2->ack_time)
                  && (s1->current_state == s2->current_state)
                  && ((s1->end_time && s2->end_time)
                      || (!s2->end_time && !s2->end_time))
                  && (s1->host_id == s2->host_id)
                  && (s1->in_downtime == s2->in_downtime)
                  && (s1->service_id == s2->service_id)
                  && ((s1->start_time && s2->start_time)
                      || (!s1->start_time && !s2->start_time)));
      }
      ++it;
    }
  }
  return (retval);
}
