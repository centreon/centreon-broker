/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#ifndef CCB_TEMPORARY_ENDPOINT_HH
#define CCB_TEMPORARY_ENDPOINT_HH

#include "com/centreon/broker/io/endpoint.hh"

CCB_BEGIN()

/**
 *  @class temporary_endpoint temporary_endpoint.hh
 *  @brief Temporary endpoint.
 *
 *  Endpoint that can be set to generate errors or not.
 */
class temporary_endpoint : public io::endpoint {
 public:
  temporary_endpoint(std::string const& id = "");
  temporary_endpoint(temporary_endpoint const& se);
  ~temporary_endpoint();
  temporary_endpoint& operator=(temporary_endpoint const& se);
  io::endpoint* clone() const;
  void close();
  std::shared_ptr<io::stream> open();

 private:
  std::string _id;
};

CCB_END()

#endif  // !CCB_TEMPORARY_ENDPOINT_HH
