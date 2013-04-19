DROP TABLE IF EXISTS q_information;
-- !
CREATE TABLE IF NOT EXISTS q_information (
    key		TEXT	PRIMARY KEY NOT NULL,
    value	TEXT	NOT NULL
);
-- !
INSERT INTO q_information
VALUES('schema_version', '1.0.1');
-- !
DROP TABLE IF EXISTS q_variables;
-- !
CREATE TABLE IF NOT EXISTS q_variables (
    name	TEXT	PRIMARY KEY NOT NULL,
    value	TEXT	NOT NULL,
	desc	TEXT
);
