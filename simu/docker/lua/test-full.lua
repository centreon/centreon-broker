--package.path = package.path .. ";/home/admin/?.lua"
local mysql = require "luasql.mysql"

local simu = {
  log_file = "/tmp/simu.log",
  host_count = 1,
  poller_count = 1,
  conn = nil,
  stack = {},
  step = 1,
}

local step = {
  require('neb.instances'),
  require('neb.hosts'),
  require('neb.custom_variables'),
  require('neb.comments'),
  require('neb.services'),
  require('neb.hostgroups'),
  require('neb.hostgroup_members'),
  require('neb.servicegroups'),
  require('neb.service_checks'),
  require('neb.service_status'),
  require('neb.downtimes'),
  require('neb.host_checks'),
  require('neb.host_status'),
  require('neb.acknowledgements'),
  require('bam.truncate'),
  require('neb.finish'),
}

-- Instances                  => 18
step[1].count = {
  instance = 2
}

-- Hosts per instance         => 312
step[2].count = {
  host = 10,
  instance = step[1].count.instance
}

-- Custom variables per host  =>
step[3].count = {
  cv = 30,
  host = step[2].count.host,
  instance = step[1].count.instance
}

-- Comments per host
step[4].count = {
  comment = 50,
  host = step[2].count.host,
  instance = step[2].count.instance,
}

-- Services per host          => 20
step[5].count = {
  service = 50,
  host = step[2].count.host,
  instance = step[2].count.instance,
}

-- Hostgroups
step[6].count = {
  group = 10,
}

-- Hostgroups members
step[7].count = {
  host = step[2].count.host,
  instance = step[2].count.instance,
  hostgroup = 1,
}

-- Servicegroups
step[8].count = {
  servicegroup = 20,
}

-- Service checks
step[9].count = {
  service = 50,
  host = step[2].count.host,
  instance = step[2].count.instance,
}

-- Services per host          => 20
step[10].count = {
  service = 50,
  host = step[2].count.host,
  instance = step[2].count.instance,
  metric = 10,
}

-- Downtimes per host
step[11].count = {
  host = 5,
}

-- Host checks and logs per instance
step[12].count = {
  host = step[2].count.host,
  instance = step[1].count.instance
}

-- Host status
step[13].count = {
  host = step[2].count.host,
  instance = step[1].count.instance
}

-- Acknowledgements
step[14].count = {
  service = 50,
  host = step[2].count.host,
  instance = step[2].count.instance,
}

-- Table truncate signal
step[15].count = {
  update_started = true,
}

function os.capture(cmd, raw)
  local f = assert(io.popen(cmd, 'r'))
  local s = assert(f:read('*a'))
  f:close()
  if raw then return s end
  s = string.gsub(s, '^%s+', '')
  s = string.gsub(s, '%s+$', '')
  s = string.gsub(s, '[\n\r]+', ' ')
  return s
end

function init(conf)
  math.randomseed(os.time())
  os.remove("/tmp/simu.log")
  broker_log:set_parameters(0, simu.log_file)
  local env = mysql.mysql()
  simu.conn = env:connect('centreon_storage', 'root', 'root', '172.17.0.1', 3306)
  if not simu.conn then
    broker_log:error(0, "No connection to database")
    error("No connection to database")
  end

  -- Some clean up
  local cursor, error_str = simu.conn:execute("DELETE FROM data_bin;")
  cursor, error_str = simu.conn:execute("DELETE FROM metrics;")
end

function read()
  if #simu.stack == 0 then
    if not simu.started then
      print("=> " .. step[simu.step].name)
      broker_log:info(0, "Step " .. simu.step)
      if step[simu.step] then
        broker_log:info(1, "First build")
        step[simu.step].build(simu.stack, step[simu.step].count)
        simu.started = true
      end
    else
      -- Check of instance in db
      if step[simu.step].check(simu.conn, step[simu.step].count) then
        simu.step = simu.step + 1
        print("=> " .. step[simu.step].name)
        broker_log:info(0, "Step " .. simu.step)
        if step[simu.step] then
          step[simu.step].build(simu.stack, step[simu.step].count)
        end
      end
    end
  end

  if #simu.stack > 0 then
    if #simu.stack % 100 == 0 then
      broker_log:info(0, "Stack contains " .. #simu.stack .. " elements")
    end
    broker_log:info(2, broker.json_encode(simu.stack[1]))
    return table.remove(simu.stack, 1)
  end
  return nil
end
