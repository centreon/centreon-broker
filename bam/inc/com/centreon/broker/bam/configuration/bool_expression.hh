/*
** Copyright 2014 Merethis
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

#ifndef BAM_CCB_CONFIGURATION_BOOL_EXPRESSION_HH
#  define BAM_CCB_CONFIGURATION_BOOL_EXPRESSION_HH

#  include <list>
#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/bam/kpi_event.hh"

CCB_BEGIN()

namespace                   bam {
  namespace                 configuration {
    /**
     *  @class bool_expression bool_expression.hh  "com/centreon/broker/bam/configuration/bool_expression.h"
     *  @brief A pseudo-kpi triggered by a the value of a boolean expression.
     *
     *  Configuration of a boolean expression, later used to impact one
     *  or more BA.
     */
    class                   bool_expression {
    public:
      typedef std::list<unsigned int> ids_of_bas;
      typedef std::list<unsigned int> kpi_ids;

                            bool_expression(
                              unsigned int id = 0,
                              double impact = 0.0,
                              std::string const& expression = "",
                              bool impact_if = false);
                            bool_expression(bool_expression const& other);
                            ~bool_expression();
      bool_expression&      operator=(bool_expression const& other);
      bool                  operator==(bool_expression const& other) const;
      bool                  operator!=(bool_expression const& other) const;

      unsigned int          get_id() const;
      double                get_impact() const;
      ids_of_bas const&     get_impacted_bas() const;
      std::string const&    get_expression() const;
      bool                  get_impact_if() const;
      kpi_ids const&        get_kpi_ids() const;
      bam::kpi_event const& get_opened_event() const;

      void                  add_kpi_id(unsigned int id);
      ids_of_bas&           impacted_bas();
      void                  set_impact(double d);
      void                  set_expression(std::string const& s);
      void                  set_impact_if(bool b);
      void                  set_opened_event(bam::kpi_event const& e);

    private:
      unsigned int          _id;
      double                _impact;
      ids_of_bas            _impacted;
      std::string           _expression;
      bool                  _impact_if;
      kpi_ids               _kpi_ids;
      bam::kpi_event        _event;
    };
  }
}

CCB_END()

#endif // !CCB_BAM_CONFIGURATION_BOOL_EXPRESSION_HH
