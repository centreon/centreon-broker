/*
** Copyright 2014 Centreon
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

#ifndef CCB_BAM_COMPUTABLE_HH
#define CCB_BAM_COMPUTABLE_HH

#include <list>
#include <memory>
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/persistent_cache.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class computable computable.hh "com/centreon/broker/bam/computable.hh"
 *  @brief Object that get computed by the BAM engine.
 *
 *  The computation of such objects is triggered by the BAM engine. It
 *  provides an effective way to compute whole part of the BA/KPI tree.
 */
class computable {
 public:
  computable();
  computable(computable const& right);
  virtual ~computable();
  computable& operator=(computable const& right);
  void add_parent(std::shared_ptr<computable> const& parent);
  void propagate_update(io::stream* visitor = NULL);
  void remove_parent(std::shared_ptr<computable> const& parent);

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
  virtual bool child_has_update(computable* child,
                                io::stream* visitor = NULL) = 0;

 private:
  void _internal_copy(computable const& right);

  std::list<std::weak_ptr<computable>> _parents;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_COMPUTABLE_HH
