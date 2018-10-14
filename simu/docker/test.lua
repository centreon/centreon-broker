--package.path = package.path .. ";/home/admin/?.lua"

local simu = {
  input_file = "/usr/src/centreon-broker/simu/test/export-neb.log",
}

function init(conf)
  broker_log:set_parameters(3, "/tmp/simu.log")

  -- We are waiting for a string here
  if conf.input_file then
    simu.input_file = conf.input_file
  end
  simu.f = io.open(simu.input_file, "r")
end

function read()
  local line = simu.f:read()
  if line == nil then
    return nil
  end
  local retval = broker.json_decode(line)
  broker_log:info(1, "send event: " .. line)
  return retval
end
