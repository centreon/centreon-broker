local _service_status = {}

local function build(service_id, host_id, metric)
  local now = os.time()
  if not _service_status.start then
    _service_status.start = now
    _service_status.finish = now
  end
  if now > _service_status.finish then
    _service_status.finish = now
  end
  local info = "_" .. service_id .. "_" .. host_id
  local d1 = math.random(0, 100)
  local d2 = math.random(0, 100)
  local d3 = math.random(0, 100)
  local d4 = math.random(0, 100)
  local d5 = math.random(0, 100)
  local retval = {
    category = 1,
    element = 24,
    _type = 65560,
    acknowledged = false,
    acknowledgement_type = 0,
    active_checks = true,
    check_attempt = 1,
    check_command = "OS-Linux-SNMP-Cpu",
    check_interval = 5,
    check_period = "24x7",
    check_type = 0,
    checked = true,
    enabled = true,
    event_handler = "",
    event_handler_enabled = true,
    execution_time = 0.204409,
    flap_detection = true,
    flapping = false,
    host_id = host_id,
    last_check = now,
    last_hard_state = 0,
    last_hard_state_change = 1541149764,
    last_state_change = 1541678384,
    last_time_ok = now,
    last_update = now,
    latency = 0.332,
    max_check_attempts = 3,
    next_check = now + 500,
    no_more_notifications = false,
    notification_number = 0,
    notify = false,
    obsess_over_service = true,
    output = "OK: Message from service status " .. info .."\n",
    passive_checks = false,
    percent_state_change = 0,
    perfdata = "'d1" .. info .. "'=" .. d1 .. "%;;;0;100 "
            .. "'d2" .. info .. "'=" .. d2 .. "%;;;0;100 "
            .. "'d3" .. info .. "'=" .. d3 .. "%;;;0;100 "
            .. "'d4" .. info .. "'=" .. d4 .. "%;;;0;100 "
            .. "'d5" .. info .. "'=" .. d5 .. "%;;;0;100 ",
    retry_interval = 1,
    scheduled_downtime_depth = 0,
    service_id = service_id,
    should_be_scheduled = true,
    state = 0,
    state_type = 1,
  }
  return retval
end

local service_status = {
  name = "Service status",
  -- id: instance id
  -- name: instance name
  -- engine: Monitoring engine name in this instance
  -- pid: Monitoring engine pid
  --
  -- return: a neb::instance event
  build = function (stack, count, conn)
    local metric_count = count.metric
    local service_count = count.service
    local host_count = count.host * count.instance
    broker_log:info(0, "BUILD SERVICE STATUS ; service_count = "
                 .. service_count .. " ; host_count = " .. host_count)
    for metric = 1,metric_count do
      for j = 1,host_count do
        for i = 1,service_count do
          table.insert(
                  stack,
                  build(i + (j - 1) * service_count, j, metric))
        end
      end
    end
    broker_log:info(0, "BUILD SERVICE STATUS => FINISHED")
  end,

  check = function (conn, count)
    local service_count = count.service
    local host_count = count.host * count.instance
    local start = _service_status.start
    if not start then
      return false
    end
    local finish = _service_status.finish
    broker_log:info(0, "CHECK SERVICE STATUS")
    local retval = true
    broker_log:info(0, "SELECT count(*) from data_bin where ctime>=" .. start .. " and ctime <= " .. finish)
    local cursor, error_str = conn["storage"]:execute("SELECT count(*) from data_bin where ctime >=" .. start .. " and ctime <= " .. finish)
    local row = cursor:fetch({}, "a")
    if row then
      if tonumber(row['count(*)']) ~= 5 * service_count * host_count * count.metric then
        broker_log:error(0, "index_data not complete, we have " .. row['count(*)'] .. " rows and not " .. tostring(5 * service_count * host_count * count.metric) .. " as expected")
        retval = false
      end
    end

    if not retval then
      broker_log:info(0, "CHECK SERVICE STATUS => NOT DONE")
    else
      broker_log:info(0, "CHECK SERVICE STATUS => DONE")
    end
    return retval
  end
}

return service_status
