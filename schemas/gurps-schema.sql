/*
*   DICE database schema for GURPS
*   Version 0.0.3
*/

---------------------------------------[Attributes and Skills]-------------------------------------

CREATE TABLE IF NOT EXISTS 'character'(
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    unspentpoints INTEGER,
    startcash INTEGER
);

CREATE TABLE IF NOT EXISTS 'attributes' (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    abbr TEXT NOT NULL,
    cost INTEGER NOT NULL DEFAULT 1,
    points INTEGER DEFAULT 0,
    lvl, INTEGER
);

/* Populate the attributes table with default values */

INSERT INTO attributes (name, abbr, cost, lvl) VALUES
    ('STRENGTH',      'ST',  10, 10),
    ('DEXTERITY',     'DX',  20, 10),
    ('INTELLIGENCE',  'IQ',  20, 10),
    ('HEALTH',        'HT',  20, 10);


/* Secondary characteristics depend on base attributes. */

CREATE TABLE IF NOT EXISTS 'secondary' (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    abbr TEXT,
    cost INTEGER DEFAULT 1,   --cost in points per lvl
    points INTEGER DEFAULT 0,
    currentValue INTEGER DEFAULT 0,
    base TEXT       --base value in relation to primary attributes
);

/* Populate defaults for secondary attributes */

INSERT INTO secondary (name, abbr, cost, base) VALUES
    ('DAMAGE',          'DMG',  1,      'ST'),
    ('BASIC LIFT',      'BL',   1,      '(ST*ST)/5'),
    ('HIT POINTS',      'HP',   2,      'HT'),
    ('WILL',            'WILL', 5,      'IQ'),
    ('PERCEPTION',      'PER',  5,      'IQ'),
    ('FATIGUE POINTS',  'FP',   3,      'HT'),
    ('BASIC SPEED',     'BL',   5,      'HT+DX'),    --Measured in fourths of a yard
    ('BASIC MOVE',      'BM',   5,      '(HT+DX)/4');

/* Contains both advantages and disadvantages */
CREATE TABLE IF NOT EXISTS 'prosandcons' (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    minimum INTEGER,    --Minimum number of points to unlock the first lvl
    cost INTEGER,       --Cost per lvl after minimum, if applicable. If NULL, the player must enter the cost manually.
    maxLevel INTEGER,   --some advantages have a cap, others do not.
    description, TEXT
);

CREATE TABLE IF NOT EXISTS 'skills' (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    base TEXT NOT NULL, --ST, DX, etc. or another skill name
    diff TEXT NOT NULL, --EASY, AVERAGE, HARD, VERY HARD
    points INTEGER,       --points points into skill
    description TEXT
);

CREATE TABLE IF NOT EXISTS 'knownskills' (
    id INTEGER PRIMARY KEY,
    points INTEGER,       --points invested into skill
    skill INTEGER,
    FOREIGN KEY(skill) REFERENCES skills(id)
);

---------------------------------------[Currency and Inventory]-------------------------------------

CREATE TABLE IF NOT EXISTS 'inventory' (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    weight REAL,
    quantity INTEGER,
    cost INTEGER,
    description TEXT,
    lore TEXT
);

CREATE TABLE IF NOT EXISTS 'handweapons'(
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    swing INTEGER,
    swingtype TEXT, --Crushing/Cutting
    thrust INTEGER, 
    thrusttype TEXT, --Crushing/Impailing
    parry INTEGER,
    skill INTEGER,
    notes TEXT,
    cost INTEGER,
    weight REAL,
    FOREIGN KEY(skill) REFERENCES skills(id)
);

CREATE TABLE IF NOT EXISTS 'rangedweapons'(
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    damage INTEGER,
    acc INTEGER,
    range INTEGER,
    rof INTEGER,
    shots INTEGER,
    st INTEGER,
    bulk INTEGER,
    rcl INTEGER,
    lc INTEGER,
    notes TEXT,
    cost INTEGER,
    weight REAL
);

CREATE TABLE IF NOT EXISTS 'armor'(
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    dr INTEGER,
    weight REAL,
    cost INTEGER
);

CREATE TABLE IF NOT EXISTS 'transactions' (
    id INTEGER PRIMARY KEY,
    cost INTEGER NOT NULL,
    quantity INTEGER,    --For buying/selling multiple items i.e. 3 quivers of arrows
    perPack INTEGER,     --i.e. 15 arrows to the quiver
    description TEXT
);

-------------------------------------------[Notes and Data]-----------------------------------------

CREATE TABLE IF NOT EXISTS 'notes' (
    id INTEGER PRIMARY KEY,
    title TEXT,
    content TEXT NOT NULL,
    priority INTEGER
);

CREATE TABLE IF NOT EXISTS 'quests' (
    id INTEGER PRIMARY KEY,
    title TEXT,
    givenBy TEXT,
    description TEXT NOT NULL,
    cashReward INTEGER,
    timeframe REAL  --how long, in days, one has to complete the quest
);

