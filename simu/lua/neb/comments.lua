local _comments = {}

local function build(internal_id, comment_id, host_id, instance_id)
  local now = os.time() - comment_id
  local retval = {
    element = 2,
    category = 1,
    _type = 65538,
    author = "admin",
    comment_type = 1,
    data = "Message from admin " .. comment_id .. ":" .. host_id .. ":" .. instance_id,
    deletion_time = 0,
    entry_time = now,
    entry_type = 1,
    expire_time = 0,
    expires = false,
    host_id = host_id,
    instance_id = instance_id,
    internal_id = internal_id,
    persistent = false,
    service_id = 0,
    source = 1,
  }
  return retval
end

local comments = {
  name = "Comments",
  build = function (stack, count, conn)
    local host_count = count.host
    local instance_count = count.instance
    broker_log:info(0, "BUILD COMMENTS ; host_count = " .. host_count .. " ; instance_count = " .. count.instance)
    local id = 1
    if not _comments.internal_id then
      math.randomseed(os.time())
      _comments.internal_id = math.random(0, 10000)
    else
      _comments.internal_id = _comments.internal_id + 1
    end
    for j = 1,instance_count do
      for i = 1,host_count do
        for k = 1,count.comment do
          table.insert(
                  stack,
                  build(_comments.internal_id, id, i + host_count * (j - 1), j))
          id = id + 1
        end
      end
    end
    broker_log:info(0, "BUILD COMMENTS => FINISHED")
  end,

  check = function (conn, count)
    local host_count = count.host
    local instance_count = count.instance
    local now = os.time()
    broker_log:info(0, "CHECK COMMENTS with internal_id = " .. _comments.internal_id)
    local retval = true
    local cursor, error_str = conn["storage"]:execute("SELECT count(*) from comments where internal_id=" .. _comments.internal_id)
    local row = cursor:fetch({}, "a")

    if tonumber(row['count(*)']) ~= host_count * instance_count * count.comment then
      retval = false
    end
    if not retval then
      broker_log:info(0, "CHECK COMMENTS => NOT DONE")
    else
      broker_log:info(0, "CHECK COMMENTS => DONE")
    end
    return retval
  end
}

return comments
