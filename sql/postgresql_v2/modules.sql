--
-- Centreon Engine modules.
--
CREATE TABLE modules (
  module_id serial,
  instance_id int NOT NULL,

  args varchar(255) default NULL,
  filename varchar(255) default NULL,
  loaded boolean default NULL,
  should_be_loaded boolean default NULL,

  PRIMARY KEY (module_id),
  INDEX (instance_id),
  FOREIGN KEY (instance_id) REFERENCES instances (instance_id)
    ON DELETE CASCADE
);
