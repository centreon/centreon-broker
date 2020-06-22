 local function build(id, instance_id, name, address)
  local now = os.time()
  local retval = {
    _type = 393227,
    category = 6,
    element = 11,
    update_started = true,
  }
  return retval
end

local truncate = {
  name = "Dimension truncate table signal",
  --
  -- return: a bam::dimension_truncate_table_signal event
  build = function (stack, count)
    broker_log:info(0, "BUILD TRUNCATE TABLE SIGNAL ; update started = "
                       .. tostring(count.update_started))
    table.insert(
            stack,
            build(count.update_started))
    broker_log:info(0, "BUILD TRUNCATE TABLE SIGNAL => FINISHED")
  end,

  check = function (conn, count)
    -- The Broker cache containing ba should now be empty
    return true
  end
}

return truncate
