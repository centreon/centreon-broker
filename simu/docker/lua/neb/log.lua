local _logs = {}

local function build(id)
  local now = os.time()
  if not _logs.start then
    _logs.start = now
    _logs.host_id = 1
  else
    _logs.host_id = _logs.host_id + 1
  end

  local retval = {
    ctime = now,
    retry = 0,
    instance_name = "instance",
    msg_type = 5,
    output = "Output message from " .. _logs.host_id,
    notification_contact = "",
    status = 0,
    host_name = "host_" .. _logs.host_id,
    issue_start_time = _logs.host_id,
    category = 1,
    element = 17,
    _type = 65553,
    notification_cmd = "notification command " .. _logs.host_id,
  }
  return retval
end

local logs = {
  name = "Logs",
  build = function (stack, count, conn)
    local log_count = count.log
    local instance_count = count.instance
    broker_log:info(0, "BUILD LOGS ; log_count = " .. log_count)
    for j = 1,log_count do
      table.insert(stack, build(j))
    end
    broker_log:info(0, "BUILD LOGS => FINISHED")
  end,

  check = function (conn, count)
    local log_count = count.log
    broker_log:info(0, "CHECK LOGS with ctime = " .. _logs.start)
    local retval = true
    local cursor, error_str = conn["storage"]:execute("SELECT count(*) FROM logs WHERE ctime=" .. _logs.start)
    local row = cursor:fetch({}, "a")
    if tonumber(row["count(*)"]) ~= log_count then
      broker_log:info(0, "NOT FINISHED; " .. row["count(*)"] .. " rows found instead of " .. log_count)
      retval = false
    end
    if not retval then
      broker_log:info(0, "CHECK LOGS => NOT DONE")
    else
      broker_log:info(0, "CHECK LOGS => DONE")
    end
    return retval
  end
}

return logs
