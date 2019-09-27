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
    local cursor, error_str = conn["storage"]:execute([[SELECT count(*) from hostgroups where hostgroup_id <= 10]])
    local row = cursor:fetch({}, "a")
    broker_log:info(1, "Check for hostgroup")
    if tonumber(row["count(*)"]) ~= 10 then
      broker_log:error(0, "We should have 10 rows in hostgroups whose id is between 1 and 10. We have "
          .. row["count(*)"]
          .. " of them")
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
