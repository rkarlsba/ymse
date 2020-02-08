-- CREATE TABLE rfid_tags(name varchar(20) NOT NULL, id BIGINT, cardtype varchar(255) NOT NULL, kb integer);
-- CREATE UNIQUE INDEX rfid_tags_name_idx ON rfid_tags(name);
-- CREATE UNIQUE INDEX rfid_tags_id_idx ON rfid_tags(id);

-- INSERT INTO rfid_tags(name,id,cardtype,kb) VALUES ('B1',0xc9c31087,'MIFARE Classic',1);
-- INSERT INTO rfid_tags(name,id,cardtype,kb) VALUES ('B2',0x27493234,'MIFARE Classic',1);
-- INSERT INTO rfid_tags(name,id,cardtype,kb) VALUES ('B3',0x4b892d1b,'MIFARE Classic',1);
-- INSERT INTO rfid_tags(name,id,cardtype,kb) VALUES ('K1',0x6954c083,'MIFARE Classic',1);
-- INSERT INTO rfid_tags(name,id,cardtype,kb) VALUES ('K2',0xfa760311,'MIFARE Classic',1);
-- INSERT INTO rfid_tags(name,id,cardtype,kb) VALUES ('K3',0x1ad90811,'MIFARE Classic',1);
-- INSERT INTO rfid_tags(name,id,cardtype,kb) VALUES ('Jobb',0x4d84d411,'MIFARE Classic',4);

-- INSERT INTO rfid_tags(name,id,cardtype) VALUES ('L1',0x043de8329b6681,'MIFARE Ultralight or Ultralight C');
-- INSERT INTO rfid_tags(name,id,cardtype) VALUES ('L2',0x0445e8329b6681,'MIFARE Ultralight or Ultralight C');
-- INSERT INTO rfid_tags(name,id,cardtype) VALUES ('L3',0x0462e9329b6680,'MIFARE Ultralight or Ultralight C');
-- INSERT INTO rfid_tags(name,id,cardtype) VALUES ('L4',0x042ce8329b6681,'MIFARE Ultralight or Ultralight C');
-- INSERT INTO rfid_tags(name,id,cardtype) VALUES ('L5',0x0430e8329b6681,'MIFARE Ultralight or Ultralight C');

INSERT INTO rfid_tags(name,id,cardtype,kb) VALUES ('KK2',0xFE146B66,'MIFARE Classic',1);
INSERT INTO rfid_tags(name,id,cardtype,kb) VALUES ('KK3',0xDED96F66,'MIFARE Classic',1);
INSERT INTO rfid_tags(name,id,cardtype,kb) VALUES ('KK4',0x5E348466,'MIFARE Classic',1);
INSERT INTO rfid_tags(name,id,cardtype,kb) VALUES ('KK5',0xCE368466,'MIFARE Classic',1);
INSERT INTO rfid_tags(name,id,cardtype,kb) VALUES ('KK6',0x8EEC8366,'MIFARE Classic',1);
INSERT INTO rfid_tags(name,id,cardtype,kb) VALUES ('KK7',0x5E889666,'MIFARE Classic',1);
INSERT INTO rfid_tags(name,id,cardtype,kb) VALUES ('KK8',0x2EFC8766,'MIFARE Classic',1);
