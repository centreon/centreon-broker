--
-- Timeperiods exclusions.
--
CREATE TABLE mod_bam_reporting_timeperiods_exclusions (
  timeperiod_id int NOT NULL,
  excluded_timeperiod_id int NOT NULL,

  FOREIGN KEY (timeperiod_id) REFERENCES mod_bam_reporting_timeperiods (timeperiod_id)
    ON DELETE CASCADE,
  FOREIGN KEY (excluded_timeperiod_id) REFERENCES mod_bam_reporting_timeperiods (timeperiod_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;
