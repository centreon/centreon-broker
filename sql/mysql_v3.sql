-- ------------------------------------
--                                   --
-- Centreon Broker's database schema --
--                                   --
--                                   --
--          Real-time data           --
--         Performance data          --
-- ------------------------------------

-- rt_schemaversion


--
-- Holds the current version of the database schema.
--
CREATE TABLE rt_schemaversion (
  software varchar(128) NOT NULL,
  version int NOT NULL
) ENGINE=InnoDB;
INSERT INTO rt_schemaversion (software, version) VALUES ('centreon-broker', 3);
