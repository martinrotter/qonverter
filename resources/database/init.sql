DROP TABLE IF EXISTS q_information;
-- !
CREATE TABLE IF NOT EXISTS q_information (
	key	TEXT	PRIMARY KEY NOT NULL,
	value	TEXT	NOT NULL
);
-- !
INSERT INTO q_information
VALUES('schema_version', '1.0.0');
-- !
DROP TABLE IF EXISTS q_history;
-- !
CREATE TABLE IF NOT EXISTS q_history (
	date	TEXT	PRIMARY KEY NOT NULL,
	formula	TEXT	NOT NULL,
	result	TEXT	NOT NULL
);
-- !
DROP TABLE IF EXISTS q_variables;
-- !
CREATE TABLE IF NOT EXISTS q_variables (
	name	TEXT	PRIMARY KEY NOT NULL,
	value	TEXT	NOT NULL,
	is_cons	INTEGER	NOT NULL CHECK(is_cons >= 0 AND is_cons <= 1)
);