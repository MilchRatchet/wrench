# Gameplay

The gameplay file contains all the object instances for a level, with the exception of Deadlocked where instances are also stored in the art instances file and the missions.

## Headers

### R&C1

|      | 0x0             | 0x4            | 0x8           | 0xc             |
| ---- | --------------- | -------------- | ------------- | --------------- |
| 0x00 | properties      | lights         | cameras       | sound instances |
| 0x10 | us english      |                | french        | german          |
| 0x20 | spanish         | italian        |               |                 |
| 0x30 | tie classes     | tie instances  | shrub classes | shrub instances |
| 0x40 | moby classes    | moby instances | moby groups   | global pvar     |
| 0x50 | pvar scratchpad | pvar table     | pvar data     | pvar relatives  |
| 0x60 | cuboids         | spheres        | cylinders     |                 |
| 0x70 | paths           | grind paths    |               |                 |
| 0x80 | light triggers  |                |               | occlusion       |
| 0x90 |                 |                |               |                 |

### R&C2 and R&C3

|      | 0x0             | 0x4               | 0x8             | 0xc             |
| ---- | --------------- | ----------------- | --------------- | --------------- |
| 0x00 | properties      | lights            | cameras         | sound instances |
| 0x10 | us english      |                   | french          | german          |
| 0x20 | spanish         | italian           |                 |                 |
| 0x30 | tie classes     | tie instances     | tie groups      | shrub classes   |
| 0x40 | shrub instances | shrub groups      | moby classes    | moby instances  |
| 0x50 | moby groups     | global pvar       | pvar scratchpad | pvar table      |
| 0x60 | pvar data       | pvar relatives    | cuboids         | spheres         |
| 0x70 | cylinders       |                   | paths           | grind paths     |
| 0x80 |                 | light triggers    |                 |                 |
| 0x90 | occlusion       | tie ambient rgbas | areas           |                 |

### Deadlocked (gameplay core)

|      | 0x0            | 0x4         | 0x8             | 0xc             |
| ---- | -------------- | ----------- | --------------- | --------------- |
| 0x00 | properties     | cameras     | sound instances | us english      |
| 0x10 | uk english     | french      | german          | spanish         |
| 0x20 | italian        | japanese    | korean          | moby classes    |
| 0x30 | moby instances | moby groups | global pvar     | pvar scratchpad |
| 0x40 | pvar table     | pvar data   | pvar relatives  | cuboids         |
| 0x50 | spheres        | cylinders   |                 | paths           |
| 0x60 | grind paths    |             |                 |                 |
| 0x70 |                | areas       |                 |                 |

### Deadlocked (art instances)

|      | 0x0               | 0x4             | 0x8           | 0xc        |
| ---- | ----------------- | --------------- | ------------- | ---------- |
| 0x00 | lights            | tie classes     | tie instances | tie groups |
| 0x10 | shrub classes     | shrub instances | shrub groups  | occlusion  |
| 0x20 | tie ambient rgbas |                 |               |            |
| 0x30 |                   |                 |               |            |

### Deadlocked (gameplay mission instances)

Some files are different to this?

|      | 0x0             | 0x4            | 0x8         | 0xc            |
| ---- | --------------- | -------------- | ----------- | -------------- |
| 0x00 | moby classes    | moby instances | moby groups | global pvar    |
| 0x10 | pvar scratchpad | pvar table     | pvar data   | pvar relatives |

