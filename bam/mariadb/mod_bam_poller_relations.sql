--
-- BA/poller relation table.
--
CREATE TABLE mod_bam_poller_relations (
  ba_id int NOT NULL,
  poller_id int NOT NULL,

  FOREIGN KEY (ba_id) REFERENCES mod_bam (ba_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;
