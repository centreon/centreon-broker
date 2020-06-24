local _instance_status = {}

local function build(id)
  local now = os.time()
  if not _instance_status.start then
    _instance_status.start = now
    _instance_status.finish = now
  end
  if now > _instance_status.finish then
    _instance_status.finish = now
  end

  local retval = {
    category = 1,
    element = 16,
    _type = 65552,
    instance_id = id,
    active_host_checks = 1,
    active_service_checks = 1,
    check_hosts_freshness = 0,
    check_services_freshness = 1,
    last_alive = now,
    obsess_over_hosts = 1,
    obsess_over_services = 1,
    passive_host_checks = 1,
    passive_service_checks = 1,
  }
  return retval
end

local instance_status = {
  name = "Instance status",

  build = function (stack, count, conn)
    local poller_count = count.instance
    broker_log:info(0, "BUILD INSTANCES STATUS ; poller_count = "
        .. tostring(poller_count))
    for i = 1,poller_count do
      table.insert(stack, build(i))
    end
  end,
  check = function (conn, count)
    if not _instance_status.finish then
      return false
    end
    local poller_count = count.instance
    broker_log:info(0, "CHECK INSTANCES STATUS with poller_count = " .. poller_count)
    local retval = true
    local cursor, error_str = conn["storage"]:execute("SELECT instance_id, obsess_over_hosts from instances WHERE last_alive>=" .. _instance_status.start .. " AND last_alive<=" .. _instance_status.finish)
    local row = cursor:fetch({}, "a")
    local id = 1
    while row do
      broker_log:info(1, "Check for instance status " .. id)
      if tonumber(row.obsess_over_hosts) ~= 1 then
        broker_log:error(0, "Row found instance_id = "
            .. row.instance_id .. " obsess_over_hosts = " .. row.obsess_over_hosts
            .. " instead of instance_id = " .. id .. " and obsess_over_hosts = 1")
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
      broker_log:info(0, "CHECK_INSTANCE_STATUS => NOT DONE")
    else
      broker_log:info(0, "CHECK_INSTANCE_STATUS => DONE")
    end
    return retval
  end
}

return instance_status
