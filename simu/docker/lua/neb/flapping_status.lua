local _flapping_status = {}

local function build(service_id, host_id)
  local now = os.time()
  if not _flapping_status.start then
    _flapping_status.start = now
    _flapping_status.finish = now
  end
  if now > _flapping_status.finish then
    _flapping_status.finish = now
  end
  local info = "_" .. service_id .. "_" .. host_id
  local d1 = math.random(0, 100)
  local d2 = math.random(0, 100)
  local d3 = math.random(0, 100)
  local d4 = math.random(0, 100)
  local d5 = math.random(0, 100)
  local retval = {
    category = 1,
    element = 7,
    _type = 65543,
    event_time = now,
    event_type = 1,
    flapping_type = 2,
    high_threshold = 3.14,
    host_id = host_id,
    low_threshold = 0.,
    percent_state_change = 0.26,
    reason_type = 1,
    service_id = service_id,
  }
  return retval
end

local flapping_status = {
  name = "Flapping status",

  build = function (stack, count, conn)
    local service_count = count.service
    local host_count = count.host * count.instance
    broker_log:info(0, "BUILD FLAPPING STATUS ; service_count = "
                 .. service_count .. " ; host_count = " .. host_count)
    for j = 1,host_count do
      for i = 1,service_count do
        table.insert(
                stack,
                build(i + (j - 1) * service_count, j))
      end
    end
    broker_log:info(0, "BUILD FLAPPING STATUS => FINISHED")
  end,

  check = function (conn, count)
    local service_count = count.service
    local host_count = count.host * count.instance
    local start = _flapping_status.start
    if not start then
      return false
    end
    local finish = _flapping_status.finish
    broker_log:info(0, "CHECK FLAPPING STATUS")
    local retval = true
    broker_log:info(0, "SELECT count(*) from flappingstatuses where event_time>=" .. start .. " and event_time<=" .. finish)
    local cursor, error_str = conn["storage"]:execute("SELECT count(*) from flappingstatuses where event_time>=" .. start .. " and event_time<=" .. finish)
    local row = cursor:fetch({}, "a")
    if row then
      if tonumber(row['count(*)']) ~= service_count * host_count then
        broker_log:error(0, "flappingstatuses not complete")
        retval = false
      end
    end

    if not retval then
      broker_log:info(0, "CHECK FLAPPING STATUS => NOT DONE")
    else
      broker_log:info(0, "CHECK FLAPPING STATUS => DONE")
    end
    return retval
  end
}

return flapping_status
