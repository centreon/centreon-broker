local data = {}

local function build_check(host_id, service_id)
  local retval = {
    category = 1,
    element = 19,
    _type = 65555,
    command_line = "super_command_de_svc",
    service_id = service_id,
    host_id = host_id,
  }
  return retval
end

local service_checks = {
  name = "Service checks",
  -- id: instance id
  -- name: instance name
  -- engine: Monitoring engine name in this instance
  -- pid: Monitoring engine pid
  --
  -- return: a neb::instance event
  build = function (stack, count, conn)
    local service_count = count.service
    local host_count = count.host * count.instance
    for j = 1,host_count do
      for i = 1,service_count do
        local service_id = i + (j - 1) * service_count
        local host_id = j
        broker_log:info(0, "BUILD SERVICE CHECKS ; host_id = " .. host_id .. " ; service_id = " .. service_id)
        table.insert(
                stack,
                build_check(host_id, service_id))
      end
    end
    broker_log:info(0, "BUILD SERVICE CHECKS => FINISHED")
  end,

  check = function (conn, count)
    broker_log:info(0, "CHECK SERVICES CHECKS")
    local service_count = count.service
    local host_count = count.host * count.instance
    local cursor, error_str = conn["storage"]:execute([[SELECT host_id, service_id, command_line from services ORDER BY host_id, service_id]])
    local retval = true
    for j = 1,host_count do
      for i = 1,service_count do
        local row = cursor:fetch({}, "a")
        if not row then
          retval = false
          break
        end
        local service_id = i + (j - 1) * service_count
        local host_id = j
        broker_log:info(0, "CHECK SERVICE CHECKS ; host_id = " .. host_id .. " ; service_id = " .. service_id)
        if tonumber(row.host_id) ~= host_id or tonumber(row.service_id) ~= service_id or row.command_line ~= "super_command_de_svc" then
          broker_log:error(0, "Row found host_id = "
              .. row.host_id
              .. " service_id = " .. row.service_id
              .. " command_line = " .. tostring(row.command_line)
              .. " instead of host_id = " .. host_id
              .. " service_id = " .. service_id
              .. " and command_line = 'super_command_de_svc'")
          retval = false
          break
        end
      end
      if not retval then
        break
      end
    end

    if not retval then
      broker_log:info(0, "CHECK HOSTS CHECKS / LOGS => NOT DONE")
    else
      broker_log:info(0, "CHECK HOSTS CHECKS / LOGS => DONE")
    end
    return retval
  end
}

return service_checks
