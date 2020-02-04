--
-- Copyright 2020 Centreon
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
-- This is a streamconnector to send data on the Centreon data lake.
-- To use it, you have to configure a streamconnector broker output
-- with several parameters:
--
-- filters_file (string): The filters file is a json file containing metrics to
--     send. It is of the following form:
--         {
--           "filters": [{
--             "host_id": 1,
--             "service_id": 1,
--             "metric_name": [ "curve1", "curve2" ]
--           },
--           {
--             "host_id": 2,
--             "service_id": 3,
--             "metric_name": [ "curve3", "curve26", "curve12" ]
--           }]
--         }
--     And it is by default located at
--     /etc/centreon-broker/anomaly-detection.json
--
-- logfile (string): It is this script log file. You have to provide a full
--                   path to the file.
--                   Default value: /var/log/centreon-broker/anomaly-detection.log
-- destination (string): It is the domain name of the destination
--                   Default value: int.mycentreon.net
-- source: A string to represent this Central
-- proxy (string): The proxy configuration. No value is given by default.
-- max_queue_size (number): The queue max size before data to be sent.
-- token (string): The token to access the platform.
--

package.path = '/usr/share/centreon-broker/lua/?.lua;' .. package.path
package.cpath = '/usr/share/centreon-broker/lua/?.so;' .. package.cpath

local cURL = require "cURL"
local md5 = require "md5"

local data = {
  logfile = "/var/log/centreon-broker/anomaly-detection.log",
  filters_file = "/etc/centreon-broker/anomaly-detection.json",
  destination = "prod.mycentreon.com",
  source = "centreon_onprem",
  proxy = "",   --"proxy.int.centreon.com:3128"
  max_queue_size = 10,
  token = "tRGkO5tjVA2qHOYfgKkY21SoTYPNif6ualYmeBpm",
  queue = {},
  filter = {},
}

local function parse_filters(dec)
  local retval = {}
  local filters = dec.filters
  for i,v in ipairs(filters) do
    -- We build a key with the Cantor pairing function
    local key = (v.host_id + v.service_id) * (v.host_id + v.service_id + 1) / 2 + v.host_id
    data.filter[key] = v.metrics
  end
end

-- Init function. It is called when the connector is started by Broker.
--
-- @param conf: The configuration given by the user
--
function init(conf)
  if conf.logfile then
    data.logfile = conf.logfile
  end
  if conf.filters_file then
    data.filters_file = conf.filters_file
  end
  if conf.destination then
    data.destination = conf.destination
  end

  -- A string to represent this Central source
  if conf.source then
    data.source = conf.source
  end

  if conf.proxy then
    data.proxy = conf.proxy
  end

  if conf.max_queue_size then
    data.max_queue_size = conf.max_queue_size
  end

  if conf.filters_file then
    data.filters_file = conf.filters_file
  end

  if conf.token then
    data.token = conf.token
  end

  broker_log:set_parameters(3, data.logfile)

  local file = open(data.filters_file, "r") -- r read mode and b binary mode
  if not file then
    broker_log.info("Anomaly detection: No filter configured. Nothing will be send to the remote server")
  else
    local content = file:read("*a")
    file:close()
    local dec = broker.json_decode(content)
    parse_filters(dec)
  end
end

local function flush()
  local metrics = table.concat(data.queue, "\\n")

  local timestamp = os.date("!%Y-%m-%dT%TZ")
  c = cURL.easy{
    url        = "https://api.a." .. data.destination .. "/v1/observability",
    post       = true,
    httpheader = {
      "Content-Type: application/vnd.centreon+json",
      "x-api-key: " .. data.token
    };
    postfields = '{\
"version": "1.0.0",\
"data": [\
  {\
    "type": "metric",\
    "timestamp": "' .. timestamp .. '",\
    "version": "1.0.0",\
    "contentType": "warp10/plaintext",\
    "content": "' .. metrics .. '"\
  }\
]\
}';
  }
  if data.proxy and data.proxy ~= "" then
    c:setopt(cURL.OPT_PROXY, data.proxy)
  end
  c:perform()
  resp_code = c:getinfo(cURL.INFO_RESPONSE_CODE)
  if resp_code ~= 200 then
    broker_log:error(1, "HTTP Code : " .. resp_code)
    if res then
      broker_log:error(1, "Response body : " .. tostring(res))
    end
  end

  broker_log:info(3, "Sent data: " .. metrics)
  data.queue = {}
  return true
end

local function match_filter(h, s)
  local key = (h + s) * (h + s + 1) / 2 + h
  return data.filter[key]
end

function write(e)
  -- We only want service_status events
  if e.category ~= 1 or e.element ~= 24 then
    return false
  end

  -- Are we interested by this service? If yes, what metrics should we keep?
  local metrics = match_filter(e.host_id, e.service_id)
  if not metrics then
    return false
  end

  local perfdata = broker.parse_perfdata(e.perfdata)
  if not perfdata or not next(perfdata) then
    broker_log:info(1, "No metric")
    return false
  end

  local labels = "_source=" .. data.source .. ",service_id=" .. e.service_id .. ",host_id=" .. e.host_id
  local srvdesc = broker_cache:get_service_description(e.host_id, e.service_id)
  local hostname = broker_cache:get_hostname(e.host_id)
  if srvdesc then
    labels = labels .. ", service_name=" .. srvdesc
  end
  if hostname then
    labels = labels .. ", host_name=" .. hostname
  end

  for i,k in ipairs(metrics) do
    local metric = e.last_check .. '000000// ' .. k .. '{' .. labels .. '} ' .. perfdata[k]
    table.insert(data.queue, metric)
  end

  local retval = false
  if #data.queue >= data.max_queue_size then
    retval = flush()
  end

  return retval
end
