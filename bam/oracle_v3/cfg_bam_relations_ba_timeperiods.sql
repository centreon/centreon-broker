--
-- BA / Timeperiod relations.
--
CREATE TABLE cfg_bam_relations_ba_timeperiods (
  ba_id int NOT NULL,
  tp_id int NOT NULL,

  UNIQUE (ba_id, tp_id),
  FOREIGN KEY (ba_id) REFERENCES cfg_bam (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (tp_id) REFERENCES timeperiod (tp_id)
    ON DELETE CASCADE
);
