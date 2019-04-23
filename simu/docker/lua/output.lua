function init(conf)
  os.remove("/tmp/output.log")
  broker_log:set_parameters(3, "/tmp/output.log")
end

function write(d)
  broker_log:info(0, broker.json_encode(d))
  return true
end
