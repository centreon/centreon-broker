local _ba_event = {}

local function build(ba_id)
  local now = os.time()
  local retval = {
    ba_id = ba_id,
    _type = 393220,
    first_level = 1,
    end_time = now,
    in_downtime = 0,
    start_time = 30,
    status = 1
  }
  return retval
end

local ba_event = {
  name = "BA Event",
  build = function (stack, count, conn)
    local ba_count = count.ba
    broker_log:info(0, "BUILD BA EVENT ; ba = " .. ba_count)

    for i = 1,ba_count do
      table.insert(stack, build(i))
    end
    broker_log:info(0, "BUILD BA EVENT => FINISHED")
  end,

  check = function (conn, count)
    local ba_count = count.ba
    broker_log:info(0, "CHECK BA EVENT")
    local retval = true
    local cursor, error_str = conn["storage"]:execute("SELECT count(*) from mod_bam_reporting_ba_events" )
    local row = cursor:fetch({}, "a")

    if tonumber(row['count(*)']) ~= ba_count then
      retval = false
    end
    if not retval then
      broker_log:info(0, "CHECK BA EVENT => NOT DONE")
    else
      broker_log:info(0, "CHECK BA EVENT => DONE")
    end
    return retval
  end
}

return ba_event
