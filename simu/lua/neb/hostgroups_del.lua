local function build(id, name)
  local retval = {
    category = 1,
    element = 10,
    _type = 65546,
    enabled = false,
    hostgroup_id = id,
    name = name,
  }
  return retval
end

local hostgroups_del = {
  name = "Host groups deletion",
  -- id: instance id
  -- name: instance name
  -- engine: Monitoring engine name in this instance
  -- pid: Monitoring engine pid
  --
  -- return: a neb::instance event
  build = function (stack, count, conn)
    local hostgroup_count = count.group
    broker_log:info(0, "BUILD HOSTGROUPS DELETION; hostgroup_count = " .. tostring(hostgroup_count))
    for i = 1,hostgroup_count do
      table.insert(
              stack,
              build(i, "hostgroup_" .. i))
    end
    broker_log:info(0, "BUILD HOSTGROUPS DELETION => FINISHED")
  end,

  check = function (conn, count)
    local hostgroup_count = count.group
    local now = os.time()
    broker_log:info(0, "CHECK HOSTGROUPS")
    local retval = true
    local cursor, error_str = conn["storage"]:execute([[SELECT count(hostgroup_id) from hostgroups]])
    local row = cursor:fetch({}, "a")
    while row do
      broker_log:info(0, "CHECK HOSTGROUPS DELETION => NOT FINISHED")
      cursor, error_str = conn["storage"]:execute([[SELECT count(hostgroup_id) from hostgroups]])
      row = cursor:fetch({}, "a")
    end

    broker_log:info(0, "CHECK HOSTGROUPS => DONE")
    return retval
  end
}

return hostgroups_del
