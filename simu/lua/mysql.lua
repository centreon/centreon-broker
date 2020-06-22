mysql = require 'luasql.mysql'

local env  = mysql.mysql()
local conn = env:connect('centreon_storage','root','root', "172.17.0.1", 3306)

if not conn then
  print("No connection")
  os.exit(1)
end

cursor, errorString = conn:execute([[select instance_id, name from instances]])
row = cursor:fetch({}, "a")
while row do
  for k,v in pairs(row) do
    print(k.." => " .. v)
    row = cursor:fetch(row, "a")
  end
end
