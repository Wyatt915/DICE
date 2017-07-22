CREATE TABLE IF NOT EXISTS items(
    name TEXT NOT NULL,
    weight REAL,
    qty INTEGER,
    cost INTEGER,       --How much it was purchased for per unit (not necessarily its value)
    description TEXT,   --What is this item?
    lore TEXT           --The item's backstory, history, etc
);
