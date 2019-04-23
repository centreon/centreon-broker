local _ba_duration_event = {}

local function build(ba_id)
  local now = os.time()
  local retval = {
    ba_id = ba_id,
    _type = 393222,
    real_start_time = 0,
    end_time = now + 1,
    start_time = now,
    duration = 1,
    sla_duration = 1,
    timeperiod_id = 1,
    timeperiod_is_default = 1,
  }
  return retval
end

local ba_duration_event = {
  name = "BA Duration Event",
  build = function (stack, count, conn)
    local ba_duration_count = count.ba_duration
    broker_log:info(0, "BUILD BA DURATION EVENT ; ba_duration = " .. ba_duration_count)

    for i = 1,ba_duration_count do
      table.insert(stack, build(i))
    end
    broker_log:info(0, "BUILD BA DURATION EVENT => FINISHED")
  end,

  check = function (conn, count)
    local ba_duration_count = count.ba_duration
    broker_log:info(0, "CHECK BA DURATION EVENT")
    local retval = true
    local cursor, error_str = conn["storage"]:execute("SELECT count(*) from mod_bam_reporting_ba_events_durations" )
    local row = cursor:fetch({}, "a")

    if tonumber(row['count(*)']) ~= ba_duration_count then
      retval = false
    end
    if not retval then
      broker_log:info(0, "CHECK BA DURATION EVENT => NOT DONE")
    else
      broker_log:info(0, "CHECK BA DURATION EVENT => DONE")
    end
    return retval
  end
}

return ba_duration_event

