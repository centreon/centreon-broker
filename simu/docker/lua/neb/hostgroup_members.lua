local function build(group_id, host_id, instance_id)
  local retval = {
    category = 1,
    element = 11,
    _type = 65547,
    host_id = host_id,
    hostgroup_id = group_id,
    instance_id = instance_id,
  }
  return retval
end

local hostgroup_members = {
  name = "Host groups members",
  -- id: instance id
  -- name: instance name
  -- engine: Monitoring engine name in this instance
  -- pid: Monitoring engine pid
  --
  -- return: a neb::instance event
  build = function (stack, count, conn)
    local host_count = count.host * count.instance
    local hostgroup_count = count.hostgroup
    local instance_id = 1
    broker_log:info(0, "BUILD HOST GROUPS MEMBERS " .. host_count .. " ; " .. hostgroup_count)
    local hg = 1
    for j = 1,host_count do
      table.insert(
              stack,
              build(hg, j, instance_id))
      if j % count.host == 0 then
        instance_id = instance_id + 1
      end
      hg = hg + 1
      if hg > hostgroup_count then
        hg = 1
      end
    end
    broker_log:info(0, "BUILD HOST GROUPS MEMBERS => FINISHED")
  end,

  check = function (conn, count)
    local host_count = count.host * count.instance
    local hostgroup_count = count.hostgroup
    broker_log:info(0, "CHECK HOST GROUPS MEMBERS")
    local retval = true
    local cursor, error_str = conn["storage"]:execute([[SELECT hostgroup_id, host_id from hosts_hostgroups ORDER BY host_id, hostgroup_id]])
    local row = cursor:fetch({}, "a")
    local host_id = 1
    local group_id = 1
    local cnt = 0
    while row do
      cnt = cnt + 1
      if tonumber(row.hostgroup_id) ~= group_id or tonumber(row.host_id) ~= host_id then
        broker_log:error(0, "Row found hostgroup_id = "
            .. row.hostgroup_id .. " host_id = " .. row.host_id
            .. " instead of hostgroup_id = " .. group_id .. " and host_id = " .. host_id)
        retval = false
        break
      end
      host_id = host_id + 1
      if host_id > host_count then
        host_id = 1
      end
      group_id = group_id + 1
      if group_id > hostgroup_count then
        group_id = 1
      end
      row = cursor:fetch({}, "a")
      --os.sleep(1)
    end

    if cnt < hostgroup_count then
      broker_log:info(0, "CHECK HOSTGROUPS MEMBERS => NOT FINISHED")
      retval = false
    end
    if not retval then
      broker_log:info(0, "CHECK HOSTGROUPS MEMBERS => NOT DONE")
    else
      broker_log:info(0, "CHECK HOSTGROUPS MEMBERS => DONE")
    end
    return retval
  end
}

return hostgroup_members
