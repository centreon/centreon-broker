local finish = {
  check = function(conn)
    broker_log:info(0, "No more step")
    local output = os.capture("ps ax | grep \"sbin.cbd\" | grep -v grep | awk '{print $1}' ", 1)
    if output ~= "" then
      broker_log:info(0, "SEND COMMAND: kill " .. output)
      os.execute("kill " .. output)
    end
    return true
  end,

  build = function ()
    broker_log:info(0, "Time to finish...")
  end
}

return finish
