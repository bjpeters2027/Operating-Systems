To implement this project we started by declaring a job struct that holds the important values 
turnaround, wait, resonse time etc). Our main method will take in the args and call the 
corresponding method depending on the scheduling type.

FIFO:
  The code reads line by line to get how many jobs there will be and then uses that number to 
  malloc an array of jobs. Then it reads through the file again to generate the jobs in the array.
  Another loop then runs which updates the fields in the job struct using a clock variable to simulate 
  time and prints out the execution messages. Finally the analysis method is called which loops 
  through the job list and prints out the required output as well as adding the values up to later 
  get the averages and print those as well.

SJF:
  Similar to FIFO in the way it accumulates the list of jobs. However SJF then sorts them and executes
  the printing and analysis on the sorted list.

RR:
   Similar to FIFO and SJF in the way it accumulates the list of jobs. However it then declares a variable 
  to keep track of the number of completered jobs and loops until that number is no longer less than the
  total number of jobs. Within that loop we first check if the time remaining is equal to -1 which is the
  identifier for a completed job. If it is, it skips that job, if not it processes it in the following way;
  The program will check to see if the time remaining is less than the time slice. If so, it increases the
  clock by how much time was remaining, increment the number of finished jobs, and assigns the field variables.
  and only if it is the jobs first time being visited does it assign response time (this is measured by the 
  "first" field). If the time remaining is not less than the time slice it decreases time remaining by the time
  slice and increases the clock by the time slice value. This process is followed by the analysis function
  call as decribed in previous methods.
