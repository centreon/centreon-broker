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

#ifndef CCB_BAM_COMPUTABLE_HH
#  define CCB_BAM_COMPUTABLE_HH

#  include <list>
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        bam {
  // Forward declaration.
  class          monitoring_stream;

  /**
   *  @class computable computable.hh "com/centreon/broker/bam/computable.hh"
   *  @brief Object that get computed by the BAM engine.
   *
   *  The computation of such objects is triggered by the BAM engine. It
   *  provides an effective way to compute whole part of the BA/KPI tree.
   */
  class          computable {
  public:
                 computable();
                 computable(computable const& right);
    virtual      ~computable();
    computable&  operator=(computable const& right);
    void         add_parent(misc::shared_ptr<computable> const& parent);
    void         propagate_update(monitoring_stream* visitor = NULL);
    void         remove_parent(misc::shared_ptr<computable> const& parent);

    /**
     *  @brief Notify node of the change of a child node.
     *
     *  This is the method used to compute the value of a node (either a
     *  BA or a KPI). This method does not recursively recompute all
     *  dependencies (this is left to the computable class) but rather
     *  update the node value.
     *
     *  @param[in] child Recently updated child node.
     *
     *  @return True if the parent was modified.
     */
    virtual bool child_has_update(
                   computable* child,
                   monitoring_stream* visitor = NULL) = 0;

  private:
    void         _internal_copy(computable const& right);

    std::list<misc::shared_ptr<computable> >
                 _parents;
  };
}

CCB_END()

#endif // !CCB_BAM_COMPUTABLE_HH
