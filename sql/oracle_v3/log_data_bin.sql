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
