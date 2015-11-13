--
-- Issues parenting.
--
CREATE TABLE issues_issues_parents (
  child_id int NOT NULL,
  end_time int default NULL,
  start_time int NOT NULL,
  parent_id int NOT NULL,

  INDEX (child_id),
  INDEX (parent_id),
  FOREIGN KEY (child_id) REFERENCES issues (issue_id)
    ON DELETE CASCADE,
  FOREIGN KEY (parent_id) REFERENCES issues (issue_id)
    ON DELETE CASCADE
);
