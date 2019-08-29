local function check_index(socket)
  -- Ask for the index
  socket:write('HEAD /centreon?pretty HTTP/1.1\r\nHost: 127.0.0.1:9200\r\nAccept: */*\r\n\r\n')
  local answer = socket:read()
  if string.match(answer, "HTTP/1.1 200 OK") then
    return true
  end
  return false
end

local function init_index(socket)
  broker_log:info(1, "init_index")
  -- Initialize the index
  local header = 'PUT /centreon/_mapping/metrics?pretty HTTP/1.1\r\nHost: 127.0.0.1:9200\r\nAccept: */*\r\nContent-Type: application/json\r\n'
  local content = [[{
    "properties": {
      "name": { "type": "keyword" },
      "timestamp": { "type": "date" },
      "value": { "type": "double" }
    }
  }]]

  header = header .. 'Content-Length: ' .. content:len() .. "\r\n\r\n" .. content
  socket:write(header)
  local answer = socket:read()
  if answer:match("HTTP/1.1 200 OK") then
    broker_log:info(1, "Index really constructed")
    return true
  else
    broker_log:info(1, "Index construction failed")
    return false
  end
end

function init(conf)
  broker_log:set_parameters(3, '/tmp/log')
  local socket = broker_tcp_socket.new()
  socket:connect('127.0.0.1', 4242)

  if check_index(socket) then
    broker_log:info(1, "Index already constructed")
  else
    broker_log:info(1, "Index missing")
    if init_index(socket) then
      broker_log:info(1, "Index constructed")
    else
      broker_log:info(1, "Index construction failed")
    end
  end
  socket:close()
end

function write(d)
  return true
end
