local _host_status = {}

local function build(host_id)
  if not _host_status.now then
    _host_status.now = os.time() - 5000
  end
  local now = _host_status.now
  local d1 = math.random(0, 100)
  local d2 = math.random(0, 100)

  local retval = {
    category = 1,
    element = 14,
    _type = 65550,
    acknowledged = false,
    acknowledgement_type = 0,
    active_checks = true,
    check_attempt = 1,
    check_command = "check-bench-alive",
    check_interval = 3,
    check_period = "24x7",
    check_type = 0,
    checked = true,
    enabled = true,
    event_handler = "",
    event_handler_enabled = true,
    execution_time = 0.293672,
    flap_detection = true,
    flapping = false,
    host_id = host_id,
    last_check = now,
    last_hard_state = 0,
    last_hard_state_change = now,
    last_state_change = now,
    last_time_up = now,
    last_update = now,
    latency = 0.523,
    max_check_attempts = 5,
    next_check = now + 5000,
    no_more_notifications = false,
    notification_number = 0,
    notify = false,
    obsess_over_host = true,
    output = "OK - web-frontend-crm-mobility: rta " .. d1 .. "ms, lost " .. d2 .. "\n",
    passive_checks = false,
    percent_state_change = 0,
    perfdata = "rta=" .. d1 .. "ms;3000.000;5000.000;0; pl=" .. d2 .. ";80;100;;",
    retry_interval = 1,
    scheduled_downtime_depth = 0,
    should_be_scheduled = true,
    state = 1,
    state_type = 1,
  }
  return retval
end

local host_status = {
  name = "Host status",
  --
  -- return: a neb::instance event
  build = function (stack, count, conn)
    local host_count = count.host * count.instance
    broker_log:info(0, "BUILD HOST STATUS ; host_count = " .. host_count)
    for j = 1,host_count do
      table.insert(stack, build(j))
    end
    broker_log:info(0, "BUILD HOST STATUS => FINISHED")
  end,

  check = function (conn, count)
    local host_count = count.host * count.instance
    local now = _host_status.now
    broker_log:info(0, "CHECK HOST STATUS")
    local retval = true
    broker_log:info(0, "SELECT count(*) from hosts where last_check=" .. now)
    local cursor, error_str = conn["storage"]:execute(
        "SELECT count(*) from hosts where last_check=" .. now)
    local row = cursor:fetch({}, "a")
    if row then
      if tonumber(row['count(*)']) ~= host_count then
        broker_log:error(0, "We should have a result equal to " .. host_count)
        retval = false
      end
    end

    if not retval then
      broker_log:info(0, "CHECK HOST STATUS => NOT DONE")
    else
      broker_log:info(0, "CHECK HOST STATUS => DONE")
    end
    return retval
  end
}

return host_status
