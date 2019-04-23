local function build(id, name)
  local retval = {
    category = 1,
    element = 10,
    _type = 65546,
    hostgroup_id = id,
    name = name,
  }
  return retval
end

local hostgroups = {
  name = "Host groups",
  -- id: instance id
  -- name: instance name
  -- engine: Monitoring engine name in this instance
  -- pid: Monitoring engine pid
  --
  -- return: a neb::instance event
  build = function (stack, count, conn)
    local hostgroup_count = count.group
    broker_log:info(0, "BUILD HOSTGROUPS ; hostgroup_count = " .. tostring(hostgroup_count))
    for i = 1,hostgroup_count do
      table.insert(
              stack,
              build(i, "hostgroup_" .. i))
    end
    broker_log:info(0, "BUILD HOSTGROUPS => FINISHED")
  end,

  check = function (conn, count)
    local hostgroup_count = count.group
    local now = os.time()
    broker_log:info(0, "CHECK HOSTGROUPS")
    local retval = true
    local cursor, error_str = conn["storage"]:execute([[SELECT hostgroup_id from hostgroups ORDER BY hostgroup_id]])
    local row = cursor:fetch({}, "a")
    local id = 1
    while row do
      broker_log:info(1, "Check for hostgroup " .. id)
      if tonumber(row.hostgroup_id) ~= id then
        broker_log:error(0, "Row found hostgroup_id = "
            .. row.hostgroup_id
            .. " instead of hostgroup_id = " .. id)
        retval = false
        break
      end
      id = id + 1
      row = cursor:fetch({}, "a")
    end

    if id <= hostgroup_count then
      broker_log:info(0, "CHECK HOSTGROUPS => NOT FINISHED")
      retval = false
    end
    if not retval then
      broker_log:info(0, "CHECK HOSTGROUPS => NOT DONE")
    else
      broker_log:info(0, "CHECK HOSTGROUPS => DONE")
    end
    return retval
  end
}

return hostgroups
