local data = {
  parent = {},
  count = 0,
}

local function build(host_id, service_id, host_dep_id, service_dep_id)
  local retval = {
    category = 1,
    element = 20,
    _type = 65556,
    dependency_period = "24/24",
    dependent_host_id = host_dep_id,
    dependent_service_id = service_dep_id,
    host_id = host_id,
    service_id = service_id,
  }
  return retval
end

local service_dependency = {
  name = "Service dependency",

  build = function (stack, count, conn)
    local host_count = count.host * count.instance
    broker_log:info(0, "BUILD SERVICE DEPENDENCY "
                       .. " ; instance_count = " .. count.instance
                       .. " ; host_count = " .. host_count)
    for k = 1,count.instance do
      for j = 1,count.host do
        for i = 1,count.service do
          local host_id = (k - 1) * count.host + j
          local service_id = i + (j - 1) * count.service
          if data.parent[k] then
            local host_dep_id = data.parent[k].host_dep_id
            local service_dep_id = data.parent[k].service_dep_id
            table.insert(
                    stack,
                    build(host_id, service_id, host_dep_id, service_dep_id))
            data.count = data.count + 1
          else
            data.parent[k] = {
              host_dep_id = host_id,
              service_dep_id = service_id
            }
          end
        end
      end
    end
    broker_log:info(0, "BUILD SERVICE DEPENDENCY => FINISHED")
  end,

  check = function (conn, count)
    local host_count = count.host * count.instance
    local finish = data.finish
    broker_log:info(0, "CHECK SERVICE DEPENDENCY")
    local retval = true
    broker_log:info(0, "SELECT count(*) from services_services_dependencies")
    local cursor, error_str = conn["storage"]:execute("SELECT count(*) from services_services_dependencies")
    local row = cursor:fetch({}, "a")
    if row then
      if tonumber(row['count(*)']) ~= data.count then
        broker_log:error(0, "services_services_dependency should contain " .. data.count .. " rows")
        retval = false
      end
    end

    if not retval then
      broker_log:info(0, "CHECK SERVICE DEPENDENCY => NOT DONE")
    else
      broker_log:info(0, "CHECK SERVICE DEPENDENCY => DONE")
    end
    return retval
  end
}

return service_dependency
