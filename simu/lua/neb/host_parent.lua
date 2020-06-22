local _host_parent = {
  parent = {},
  count = 0
}

local function build(host_id, parent_host_id)
  local retval = {
    category = 1,
    element = 13,
    _type = 65549,
    child_id = host_id,
    parent_id = parent_host_id,
  }
  return retval
end

local host_parent = {
  name = "Host parent",
  -- return: a neb::host_parent
  build = function (stack, count, conn)
    local poller_count = count.instance
    local host_count = count.host * poller_count
    broker_log:info(0, "BUILD HOST PARENT "
                       .. " ; instance_count = " .. poller_count
                       .. " ; host_count = " .. host_count)
    for j = 1,poller_count do
      for i = 1,count.host do
        local host_id = i + (j - 1) * count.host
        local instance_id = j
        if _host_parent.parent[instance_id] then
          local parent_id = _host_parent.parent[instance_id]
          table.insert(
                  stack,
                  build(host_id, parent_id))
          _host_parent.count = _host_parent.count + 1
        else
          _host_parent.parent[instance_id] = host_id
        end
      end
    end
    broker_log:info(0, "BUILD HOST PARENT => FINISHED")
  end,

  check = function (conn, count)
    local host_count = count.host * count.instance
    local finish = _host_parent.finish
    broker_log:info(0, "CHECK HOST PARENT")
    local retval = true
    broker_log:info(0, "SELECT count(*) from hosts_hosts_parents")
    local cursor, error_str = conn["storage"]:execute("SELECT count(*) from hosts_hosts_parents")
    local row = cursor:fetch({}, "a")
    if row then
      if tonumber(row['count(*)']) ~= _host_parent.count then
        broker_log:error(0, "hosts_hosts_parents should contain " .. _host_parent.count .. " rows")
        retval = false
      end
    end

    if not retval then
      broker_log:info(0, "CHECK HOST PARENT => NOT DONE")
    else
      broker_log:info(0, "CHECK HOST PARENT => DONE")
    end
    return retval
  end
}

return host_parent
