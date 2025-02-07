
This field synchronization simulator may look very complex due to the sheer number of 
Pthread variables used. The code can be summarized as this: The simulation starts by 
initializing the field and sport structs, all of which hold mutexs, conditinals and 
semaphores. Then the threads are created. When the first threads are created they check
the field for the current sport to be played and a "captain" takes the field. All a 
captain does is check forwhen it's time for their sport and then releases the sports
mutex held in the field "m". Then the playes take the field, the game runs for a random 
time between 3 and 7 seconds. The playes then exit the field and change the currentSport 
field of the field struct to the next sport to be played which is represented as 
(currentSport + 1 % 3). Then the next team captain takes the field and all is repeated.

Rugby is a special cas in which a captain represents a pair of ruby players and is capped 
at 15 pairs of players as are the parameters of the assignment.
