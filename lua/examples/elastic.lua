local elastic = {
  max_row   = 5,
  rows      = {}
}

--------------------------------------------------------------------------------
--  Check if the desired index exists on the elasticsearch server
--  @param socket the socket connected to the elasticsearch server
--
--  @return a boolean true on success, false otherwise.
--------------------------------------------------------------------------------
local function check_index(socket)
  -- Ask for the index
  socket:write('GET /centreon/_mapping?pretty HTTP/1.1\r\nHost: '
           .. elastic.address .. ':' .. elastic.port
           .. '\r\nAccept: */*\r\n\r\n')
  local answer = socket:read()
  if string.match(answer, "HTTP/1.1 200 OK") then
    return true
  end
  return false
end

--------------------------------------------------------------------------------
--  Initializes the mapping on the elasticsearcg server
--  @param socket the socket connected to the elasticsearch server
--
--  @return true on success, false otherwise
--------------------------------------------------------------------------------
local function init_index(socket)
  broker_log:info(1, "init_index")
  -- Initialize the index
  local header = 'PUT /centreon?pretty HTTP/1.1\r\nHost: '
          .. elastic.address .. ':' .. elastic.port
          .. '\r\nAccept: */*\r\nContent-Type: application/json\r\n'
  local content = [[{
    "mappings": {
      "metrics": {
        "_all": { "enabled": false },
        "properties": {
          "host": { "type": "keyword" },
          "metric": { "type": "keyword" },
          "value": { "type": "double" },
          "timestamp": { "type": "date" }
        }
      }
    }
  }
]]

  header = header .. 'Content-Length: '
    .. content:len() .. "\r\n\r\n" .. content
  socket:write(header)
  local answer = socket:read()
  if answer:match("HTTP/1.1 200 OK") then
    broker_log:info(1, "Index constructed")
    return true
  else
    broker_log:info(1, "Index construction failed")
    return false
  end
end

--------------------------------------------------------------------------------
--  Initialization of the module
--  @param conf A table containing data entered by the user through the GUI
--------------------------------------------------------------------------------
function init(conf)
  broker_log:set_parameters(1, '/tmp/log')
  if conf['elastic-address'] and conf['elastic-address'] ~= "" then
    elastic.address = conf['elastic-address']
  else
    error("Unable to find the 'elastic-address' value of type 'string'")
  end

  if conf['elastic-port'] and conf['elastic-port'] ~= "" then
    elastic.port = conf['elastic-port']
  else
    error("Unable to find the 'elastic-port' value of type 'number'")
  end

  if conf['max-row'] then
    elastic.max_row = conf['max-row']
  else
    error("Unable to find the 'elastic-port' value of type 'number'")
  end
  elastic.socket = broker_tcp_socket.new()
  elastic.socket:connect(elastic.address, elastic.port)

  if not check_index(elastic.socket) then
    broker_log:info(3, "Index missing")
    if init_index(elastic.socket) then
      broker_log:info(1, "Index constructed")
    else
      broker_log:error(1, "Index construction failed")
      error("Index construction failed")
    end
  end
  elastic.socket:close()
end

--------------------------------------------------------------------------------
--  Called when the data limit count is reached.
--------------------------------------------------------------------------------
local function flush()
  local retval = true
  if #elastic.rows > 0 then
    elastic.socket:connect(elastic.address, elastic.port)
    local header = "POST /centreon/metrics/_bulk HTTP/1.1\r\nHost: "
            .. elastic.address .. ":" .. elastic.port .. "\r\n"
            .. "Accept: */*\r\n"
            .. "Content-Type: application/json\r\n"

    local data = ''
    for k,v in pairs(elastic.rows) do
      data = data .. '{"index":{}}\n' .. broker.json_encode(v) .. '\n'
    end

    header = header .. 'Content-Length: '
      .. data:len() .. "\r\n\r\n" .. data
    elastic.socket:write(header)
    local answer = elastic.socket:read()
    local ret
    if answer:match("HTTP/1.1 200 OK") then
      ret = true
    else
      broker_log:error(1, "Unable to write data on the server")
      ret = false
    end
    if ret then
      elastic.rows = {}
      elastic.socket:close()
    else
      retval = false
    end
  end
  return retval
end

--------------------------------------------------------------------------------
--  Function attached to the write event.
--------------------------------------------------------------------------------
function write(d)

  local hostname = broker_cache:get_hostname(d.host_id)
  if not hostname then
    broker_log:error(1, "host name for id " .. d.host_id .. " unknown")
  else
    broker_log:info(3, tostring(d.ctime)
                       .. ' --- ' .. hostname .. ' ; '
                       .. d.name .. ' ; ' .. tostring(d.value))

    elastic.rows[#elastic.rows + 1] = {
      timestamp = d.ctime * 1000,
      host = hostname,
      metric = d.name,
      value = d.value
    }
  end

  if #elastic.rows >= elastic.max_row then
    return flush()
  end
  return false
end

--------------------------------------------------------------------------------
--  The filter function. When it returns false, the write function is not
--  called.
--  @param category The event category
--  @param element  The event sub-category.
--
--  @return a boolean true when the event is accepted, false otherwise.
--------------------------------------------------------------------------------
function filter(category, element)
  if category == 3 and element == 1 then
    return true
  end
  return false
end
