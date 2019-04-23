local _kpi_event = {}

local function build(kpi_id)
  local now = os.time()
  local retval = {
    kpi_id = kpi_id,
    _type = 393221,
    end_time = 0,
    impact_level = 1,
    in_downtime = 2,
    first_output = 1,
    first_perfdata = 0,
    start_time = 30,
    status = 1,
  }
  return retval
end

local kpi_event = {
  name = "KPI Event",
  build = function (stack, count, conn)
    local kpi_count = count.kpi
    broker_log:info(0, "BUILD KPI EVENT ; kpi = " .. kpi_count)

    for i = 1,kpi_count do
      table.insert(stack, build(i))
    end
    broker_log:info(0, "BUILD KPI EVENT => FINISHED")
  end,

  check = function (conn, count)
    local kpi_count = count.kpi
    broker_log:info(0, "CHECK KPI EVENT")
    local retval = true
    local cursor, error_str = conn["storage"]:execute("SELECT count(*) from mod_bam_reporting_kpi_events")
    local row = cursor:fetch({}, "a")

    if tonumber(row['count(*)']) ~= kpi_count then
      retval = false
    end
    if not retval then
      broker_log:info(0, "CHECK KPI EVENT => NOT DONE")
    else
      broker_log:info(0, "CHECK KPI EVENT => DONE")
    end
    return retval
  end
}

return kpi_event
