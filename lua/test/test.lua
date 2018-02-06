--
-- Copyright 2017 Centreon
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.
--
-- For more information : contact@centreon.com
--


-- Init function
function init(params)
  broker.log_error("init...")
  for i,v in pairs(params) do
    broker.log_error(i .. " => "..tostring(v))
  end
  broker.log_error("init() finished")
end

-- Write function
function write(d)
  broker.log_error("write()")
  for i,v in pairs(d) do
    broker.log_error(i .. " => " .. tostring(v))
  end
  broker.log_error("write() finished")
  return true
end

-- Filter function
function filter(typ, cat)
  broker.log_error("filter()")
  return true
end
