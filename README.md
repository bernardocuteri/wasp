Run WASP with eager propagators:
====

To run WASP with eager propagators run the script run_eager.sh with encoding constraints and instance

Example
```
sh run_eager.sh encoding constraints instance
```
* constraints is the file containing the set of constraints to be unit propagated 
* encoding and instance are both ASP files

Requirements
===
In order to run wasp with eager propagators you need an ASP grounder.

gringo is the grounder used in the run_eager.sh script
In order to install gringo just type:
```
sudo apt install gringo
```
