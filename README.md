```
oooooooooo.    o8o                      
`888'   `Y8b   `"'                      
 888      888 oooo   .ooooo.   .ooooo.  
 888      888 `888  d88' `"Y8 d88' `88b 
 888      888  888  888       888ooo888 
 888     d88'  888  888   .o8 888    .o 
o888bood8P'   o888o `Y8bod8P' `Y8bod8P' 
```
                                        
# DICE - DICE Improves your Campaign Experience.

DICE allows RPG players and game masters to quickly and easily manage their stats, inventory, quests, and currency. DICE aims to require minimal interaction to edit game data, allowing users to focus more on playing their game rather than tedious bookkeeping.

# Usage
The controls for DICE are meant to require as few keystrokes as possible.

## On the main screen

Key      | Action
-------  | ------------------------------------------------------
`space`  | Roll default dice. From here, more dice can be rolled.
`tab`    | cycle through windows
`+`      | increment a value
`-`      | decrement a value


# File Format
Do you already have character or game data saved in a spreadsheet? If you're anything like me, you probably do. Seeing as everyone will probably keep track of things differently, there is no way for me to prepare for every possible format and organization scheme that other players will have devised. The best solution I could think of was to use [sqlite](https://www.sqlite.org/) as my file format. This allows for other programs (or at least the standard sqlite cli interface) to easily edit DICE files. Sqlite is one of the most widely used pieces of software on the planet, which has the added benefit of massive online resources and documentation. There are plenty of guides on how to import spreadsheets into an sqlite database, so I will not cover them here. To see how DICE organizes data, take a look in the schemas folder.

# Dependencies
To build, you will need to have the following libraries installed:

- ncurses (including the developer package)

DICE no longer requires any sqlite-related packages.
