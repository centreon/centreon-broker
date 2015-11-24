--
-- BA/timeperiods relations.
--
CREATE TABLE mod_bam_reporting_relations_ba_timeperiods (
  ba_id int default NULL,
  timeperiod_id int default NULL,
  is_default boolean default NULL,

  UNIQUE (ba_id, timeperiod_id),
  FOREIGN KEY (ba_id) REFERENCES mod_bam_reporting_ba (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (timeperiod_id) REFERENCES mod_bam_reporting_timeperiods (timeperiod_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;
