local function build(id, host_id, description, dname)
  local now = os.time()
  local retval = {
    category = 1,
    element = 23,
    _type = 65559,
    acknowledged = false,
    acknowledgement_type = 0,
    action_url = "",
    active_checks = false,
    check_attempt = 0,
    check_command = "",
    check_freshness = false,
    check_interval = 0,
    check_period = "",
    check_type = 0,
    checked = false,
    default_active_checks = false,
    default_event_handler_enabled = false,
    default_flap_detection = false,
    default_notify = false,
    default_passive_checks = false,
    description = description,
    display_name = dname,
    enabled = true,
    event_handler = "",
    event_handler_enabled = false,
    execution_time = 0,
    first_notification_delay = 0,
    flap_detection = false,
    flap_detection_on_critical = false,
    flap_detection_on_ok = false,
    flap_detection_on_unknown = false,
    flap_detection_on_warning = false,
    flapping = false,
    freshness_threshold = 0,
    high_flap_threshold = 0,
    host_id = host_id,
    icon_image = "",
    icon_image_alt = "",
    last_hard_state = 4,
    last_update = 1541680324,
    latency = 0,
    low_flap_threshold = 0,
    max_check_attempts = 0,
    no_more_notifications = false,
    notes = "",
    notes_url = "",
    notification_interval = 0,
    notification_number = 0,
    notification_period = "",
    notify = false,
    notify_on_critical = false,
    notify_on_downtime = false,
    notify_on_flapping = false,
    notify_on_recovery = false,
    notify_on_unknown = false,
    notify_on_warning = false,
    obsess_over_service = false,
    output = "",
    passive_checks = false,
    percent_state_change = 0,
    perfdata = "",
    retain_nonstatus_information = false,
    retain_status_information = false,
    retry_interval = 0,
    scheduled_downtime_depth = 0,
    service_id = id,
    should_be_scheduled = false,
    stalk_on_critical = false,
    stalk_on_ok = false,
    stalk_on_unknown = false,
    stalk_on_warning = false,
    state = 3,
    state_type = 0,
    volatile = false,
  }
  return retval
end

local services = {
  name = "Services",
  -- id: instance id
  -- name: instance name
  -- engine: Monitoring engine name in this instance
  -- pid: Monitoring engine pid
  --
  -- return: a neb::instance event
  build = function (stack, count, conn)
    local service_count = count.service
    local host_count = count.host * count.instance
    broker_log:info(0, "BUILD SERVICES ; service_count = " .. service_count .. " ; host_count = " .. host_count)
    for j = 1,host_count do
      for i = 1,service_count do
        table.insert(
                stack,
                build(i + (j - 1) * service_count, j, "host_" .. i .. j, "1.2.i." .. j))
      end
    end
    broker_log:info(0, "BUILD SERVICES => FINISHED")
  end,

  check = function (conn, count)
    local service_count = count.service
    local host_count = count.host * count.instance
    local now = os.time()
    broker_log:info(0, "CHECK SERVICES")
    local retval = true
    local cursor, error_str = conn["storage"]:execute([[SELECT service_id, host_id from services ORDER BY service_id]])
    local row = cursor:fetch({}, "a")
    local id = 1
    local host_id = 1
    while row do
      broker_log:info(1, "Check for service " .. id)
      if tonumber(row.service_id) ~= id or tonumber(row.host_id) ~= host_id then
        broker_log:error(0, "Row found service_id = "
            .. row.service_id .. " host_id = " .. row.host_id
            .. " instead of service_id = " .. id .. " and host_id = " .. host_id)
        retval = false
        break
      end
      if id % service_count == 0 then
        host_id = host_id + 1
      end
      id = id + 1
      row = cursor:fetch({}, "a")
    end

    if id <= service_count * host_count then
      broker_log:info(0, "CHECK SERVICES => NOT FINISHED")
      retval = false
    end
    if not retval then
      broker_log:info(0, "CHECK SERVICES => NOT DONE")
    else
      broker_log:info(0, "CHECK SERVICES => DONE")
    end
    return retval
  end
}

return services
