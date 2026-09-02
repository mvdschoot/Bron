

De physics engine moet:
- totaal separate zijn
    - In het begin iig
    - Transformers met assimp enz kunnen aan de Bron kant, niet aan de physics kant.
- Object:
    - Heeft een ID, enum -> wat voor object het is. 
    - Alle geometrische data om het object te beschrijven. 
    - NO COMMON ANCESTOR!!
- Systems:
    - A system contains objects that can interact with each other. 
    - Everything is done per system. An object does not do anything if it is outside a system.
    - The system's object contains all the interaction data, the objects don't contain any such data themselves. 
- Loop:
    - Triggered per system.
    - loop over objecten in system
    - Krachten list * time passed + old_position
    - Check for collisions

Elk systeem heeft
- eigen BVH

