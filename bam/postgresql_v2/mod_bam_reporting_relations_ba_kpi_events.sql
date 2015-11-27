--
-- Relations between BA events and KPI events.
--
CREATE TABLE mod_bam_reporting_relations_ba_kpi_events (
  ba_event_id int NOT NULL,
  kpi_event_id int NOT NULL,

  FOREIGN KEY (ba_event_id) REFERENCES mod_bam_reporting_ba_events (ba_event_id)
    ON DELETE CASCADE,
  FOREIGN KEY (kpi_event_id) REFERENCES mod_bam_reporting_kpi_events (kpi_event_id)
    ON DELETE CASCADE
);
