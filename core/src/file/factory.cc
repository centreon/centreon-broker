/*
** Copyright 2011-2013,2016-2017 Centreon
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

#include "com/centreon/broker/file/factory.hh"

#include <memory>

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/file/opener.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Check if a configuration match the file layer.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return True if configuration matches the file layer.
 */
bool factory::has_endpoint(config::endpoint& cfg, flag* flag) {
  bool retval;
  if (flag)
    *flag = no;
  if (cfg.type == "file") {
    cfg.params["coarse"] = "yes";  // File won't respond to any salutation.
    retval = true;
  } else
    retval = false;
  return (retval);
}

/**
 *  Generate an endpoint matching a configuration.
 *
 *  @param[in]  cfg          Endpoint configuration.
 *  @param[out] is_acceptor  Will be set to false.
 *  @param[in]  cache        Unused.
 *
 *  @return Acceptor matching configuration.
 */
io::endpoint* factory::new_endpoint(
    config::endpoint& cfg,
    bool& is_acceptor,
    std::shared_ptr<persistent_cache> cache) const {
  (void)cache;

  // Find path to the file.
  std::string filename;
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("path")};
    if (it == cfg.params.end())
      throw exceptions::msg() << "file: no 'path' defined for file "
                                 "endpoint '"
                              << cfg.name << "'";
    filename = it->second;
  }

  // Generate opener.
  std::unique_ptr<opener> openr(new opener);
  openr->set_filename(filename);
  is_acceptor = false;
  return openr.release();
}
