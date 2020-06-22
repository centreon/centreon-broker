local finish = {
  name = "Finish",
  check = function(conn)
    broker_log:info(0, "No more step")
    local output = os.capture("ps ax | grep \"\\<cbd\\>\" | grep -v grep | awk '{print $1}' ", 1)
    print("################################")
    print(output)
    print("################################")
    if output ~= "" then
      broker_log:info(0, "SEND COMMAND: kill " .. output)
      os.execute("kill -9 " .. output)
    end
    return true
  end,

  build = function ()
    broker_log:info(0, "Time to finish...")
  end
}

return finish
