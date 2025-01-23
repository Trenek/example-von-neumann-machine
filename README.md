# Example Von Neumann Machine
## command line arguments
* `-f <source file>` - choose source file to execute, default `main.vnm`
* `-base <base>` - choose number base (only for output), default `10`
* `-signed` - let every number be signed
* `-unsigned` - let every number be unsinged
* `debugMode <mode>` go through every step. Possible modes:
  * `cs` - clean and stop after every line
  * `c` - clean after every line
  * `s` - stop after every line
