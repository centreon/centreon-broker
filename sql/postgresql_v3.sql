-- ------------------------------------
--                                   --
-- Centreon Broker's database schema --
--                                   --
--                                   --
--          Real-time data           --
--         Performance data          --
-- ------------------------------------

-- log_data_bin
-- rt_metrics
-- rt_schemaversion


--
-- Holds the current version of the database schema.
--
CREATE TABLE rt_schemaversion (
  software varchar(128) NOT NULL,
  version int NOT NULL
);
INSERT INTO rt_schemaversion (software, version) VALUES ('centreon-broker', 3);


--
--  Metrics.
--
CREATE TABLE rt_metrics (
  metric_id serial,
  index_id int NOT NULL,
  metric_name varchar(255) NOT NULL,

  crit double default NULL,
  crit_low double default NULL,
  crit_threshold_mode boolean default NULL,
  current_value double default NULL,
  data_source_type smallint NOT NULL default 0,
  hidden boolean NOT NULL default 0,
  locked boolean NOT NULL default 0,
  min double default NULL,
  max double default NULL,
  to_delete boolean NOT NULL default 0,
  unit_name varchar(32) default NULL,
  warn double default NULL,
  warn_low double default NULL,
  warn_threshold_mode boolean default NULL,

  PRIMARY KEY (metric_id),
  UNIQUE KEY (index_id, metric_name),
  FOREIGN KEY (index_id) REFERENCES rt_index_data (index_id)
    ON DELETE CASCADE,
  INDEX (index_id)
);

--
--  Performance data.
--
CREATE TABLE log_data_bin (
  metric_id int NOT NULL,
  ctime int NOT NULL,
  status smallint NOT NULL default 3,
  value float default NULL,

  FOREIGN KEY (metric_id) REFERENCES rt_metrics (metric_id)
    ON DELETE CASCADE,
  INDEX (metric_id)
);
