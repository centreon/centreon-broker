/*
** Copyright 2009-2013 Merethis
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
** <http:/www.gnu.org/licenses/>.
*/
#ifndef CCB_CONFIGURATION_STATE_HH
#  define CCB_CONFIGURATION_STATE_HH

#  include <vector>
#  include "com/centreon/broker/bam/configuration/kpi.hh"
#  include "com/centreon/broker/bam/configuration/bool_expression.hh"
#  include "com/centreon/broker/bam/configuration/ba.hh"

#  include "com/centreon/broker/namespace.hh"
CCB_BEGIN()

/**
 *
 *@class state
 *
 *
 */
namespace configuration{


/**
 * typedefs
 *
 */
typedef std::vector<ba> bas;
typedef std::vector<kpi> kpis;
typedef std::vector<bool_expression> bool_exps;


  
  class state{
  public:
    bas const& get_bas();
    kpis const& get_kpis();
    bool_exps const& get_boolexps();
    
  private:
    bas _bas;
    kpis _kpis;
    bool_exps _bool_expressions;
  };

}

CCB_END()

#endif
