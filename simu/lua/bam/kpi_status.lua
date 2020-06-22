 local _kpi_status = {}

local function build(kpi_id)
  local now = os.time()
  local retval = {
    kpi_id = kpi_id,
    _type = 393218,
    in_downtime = 1,
    level_acknowledgement_hard = 1,
    level_acknowledgement_soft = 1,
    level_downtime_hard = 1,
    level_downtime_soft = 1,
    level_nominal_hard = 1,
    level_nominal_soft = 1,
    state_hard = 1,
    state_soft = 1,
    last_state_change = 0,
    last_impact = 1,
    valid = 1,
  }
  return retval
end

local kpi_status = {
  name = "KPI Status",
  build = function (stack, count, conn)
    local kpi_count = count.kpi
    broker_log:info(0, "BUILD KPI STATUS ; kpi = " .. kpi_count)

    for i = 1,kpi_count do
      local squery = "INSERT INTO mod_bam_kpi (kpi_id, state_type, kpi_type, valid) VALUES (%i,'0','0',1)"
      local cursor, err = conn["cfg"]:execute(squery:format(i))
      if err then
        print(err)
        error(err)
      end
    end

    for i = 1,kpi_count do
      table.insert(stack, build(i))
    end
    broker_log:info(0, "BUILD KPI STATUS => FINISHED")
  end,

  check = function (conn, count)
    local kpi_count = count.kpi
    broker_log:info(0, "CHECK KPI STATUS")
    local retval = true
    local cursor, error_str = conn["cfg"]:execute("SELECT count(*) from mod_bam_kpi WHERE last_impact = 1" )
    local row = cursor:fetch({}, "a")

    if tonumber(row['count(*)']) ~= kpi_count then
      retval = false
    end
    if not retval then
      broker_log:info(0, "CHECK KPI STATUS => NOT DONE")
    else
      broker_log:info(0, "CHECK KPI STATUS => DONE")
    end
    return retval
  end
}

return kpi_status
