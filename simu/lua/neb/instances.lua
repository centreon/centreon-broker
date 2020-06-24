local function build(id, name, engine, pid)
  local now = os.time()
  local retval = {
    category = 1,
    element = 15,
    _type = 65551,
    instance_id = id,
    name = name,
    active_host_checks = 1,
    active_service_checks = 1,
    check_hosts_freshness = 0,
    check_services_freshness = 1,
    engine = engine,
    event_handlers = 1,
    flap_detection = 0,
    last_alive = now,
    notifications = 1,
    passive_host_checks = 1,
    passive_service_checks = 1,
    pid = pid,
    running = 1,
    start_time = now,
    version = "1.8.1"
  }
  return retval
end

local instances = {
  name = "Instances",
  -- id: instance id
  -- name: instance name
  -- engine: Monitoring engine name in this instance
  -- pid: Monitoring engine pid
  --
  -- return: a neb::instance event
  build = function (stack, count, conn)
    local poller_count = count.instance
    broker_log:info(0, "BUILD INSTANCES ; poller_count = "
        .. tostring(poller_count))
    for i = 1,poller_count do
      table.insert(
              stack,
              build(i, "Central_" .. i, "Centreon Engine " .. i, 600 + i))
    end
  end,
  check = function (conn, count)
    local poller_count = count.instance
    broker_log:info(0, "CHECK INSTANCES with poller_count = " .. poller_count)
    local retval = true
    local cursor, error_str = conn["storage"]:execute([[SELECT instance_id, name from instances ORDER BY instance_id]])
    local row = cursor:fetch({}, "a")
    local id = 1
    while row do
      broker_log:info(1, "Check for instance " .. id)
      local name = "Central_" .. id
      if tonumber(row.instance_id) ~= id or row.name ~= name then
        broker_log:error(0, "Row found instance_id = "
            .. row.instance_id .. " name = " .. row.name
            .. " instead of instance_id = " .. id .. " and name = " .. name)
        retval = false
        break
      end
      id = id + 1
      row = cursor:fetch({}, "a")
    end

    if id < poller_count then
      retval = false
    end
    if not retval then
      broker_log:info(0, "CHECK_INSTANCES => NOT DONE")
    else
      broker_log:info(0, "CHECK_INSTANCES => DONE")
    end
    return retval
  end
}

return instances
