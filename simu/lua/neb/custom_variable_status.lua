local function build(name, value, host_id)
  local now = os.time()
  local retval = {
    category = 1,
    element = 4,
    _type = 65540,
    host_id = host_id,
    modified = true,
    name = name,
    update_time = 0,
    value = value,
  }
  return retval
end

local custom_variable_status = {
  name = "Custom variable status",
  build = function (stack, count, conn)
    local cv_count = count.cv
    local host_count = count.host * count.instance
    broker_log:info(0, "BUILD CUSTOM VARIABLE STATUS ; cv_count = " .. tostring(cv_count) .. " host_count = " .. tostring(host_count))
    for j = 1,host_count do
      for i = 1,cv_count do
        table.insert(
                stack,
                build("cv_" .. i .. "_" .. j, "v" .. i .. "_" .. j .. "new", j))
      end
    end
    broker_log:info(0, "BUILD CUSTOM VARIABLES STATUS => FINISHED")
  end,

  check = function (conn, count)
    local cv_count = count.cv
    local host_count = count.host * count.instance
    local now = os.time()
    broker_log:info(0, "CHECK CUSTOM VARIABLES STATUS")
    local retval = true
    local cursor, error_str = conn["storage"]:execute([[SELECT host_id, name, value from customvariables WHERE type=1 ORDER BY host_id]])
    local row = cursor:fetch({}, "a")
    local count = 0
    while row do
      local host_id = tonumber(row.host_id)
      count = count + 1
      local name = "cv_.*_" .. host_id
      local value = "v.*_" .. host_id .. "new"
      if tonumber(row.host_id) ~= host_id or not string.match(row.name, name) or not string.match(row.value, value) then
        broker_log:error(0, "Row found host_id = "
            .. row.host_id .. " name = " .. row.name
            .. " value = " .. tostring(row.value)
            .. " does not match host_id = " .. tostring(host_id) .. " and name = " .. tostring(name) .. " and value = " .. tostring(value))
        retval = false
        break
      end

      row = cursor:fetch({}, "a")
    end

    broker_log:info(1, "custom variables count = " .. count)
    if count ~= cv_count * host_count then
      broker_log:info(0, "CHECK CUSTOM VARIABLES => NOT FINISHED")
      retval = false
    end
    if not retval then
      broker_log:info(0, "CHECK CUSTOM VARIABLES => NOT DONE")
    else
      broker_log:info(0, "CHECK CUSTOM VARIABLES => DONE")
    end
    return retval
  end
}

return custom_variable_status
