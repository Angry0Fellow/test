CREATE TABLE IF NOT EXISTS objects (
    id INT AUTO_INCREMENT PRIMARY KEY,
    serial VARCHAR(100) NOT NULL,
    name VARCHAR(255) NOT NULL
);

CREATE TABLE IF NOT EXISTS relationships (
    parent_id INT,
    child_id INT,
    PRIMARY KEY (parent_id, child_id),
    FOREIGN KEY (parent_id) REFERENCES objects(id) ON DELETE CASCADE,
    FOREIGN KEY (child_id) REFERENCES objects(id) ON DELETE CASCADE
);

-- Add root object
INSERT INTO objects (serial, name) VALUES ('ROOT001', 'Root Item');

-- Insert new objects
INSERT INTO objects (serial, name) VALUES 
    ('ITEM002', 'Item A'),
    ('ITEM003', 'Item B'),
    ('ITEM004', 'Item C'),
    ('ITEM005', 'Item D');

-- Establish parent-child relationships
INSERT INTO relationships (parent_id, child_id) VALUES 
    ((SELECT id FROM objects WHERE serial = 'ROOT001'), (SELECT id FROM objects WHERE serial = 'ITEM002')),
    ((SELECT id FROM objects WHERE serial = 'ROOT001'), (SELECT id FROM objects WHERE serial = 'ITEM003')),
    ((SELECT id FROM objects WHERE serial = 'ITEM002'), (SELECT id FROM objects WHERE serial = 'ITEM004')),
    ((SELECT id FROM objects WHERE serial = 'ITEM003'), (SELECT id FROM objects WHERE serial = 'ITEM005'));

