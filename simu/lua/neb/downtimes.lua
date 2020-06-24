local data = {
}

local function build(id)
  local now = os.time()
  if not data.start_time then
    data.start_time = now
    data.end_time = now
  end
  if now > data.end_time then
    data.end_time = now
  end

  local retval = {
    category = 1,
    element = 5,
    _type = 65541,
    actual_end_time = data.end_time,
    actual_start_time = data.start_time,
    author = "admin",
    downtime_type = 2,
    duration = 500,
    internal_id = id,
    end_time = data.end_time,
    entry_time = data.start_time,
    fixed = false,
    host_id = id,
    instance_id = 1,
    service_id = 0,
    start_time = data.start_time,
    was_started = true,
    comment = "Downtime comment",
  }
  return retval
end

local downtimes = {
  name = "Downtimes",
  -- id: instance id
  -- name: instance name
  -- engine: Monitoring engine name in this instance
  -- pid: Monitoring engine pid
  --
  -- return: a neb::instance event
  build = function (stack, count, conn)
    local downtime_count = count.host
    broker_log:info(0, "BUILD DOWNTIMES ; downtime_count = " .. tostring(downtime_count))
    for i = 1,downtime_count do
      table.insert(stack, build(i))
    end
    broker_log:info(0, "BUILD DOWNTIMES => FINISHED")
  end,

  check = function (conn, count)
    local downtime_count = count.host
    broker_log:info(0, "CHECK DOWNTIMES")
    local retval = true
    broker_log:info(3, "SELECT host_id FROM downtimes WHERE start_time = "
                                .. data.start_time .. " ORDER BY host_id")
    local cursor, error_str = conn["storage"]:execute("SELECT host_id FROM downtimes WHERE start_time = "
                                .. data.start_time .. " ORDER BY host_id")
    local row = cursor:fetch({}, "a")
    local id = 1
    while row do
      broker_log:info(1, "Check for downtime of host id " .. id)
      if tonumber(row.host_id) ~= id then
        broker_log:error(0, "Row found host_id = "
            .. row.host_id .. " instead of host_id = " .. id)
        retval = false
        break
      end
      id = id + 1
      row = cursor:fetch({}, "a")
    end

    if id <= downtime_count then
      broker_log:info(0, "CHECK DOWNTIMES => NOT FINISHED")
      retval = false
    end
    if not retval then
      broker_log:info(0, "CHECK DOWNTIMES => NOT DONE")
    else
      broker_log:info(0, "CHECK DOWNTIMES => DONE")
    end
    return retval
  end
}

return downtimes
