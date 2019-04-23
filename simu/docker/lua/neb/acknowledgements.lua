local ack_data = {}

local function build(instance_id, host_id, service_id)
  if not ack_data.start then
    ack_data.start = os.time()
  end
  local retval = {
    category = 1,
    element = 1,
    _type = 65537,
    acknowledgement_type = 1,
    author = "admin",
    comment = "Acknowledged",
    deletion_time = 0,
    entry_time = ack_data.start,
    host_id = host_id,
    instance_id = instance_id,
    service_id = service_id,
    state = 3,
  }
  return retval
end

local acknowledgements = {
  name = "Acknowledgements",
  -- id: instance id
  -- name: instance name
  -- engine: Monitoring engine name in this instance
  -- pid: Monitoring engine pid
  --
  -- return: a neb::instance event
  build = function (stack, count, conn)
    local instance_count = count.instance
    local host_count = count.host
    local service_count = count.service
    broker_log:info(0, "BUILD ACKNOWLEDGEMENT ; instance_count = " .. instance_count .. " ; service_count = " .. service_count .. " ; host_count = " .. host_count)
    for k = 1,instance_count do
      for j = 1,host_count do
        for i = 1,service_count do
          local host_id = j + (k - 1) * host_count
          local service_id = i + (host_id - 1) * service_count
          table.insert(
                  stack,
                  build(k, host_id, service_id))
        end
      end
    end
    broker_log:info(0, "BUILD ACKNOWLEDGEMENT => FINISHED")
  end,

  check = function (conn, count)
    broker_log:info(0, "CHECK ACKNOWLEDGEMENT")
    if not ack_data.start then
      return false
    end
--    local service_count = count.service
--    local host_count = count.host * count.instance
--    local now = os.time()
--    local retval = true
    local cursor, error_str = conn["storage"]:execute(
            "SELECT count(*) from acknowledgements WHERE entry_time="
            .. ack_data.start)
    local row = cursor:fetch({}, "a")
    local id = 1
    if tonumber(row['count(*)']) ~= 1000 then
      retval = false
    else
      retval = true
    end
    if not retval then
      broker_log:info(0, "CHECK ACKNOWLEDGEMENT => NOT DONE")
    else
      broker_log:info(0, "CHECK ACKNOWLEDGEMENT => DONE")
    end
    return retval
  end
}

return acknowledgements
