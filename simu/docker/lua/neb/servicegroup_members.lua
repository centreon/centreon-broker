local function build(group_id, host_id, service_id, instance_id)
  local retval = {
    category = 1,
    element = 22,
    _type = 65558,
    host_id = host_id,
    service_id = service_id,
    servicegroup_id = group_id,
    instance_id = instance_id,
  }
  return retval
end

local servicegroup_members = {
  name = "Service groups members",
  -- id: instance id
  -- name: instance name
  -- engine: Monitoring engine name in this instance
  -- pid: Monitoring engine pid
  --
  -- return: a neb::instance event
  build = function (stack, count, conn)
    local host_count = count.host * count.instance
    local service_count = count.service * host_count
    local servicegroup_count = count.servicegroup
    local instance_id = 1
    local host_id = 1
    broker_log:info(0, "BUILD SERVICE GROUPS MEMBERS " .. service_count .. " ; " .. servicegroup_count)
    local sg = 1
    for service_id = 1,service_count do
      table.insert(
              stack,
              build(sg, host_id, service_id, instance_id))
      if service_id % count.service == 0 then
        host_id = host_id + 1
      end
      if host_id % count.host == 0 then
        instance_id = instance_id + 1
      end
      if instance_id > count.instance then
        instance_id = 1
      end
      sg = sg + 1
      if sg > servicegroup_count then
        sg = 1
      end
    end
    broker_log:info(0, "BUILD SERVICE GROUPS MEMBERS => FINISHED")
  end,

  check = function (conn, count)
    local host_count = count.host * count.instance
    local service_count = count.service * host_count
    local servicegroup_count = count.servicegroup
    broker_log:info(0, "CHECK SERVICE GROUPS MEMBERS")
    local retval = true
    local cursor, error_str = conn["storage"]:execute([[SELECT servicegroup_id, host_id,service_id from services_servicegroups ORDER BY host_id, service_id, servicegroup_id]])
    if error_str then
      print(error_str)
      error(error_str)
    end
    local row = cursor:fetch({}, "a")
    local host_id = 1
    local service_id = 1
    local group_id = 1
    local cnt = 0
    while row do
      cnt = cnt + 1
      if tonumber(row.servicegroup_id) ~= group_id or tonumber(row.host_id) ~= host_id or tonumber(row.service_id) ~= service_id then
        broker_log:error(0,
            "Row found servicegroup_id = " .. row.servicegroup_id
            .. " host_id = " .. row.host_id
            .. " service_id = " .. row.service_id
            .. " instead of servicegroup_id = " .. group_id
            .. " host_id = " .. host_id
            .. " and service_id = " .. service_id)
        retval = false
        break
      end
      if service_id % count.service == 0 then
        host_id = host_id + 1
      end
      service_id = service_id + 1
      group_id = group_id + 1
      if group_id > servicegroup_count then
        group_id = 1
      end
      row = cursor:fetch({}, "a")
      --os.sleep(1)
    end

    if cnt < servicegroup_count then
      broker_log:info(0, "CHECK SERVICEGROUPS MEMBERS => NOT FINISHED")
      retval = false
    end
    if not retval then
      broker_log:info(0, "CHECK SERVICEGROUPS MEMBERS => NOT DONE")
    else
      broker_log:info(0, "CHECK SERVICEGROUPS MEMBERS => DONE")
    end
    return retval
  end
}

return servicegroup_members
