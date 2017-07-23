/*
*   DICE database schema for GURPS
*   Version 0.0.3
*/

---------------------------------------[Attributes and Skills]-------------------------------------

CREATE TABLE IF NOT EXISTS 'character'(
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    points INTEGER,
    startcash INTEGER
);

CREATE TABLE IF NOT EXISTS 'attributes' (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    abbr TEXT NOT NULL,
    cost INTEGER NOT NULL,
    invested INTEGER,
    lvl, INTEGER
);

/* Populate the attributes table with default values */

INSERT INTO attributes (name, abbr, cost) VALUES
    ('Strength', 'ST', 10),
    ('Dexterity', 'DX', 20),
    ('Intelligence', 'IQ', 20),
    ('Health', 'HT', 20);


/* Secondary characteristics depend on base attributes. */

CREATE TABLE IF NOT EXISTS 'secondary' (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    abbr TEXT,
    cost INTEGER,   --cost in points per level
    value INTEGER,
    currentValue INTEGER,
    base TEXT       --base value in relation to primary attributes
);

/* Populate defaults for secondary attributes */

INSERT INTO secondary (name, abbr, cost, base) VALUES
    ('Damage',          'Dmg',  NULL,   'ST'),
    ('Basic Lift',      'BL',   NULL,   '(ST*ST)/5'),
    ('Hit Points',      'HP',   2,      'HT'),
    ('Will',            'WILL', 5,      'IQ'),
    ('Perception',      'PER',  5,      'IQ'),
    ('Fatigue Points',  'FP',   3,      'HT'),
    ('Basic Speed',     NULL,   5,      'HT+DX'),    --Measured in fourths of a yard
    ('Basic Move',      NULL,   5,      '(HT+DX)/4');

/* Contains both advantages and disadvantages */
CREATE TABLE IF NOT EXISTS 'prosandcons' (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    minimum INTEGER,    --Minimum number of points to unlock the first level
    cost INTEGER,       --Cost per level after minimum, if applicable. If NULL, the player must enter the cost manually.
    maxLevel INTEGER,   --some advantages have a cap, others do not.
    description, TEXT
);

CREATE TABLE IF NOT EXISTS 'skills' (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    base TEXT NOT NULL, --ST, DX, etc. or another skill name
    diff TEXT NOT NULL, --EASY, AVERAGE, HARD, VERY HARD
    pnts INTEGER,       --points invested into skill
    description TEXT
);

CREATE TABLE IF NOT EXISTS 'knownskills' (
    id INTEGER PRIMARY KEY,
    pnts INTEGER,       --points invested into skill
    skill INTEGER,
    FOREIGN KEY(skill) REFERENCES skills(id)
);

---------------------------------------[Currency and Inventory]-------------------------------------

CREATE TABLE IF NOT EXISTS 'inventory' (
    id INTEGER PRIMARY KEY,
    item TEXT NOT NULL,
    weight REAL,
    quantity INTEGER,
    cost INTEGER,
    description TEXT,
    lore TEXT
);

CREATE TABLE IF NOT EXISTS 'handweapons'(
    id INTEGER PRIMARY KEY,
    weapon TEXT NOT NULL,
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
    weapon TEXT NOT NULL,
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

