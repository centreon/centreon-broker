local _event_handler = {}

local function build(service_id, host_id)
  local now = os.time()
  if not _event_handler.now then
    _event_handler.now = now
  else
    _event_handler.now_max = now
  end
  local retval = {
    category = 1,
    element = 6,
    _type = 65542,
    command_args = "arg" .. host_id .. " arg" .. service_id,
    command_line = "command" .. host_id .. "_" .. service_id,
    output = "output" .. host_id .. "_" .. service_id,
    timeout = 0,
    state_type = 0,
    state = (service_id * host_id) % 4,
    start_time = now,
    service_id = service_id,
    host_id = host_id,
    return_code = 0,
    type = 0,
    end_time = now + 2,
    execution_time = 2,
    early_timeout = 0,
  }
  return retval
end

local event_handler = {
  name = "Event handler",
  -- id: instance id
  -- name: instance name
  -- engine: Monitoring engine name in this instance
  -- pid: Monitoring engine pid
  --
  -- return: a neb::instance event
  build = function (stack, count, conn)
    local host_count = count.host * count.instance
    local service_count = count.service
    broker_log:info(0, "EVENT HANDLER ; service_count = "
                 .. service_count .. " ; host_count = " .. host_count)
    for j = 1,host_count do
      for i = 1,service_count do
        table.insert(
                stack,
                build(i + (j - 1) * service_count, j))
      end
    end
    broker_log:info(0, "EVENT HANDLER => FINISHED")
  end,

  check = function (conn, count)
    local host_count = count.host * count.instance
    local service_count = count.service
    local now = _event_handler.now
    local now_max = _event_handler.now_max
    broker_log:info(0, "CHECK EVENT HANDLER")
    local retval = true
    broker_log:info(0, "SELECT count(*) from eventhandlers where start_time>=" .. now .. " and start_time <= " .. now_max)
    local cursor, error_str = conn["storage"]:execute("SELECT count(*) from eventhandlers where start_time >=" .. now .. " and start_time <= " .. now_max)
    local row = cursor:fetch({}, "a")
    if row then
      if tonumber(row['count(*)']) ~= service_count * host_count then
        broker_log:error(0, "eventhandlers not complete")
        retval = false
      end
    end

    if not retval then
      broker_log:info(0, "CHECK EVENT HANDLERS => NOT DONE")
    else
      broker_log:info(0, "CHECK EVENT HANDLERS => DONE")
    end
    return retval
  end
}

return event_handler
