-- ------------------------------------
--                                   --
-- Centreon Broker's database schema --
--                                   --
--                                   --
--          Real-time data           --
--         Performance data          --
-- ------------------------------------

-- data_bin
-- schemaversion


--
-- Holds the current version of the database schema.
--
CREATE TABLE schemaversion (
  software varchar(128) NOT NULL,
  version int NOT NULL
) ENGINE=InnoDB;
INSERT INTO schemaversion (software, version) VALUES ('centreon-broker', 2);


--
--  Performance data.
--
CREATE TABLE data_bin (
  id_metric int NOT NULL,
  ctime int NOT NULL,
  status enum('0', '1', '2', '3', '4') NOT NULL default '3',
  value float default NULL,

  FOREIGN KEY (id_metric) REFERENCES metrics (metric_id)
    ON DELETE CASCADE,
  INDEX (id_metric)
) ENGINE=InnoDB;
