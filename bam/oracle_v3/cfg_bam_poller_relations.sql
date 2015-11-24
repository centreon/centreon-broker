--
-- BA / poller association.
--
CREATE TABLE cfg_bam_poller_relations (
  ba_id int NOT NULL,
  poller_id int NOT NULL

  FOREIGN KEY (ba_id) REFERENCES cfg_bam (ba_id)
    ON DELETE CASCADE
);
