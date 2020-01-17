-- vim:ts=4:sw=4:sts=4:et:ai

-- for passord, se ~/mqtt.pw

-- MySQL/MariaDB syntax. Rewritten to PostgreSQL below.
--
-- CREATE TABLE tbl_messages (
--   messageID INT NOT NULL AUTO_INCREMENT,
--   clientID VARCHAR(20) NOT NULL,
--   topic VARCHAR(50) NOT NULL,
--   message VARCHAR(100) NOT NULL,
--   Enable BOOLEAN DEFAULT 1,
--   DateTime_created TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
--   PRIMARY KEY (messageID)
-- );


CREATE TABLE mqtt_messages (
    id SERIAL PRIMARY KEY,
    clientid TEXT NOT NULL,
    topic TEXT NOT NULL,
    message TEXT NOT NULL,
    is_enabled INTEGER DEFAULT 1,
    time TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT CURRENT_TIMESTAMP
);
