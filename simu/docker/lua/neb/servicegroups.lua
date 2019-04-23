local function build(id, name)
  local retval = {
    category = 1,
    element = 21,
    _type = 65557,
    servicegroup_id = id,
    name = name,
  }
  return retval
end

local servicegroups = {
  name = "Service groups",
  -- id: instance id
  -- name: instance name
  -- engine: Monitoring engine name in this instance
  -- pid: Monitoring engine pid
  --
  -- return: a neb::instance event
  build = function (stack, count, conn)
    local servicegroup_count = count.servicegroup
    broker_log:info(0, "BUILD SERVICEGROUPS ; servicegroup_count = " .. tostring(servicegroup_count))
    for i = 1,servicegroup_count do
      table.insert(
              stack,
              build(i, "servicegroup_" .. i))
    end
    broker_log:info(0, "BUILD SERVICEGROUPS => FINISHED")
  end,

  check = function (conn, count)
    local servicegroup_count = count.servicegroup
    local now = os.time()
    broker_log:info(0, "CHECK SERVICEGROUPS")
    local retval = true
    local cursor, error_str = conn["storage"]:execute([[SELECT servicegroup_id, name from servicegroups ORDER BY servicegroup_id]])
    local row = cursor:fetch({}, "a")
    local id = 1
    while row do
      local name = "servicegroup_" .. id
      broker_log:info(1, "Check for servicegroup " .. id)
      if tonumber(row.servicegroup_id) ~= id or row.name ~= name then
        broker_log:error(0, "Row found servicegroup_id = "
            .. row.servicegroup_id .. " name = " .. row.name
            .. " instead of servicegroup_id = " .. id .. " and name = " .. name)
        retval = false
        break
      end
      id = id + 1
      row = cursor:fetch({}, "a")
    end

    if id <= servicegroup_count then
      broker_log:info(0, "CHECK SERVICEGROUPS => NOT FINISHED")
      retval = false
    end
    if not retval then
      broker_log:info(0, "CHECK SERVICEGROUPS => NOT DONE")
    else
      broker_log:info(0, "CHECK SERVICEGROUPS => DONE")
    end
    return retval
  end
}

return servicegroups
