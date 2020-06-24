local _host_dependency_removed = {
  parent = {},
}

local function build(host_id, dep_host_id)
  local retval = {
    category = 1,
    element = 9,
    _type = 65545,
    dependency_period = "24/24",
    dependent_host_id = dep_host_id,
    enabled = 0,
    host_id = host_id,
    inherits_parent = true,
    notification_failure_options = "notif test",
  }
  return retval
end

local host_dependency_removed = {
  name = "Host dependency removed",
  -- return: a neb::host_dependency
  build = function (stack, count, conn)
    local poller_count = count.instance
    local host_count = count.host * poller_count
    broker_log:info(0, "BUILD HOST DEPENDENCY REMOVED"
                       .. " ; instance_count = " .. poller_count
                       .. " ; host_count = " .. host_count)
    for j = 1,poller_count do
      for i = 1,count.host do
        local host_id = i + (j - 1) * count.host
        local instance_id = j
        if _host_dependency_removed.parent[instance_id] then
          local parent_id = _host_dependency_removed.parent[instance_id]
          table.insert(
                  stack,
                  build(host_id, parent_id))
        else
          _host_dependency_removed.parent[instance_id] = host_id
        end
      end
    end
    broker_log:info(0, "BUILD HOST DEPENDENCY REMOVED => FINISHED")
    _host_dependency_removed.done = true
  end,

  check = function (conn, count)
    local retval = true;
    if not _host_dependency_removed.done then
      retval = false
    else
      broker_log:info(0, "CHECK HOST DEPENDENCY REMOVED")
      broker_log:info(0, "SELECT count(*) from hosts_hosts_dependencies")
      local cursor, error_str = conn["storage"]:execute("SELECT count(*) from hosts_hosts_dependencies")
      local row = cursor:fetch({}, "a")
      if row then
        if tonumber(row['count(*)']) ~= 0 then
          broker_log:error(0, "hosts_hosts_dependencies should contain 0 rows but contains " .. row['count(*)'])
          retval = false
        end
      end
    end

    if not retval then
      broker_log:info(0, "CHECK HOST DEPENDENCY REMOVED => NOT DONE")
    else
      broker_log:info(0, "CHECK HOST DEPENDENCY REMOVED => DONE")
    end
    return retval
  end,
}

return host_dependency_removed
