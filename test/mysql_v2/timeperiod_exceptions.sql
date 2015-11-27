--
-- Configuration of timeperiod exceptions.
--
CREATE TABLE timeperiod_exceptions (
  exception_id int NOT NULL auto_increment,

  timeperiod_id int NOT NULL,
  days varchar(255) NOT NULL,
  timerange varchar(255) NOT NULL,

  PRIMARY KEY (exception_id),
  INDEX (timeperiod_id),
  FOREIGN KEY (timeperiod_id) REFERENCES timeperiod (tp_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;
