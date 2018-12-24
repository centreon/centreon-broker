local data = {}

local function build_check(id, instance_id, name, address)
  local retval = {
    category = 1,
    element = 8,
    _type = 65544,
    command_line = "super_command_de_check",
    host_id = id,
  }
  return retval
end

local function build_log(id, instance_id, name, address)
  if not data.now then
    local now = os.time()
    data.now = now
  end
  local retval = {
    category = 1,
    element = 17,
    _type = 65553,
    retry = 0,
    instance_name = "Central_" .. instance_id,
    msg_type = 5,
    output = "Je viens de lancer un check",
    status = 0,
    host_name = name,
    host_id = id,
    ctime = data.now
  }
  return retval
end

local hosts = {
  name = "Hosts checks",
  -- id: instance id
  -- name: instance name
  -- engine: Monitoring engine name in this instance
  -- pid: Monitoring engine pid
  --
  -- return: a neb::instance event
  build = function (stack, count)
    local host_count = count.host
    local poller_count = count.instance
    for j = 1,poller_count do
      for i = 1,host_count do
        broker_log:info(0, "BUILD HOST CHECK ; host_id = " .. i .. " ; instance_id = " .. j)
        table.insert(
                stack,
                build_check(i + (j - 1) * host_count, j, "host_" .. i .. j, "1.2.i." .. j))
        broker_log:info(0, "BUILD HOST CHECK LOG ; host_id = " .. i .. " ; instance_id = " .. j)
        table.insert(
                stack,
                build_log(i + (j - 1) * host_count, j, "host_" .. i .. j, "1.2.i." .. j))
      end
    end
    broker_log:info(0, "BUILD HOSTS CHECKS AND LOGS => FINISHED")
  end,

  check = function (conn, count)
    local host_count = count.host
    local poller_count = count.instance
    broker_log:info(0, "CHECK HOSTS CHECKS")
    local retval = true
    local cursor, error_str = conn:execute([[SELECT host_id, command_line from hosts ORDER BY host_id]])
    local row = cursor:fetch({}, "a")
    local id = 1
    local instance_id = 1
    while row do
      broker_log:info(1, "Check for host " .. id)
      if tonumber(row.host_id) ~= id or row.command_line ~= "super_command_de_check" then
        broker_log:error(0, "Row found host_id = "
            .. row.host_id .. " command_line = " .. tostring(row.command_line)
            .. " instead of host_id = " .. id .. " and instance_id = 'super_command_de_check'")
        retval = false
        break
      end
      if id % host_count == 0 then
        instance_id = instance_id + 1
      end
      id = id + 1
      row = cursor:fetch({}, "a")
    end

    broker_log:info(0, "CHECK HOSTS LOGS")
    local retval = true
    local cursor, error_str = conn:execute([[SELECT host_id,output from logs WHERE ctime = ]] .. data.now .. " ORDER BY host_id")
    local row = cursor:fetch({}, "a")
    local id = 1
    local instance_id = 1
    while row do
      broker_log:info(1, "Check for host " .. id)
      if tonumber(row.host_id) ~= id or row.output ~= "Je viens de lancer un check" then
        broker_log:error(0, "Row found host_id = "
            .. row.host_id .. " output = " .. tostring(row.output))
        retval = false
        break
      end
      if id % host_count == 0 then
        instance_id = instance_id + 1
      end
      id = id + 1
      row = cursor:fetch({}, "a")
    end

    if id <= host_count * poller_count then
      broker_log:info(0, "NOT FINISHED")
      retval = false
    end
    if not retval then
      broker_log:info(0, "CHECK HOSTS CHECKS / LOGS => NOT DONE")
    else
      broker_log:info(0, "CHECK HOSTS CHECKS / LOGS => DONE")
    end
    return retval
  end
}

return hosts
