local _host_dependency = {
  parent = {},
  count = 0
}

local function build(host_id, dep_host_id)
  local retval = {
    category = 1,
    element = 9,
    _type = 65545,
    dependency_period = "24/24",
    dependent_host_id = dep_host_id,
    enabled = true,
    host_id = host_id,
    inherits_parent = true,
    notification_failure_options = "notif test",
  }
  return retval
end

local host_dependency = {
  name = "Host dependency",
  -- return: a neb::host_dependency
  build = function (stack, count, conn)
    local poller_count = count.instance
    local host_count = count.host * poller_count
    broker_log:info(0, "BUILD HOST DEPENDENCY "
                       .. " ; instance_count = " .. poller_count
                       .. " ; host_count = " .. host_count)
    for j = 1,poller_count do
      for i = 1,count.host do
        local host_id = i + (j - 1) * count.host
        local instance_id = j
        if _host_dependency.parent[instance_id] then
          local parent_id = _host_dependency.parent[instance_id]
          table.insert(
                  stack,
                  build(host_id, parent_id))
          _host_dependency.count = _host_dependency.count + 1
        else
          _host_dependency.parent[instance_id] = host_id
        end
      end
    end
    broker_log:info(0, "BUILD HOST DEPENDENCY => FINISHED")
  end,

  check = function (conn, count)
    local host_count = count.host * count.instance
    local finish = _host_dependency.finish
    broker_log:info(0, "CHECK HOST DEPENDENCY")
    local retval = true
    broker_log:info(0, "SELECT count(*) from hosts_hosts_dependencies")
    local cursor, error_str = conn["storage"]:execute("SELECT count(*) from hosts_hosts_dependencies")
    local row = cursor:fetch({}, "a")
    if row then
      if tonumber(row['count(*)']) ~= _host_dependency.count then
        broker_log:error(0, "hosts_hosts_dependency should contain " .. _host_dependency.count .. " rows")
        retval = false
      end
    end

    if not retval then
      broker_log:info(0, "CHECK HOST DEPENDENCY => NOT DONE")
    else
      broker_log:info(0, "CHECK HOST DEPENDENCY => DONE")
    end
    return retval
  end
}

return host_dependency
