DROP DATABASE IF EXISTS bfss;
CREATE DATABASE bfss;
USE bfss;
-- Drop everything if it exists (for re-runs)
DROP TABLE IF EXISTS location_services;
DROP TABLE IF EXISTS services;
DROP TABLE IF EXISTS practitioners;
DROP TABLE IF EXISTS locations;

-- 1. Locations table
CREATE TABLE locations (
    location_id         INT AUTO_INCREMENT PRIMARY KEY,
    location_code       CHAR(10) NOT NULL UNIQUE,                  -- e.g. ANC-M, BET-M
    location_name       VARCHAR(180) NOT NULL,
    parent_location_id  INT NULL,
    location_type       ENUM(
        'Main Campus',
        'Child & Family Center',
        'Behavioral Health Campus',
        'Outpatient Clinic',
        'Therapy & Rehabilitation',
        'Urgent Care',
        'Community Support Office',
        'Administrative Headquarters'
    ) NOT NULL DEFAULT 'Main Campus',
    address             VARCHAR(200),
    city                VARCHAR(100) NOT NULL,
    state               CHAR(2) DEFAULT 'AK',
    zip_code            CHAR(5),
    phone               VARCHAR(20),
    is_active           TINYINT(1) DEFAULT 1,
    opened_date         DATE,
    notes               TEXT,

    FOREIGN KEY (parent_location_id) REFERENCES locations(location_id)
      ON DELETE SET NULL,

    INDEX idx_city (city),
    INDEX idx_type (location_type),
    INDEX idx_parent (parent_location_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ===================================================================
-- Bayside Family Support Specialists – A to E Port Locations
-- ===================================================================

INSERT INTO locations (
location_code, location_name, location_type, address, city, zip_code, phone, opened_date, notes
) VALUES
-- A – Anchorage
('ANC-M', 'Bayside Family Support Specialists – Anchorage Main Campus',         'Main Campus',            '3200 Providence Dr',        'Anchorage',   '99508', '(907) 562-2211', '1998-04-01', 'Flagship family-centered campus with full pediatric and adult services'),
('ANC-CF', 'Bayside Children & Family Center at Anchorage',                       'Child & Family Center',  '3200 Providence Dr',        'Anchorage',   '99508', '(907) 562-2940', '2005-09-15', 'Dedicated pediatric wing and family therapy suites'),
('ANC-BH', 'Bayside Behavioral Health Campus – Anchorage',                        'Behavioral Health Campus','3841 Piper St',             'Anchorage',   '99508', '(907) 212-6800', '2012-03-01', 'Inpatient and intensive outpatient mental health services'),

-- B – Bethel
('BET-M', 'Bayside Family Support Specialists – Yukon-Kuskokwim Campus',         'Main Campus',            'PO Box 528',                'Bethel',      '99559', '(907) 543-6300', '2001-06-01', 'Serves Yukon Delta villages with family counseling and telehealth'),
('BET-OC', 'Bethel Family Outpatient Clinic',                                      'Outpatient Clinic',      'PO Box 528',                'Bethel',      '99559', '(907) 543-6789', '2019-11-01', 'Primary care, maternal health, and substance-use support'),

-- C – Cordova
('COR-M', 'Bayside Family Support Specialists – Cordova Community Campus',      'Main Campus',            '602 Chase Ave',             'Cordova',     '99574', '(907) 424-8000', '2003-08-01', 'Family medicine and counseling on Prince William Sound'),
('COR-UC', 'Cordova Family Urgent Care',                                           'Urgent Care',            '602 Chase Ave',             'Cordova',     '99574', '(907) 424-8200', '2017-05-01', 'Walk-in family and pediatric urgent care'),

-- D – Dillingham
('DIL-M', 'Bayside Family Support Specialists – Bristol Bay Campus',             'Main Campus',            '6000 Kanakanak Rd',         'Dillingham',  '99576', '(907) 842-5201', '1999-02-01', 'Serves Bristol Bay communities with strong Alaska Native focus'),
('DIL-TR', 'Dillingham Family Therapy & Rehabilitation Center',                   'Therapy & Rehabilitation','6000 Kanakanak Rd',        'Dillingham',  '99576', '(907) 842-9450', '2021-07-01', 'Physical, occupational, and family therapy services'),

-- E – Dutch Harbor / Unalaska
('DUT-M', 'Bayside Family Support Specialists – Iliuliuk Family Campus',         'Main Campus',            '34 Lavelle Rd',             'Unalaska',    '99685', '(907) 581-1202', '2000-09-01', 'Serves commercial fishing families and Aleutian communities'),
('DUT-OC', 'Dutch Harbor Family Health Clinic',                                    'Outpatient Clinic',      '34 Lavelle Rd',             'Unalaska',    '99685', '(907) 581-2740', '2010-04-01', 'Occupational health, family planning, and counseling');

-- Set hierarchical relationships (sub-campuses point to their main campus)
-- Anchorage sub-campuses
SET @anc_parent = (SELECT location_id FROM locations WHERE location_code = 'ANC-M');
UPDATE locations SET parent_location_id = @anc_parent WHERE location_code IN ('ANC-CF', 'ANC-BH');

-- Bethel
SET @bet_parent = (SELECT location_id FROM locations WHERE location_code = 'BET-M');
UPDATE locations SET parent_location_id = @bet_parent WHERE location_code = 'BET-OC';

-- Cordova
SET @cor_parent = (SELECT location_id FROM locations WHERE location_code = 'COR-M');
UPDATE locations SET parent_location_id = @cor_parent WHERE location_code = 'COR-UC';

-- Dillingham
SET @dil_parent = (SELECT location_id FROM locations WHERE location_code = 'DIL-M');
UPDATE locations SET parent_location_id = @dil_parent WHERE location_code = 'DIL-TR';

-- Dutch Harbor (the one that was failing)
SET @dut_parent = (SELECT location_id FROM locations WHERE location_code = 'DUT-M');
UPDATE locations SET parent_location_id = @dut_parent WHERE location_code = 'DUT-OC';

-- 2. Services table
CREATE TABLE services (
service_id            INT PRIMARY KEY,
service_name          VARCHAR(100) NOT NULL,
avg_wait_time_minutes INT,
requires_fasting      BOOLEAN
);

INSERT INTO services (service_id, service_name, avg_wait_time_minutes, requires_fasting) VALUES
(1, 'Emergency Care',                       240, false),
(2, 'MRI',                                   90, false),
(3, 'Childbirth',                          720, false),
(4, 'Colonoscopy',                           45, true),
(5, 'Therapy (Talk)',                        50, false),
(6, 'Caffeine IV Drip (Gift Shop Special)',   2, false),
(7, 'General Surgery',                      180, true),
(8, '"You''ll Be Fine" Reassurance',         0, true);

-- 3. Junction table (many-to-many)
CREATE TABLE location_services (
location_id INT REFERENCES locations(location_id),
service_id  INT REFERENCES services(service_id),
PRIMARY KEY (location_id, service_id)
);

INSERT INTO location_services (location_id, service_id) VALUES
(1,1), (1,8),
(2,7), (2,8),
(3,3), (3,8),
(4,2), (4,4),
(5,6),
(6,5), (6,8),
(7,4), (7,7);

-- 4. Practitioners table
CREATE TABLE practitioners (
practitioner_id   INT PRIMARY KEY,
full_name         VARCHAR(100) NOT NULL,
title             VARCHAR(100),
primary_location_id INT REFERENCES locations(location_id),
years_experience  INT,
bio               TEXT
);

INSERT INTO practitioners (practitioner_id, full_name, title, primary_location_id, years_experience, bio) VALUES
(101, 'Dr. Hugo First',          'Chief of Emergency Medicine', 1, 20, 'Has treated everything from paper cuts to "I thought I could juggle chainsaws." Still waiting for the Nobel Prize in Eye-Rolling.'),
(102, 'Nurse Jackie Daniels',   'Head Nurse, ER',              1, 14, 'Runs on caffeine, sarcasm, and the sheer power of telling patients "this is why we can''t have nice things."'),
(103, 'Dr. Robin Banks',         'Cardiologist',                2, 16, 'Specialty: stealing hearts (and fixing them when your diet tries to murder you).'),
(104, 'Dr. Anita Bath',          'Dermatologist',               4, 11, 'Will prescribe cream for that... or just tell you to stop Googling at 3 a.m.'),
(105, 'Nurse Joy Alvarez',       'Maternity Ward Lead',         3,  9, 'Delivered 1,247 babies and still cries at every single one. Pokemon references encouraged.'),
(106, 'Dr. Will Travel',         'Orthopedic Surgeon',          2, 25, 'Has replaced more hips than a bad dance club. Patients leave walking... eventually.'),
(107, 'Dr. Paige Turner',        'Psychiatrist',                6, 13, 'Listens to your problems, then assigns you homework. Favorite phrase: "How does that make you feel on a scale of 1 to insurance co-pay?"'),
(108, 'Dr. Barry Cade',          'General Surgeon',             7, 18, 'Once removed 47 feet of Christmas lights from a guy who "just wanted to feel festive inside."'),
(109, 'Nurse Barbara Gordon',    'ICU Night Shift Legend',      2, 10, 'Can silence a beeping monitor with a single death glare. Sleeps hanging upside-down like a bat (probably).'),
(110, 'Dr. Sue Permann',         'Anesthesiologist',            2, 15, 'Counts backward from 10 so smoothly you''ll wake up wondering if you ever had a surgery at all.');
-- SELECT * FROM locations;
-- SELECT * FROM services;
-- SELECT * FROM location_services;
-- SELECT * FROM practitioners;REATE TABLE locations
