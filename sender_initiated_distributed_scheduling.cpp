#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;
#define pb push_back
#define N 10
#define bThreshold 100 

// defining the colors
const std::string red("\033[0;31m");
const std::string green("\033[1;32m");
const std::string yellow("\033[1;33m");
const std::string cyan("\033[0;36m");
const std::string magenta("\033[0;35m");
const std::string reset("\033[0m");

//defining global timer
int timer = 0;
//Process structure
struct Process{
	int pid;
	int arrival_time;
	int burst_time;
	int start_time;
	int affinity;
  //Default comparator for process
	bool operator < ( const Process &str) const
	{
		return arrival_time < str.arrival_time;
	}
	void printProcess()
	{
		cout << "Process : " << pid << "Arrival Time : "<< arrival_time << "Burst Time : "<< burst_time << "Start Time : "<<start_time;
	}
};

//Global Datastructures of Processes
vector<int> threshold;
vector< vector<int> > processorQueue;
vector<int> queuePointer;
unordered_map<int,Process> processMap; 

/**
	 * Checks if the threshold for a certain processor has been met
	 * 
	 * @param processor id of the Processor to be checked
	 * @return boolean value indicating whether the processor can be allocated a process
	 */

//Check if the threshold of a process has been reached or not
bool isThresholdReached(int processor)
{
  int burstSum = 0;
  //Calculate the burst times of all the processes in the ready of the processor
  for(int i=max(queuePointer[processor],0);i<processorQueue[processor].size();i++){
    burstSum+=processMap[processorQueue[processor][i]].burst_time;
  }
  //If the total burst time of all the processes in the ready queue is greater then  the threshold then return true
  if(burstSum>=threshold[processor])
    return true;
  return false;
}

/**
	 * Checks for a Processor which is operating below its threshold and returns its id.
	 * 
	 * @param sourceId Id of Polling  Node
	 * @return id index of processor
	 */
int pollAllNodes(int sourceId){
  
  for(int i=0;i<N;i++)						// iterate all the processors  
  {
    if(i == sourceId)
      continue;
   	int pointer=queuePointer[i];											// get currrent position in ready queue for processor i
    if(pointer == -1)
      return i;
    vector<int> readyQueue=processorQueue[i];					// get ready queue of processor i
    int burstTimeSum=0;																// stores the total burst time scheduled on processor i
    for(int j=pointer;j<readyQueue.size();j++)				// iterate the ready queue from the current pointer till end
    {
      Process p=processMap[readyQueue[j]];						// get current process being run
      if(j==pointer)															
      {
        /*for current process add the remaining burst time left*/
        burstTimeSum+=p.start_time+p.burst_time-timer;
      }
      else
      {
      	/*for the remaining processes in the ready queue */
        burstTimeSum+=p.burst_time;
      }
    }
    if(burstTimeSum<threshold[i])		// if threshold of processor i has not been reached then return this process
      return i;
  }
  return sourceId;			// if no processor with suitable threshold found then return the original source id
} 

// update all the nodes at current arrival time
void updateAllNodes(int arrivalTime){
  //Upodate the current pointers of each of the processors
  for(int i=0; i<N; i++)
  {
    int j = queuePointer[i];
    if(j==-1)
      j=0;
    queuePointer[i]=-1;
    //Check the entire process queue for the processor to position the pointer in the correct position
    for(;j<processorQueue[i].size();j++)
    {
      Process p = processMap[processorQueue[i][j]];
      //If this condition is true then it means that the process and after need to complete execution
      if(p.start_time<=arrivalTime&&(p.start_time + p.burst_time) >= arrivalTime)
      {
        queuePointer[i]=j;
        break;
      }
    }
  }
} 


//Print the Gantt Chart
void printGaantChart(int id){
  //HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  printf("\033c");
  for(int i=0;i<processorQueue.size();i++){
    cout<<"\n--------------------------------------PROCESSOR "<<(i)<<" (THRESHOLD : "<<threshold[i]<<") ---------------------------------------------------\n\n";
    for(int j=0;j<processorQueue[i].size();j++){
      //Convert all the integers to strings
      string pid = to_string(processorQueue[i][j]);
      string arrival_time = to_string(processMap[processorQueue[i][j]].arrival_time);
      string start_time = to_string(processMap[processorQueue[i][j]].start_time);
      string end_time = to_string(processMap[processorQueue[i][j]].start_time+processMap[processorQueue[i][j]].burst_time);
      string burst_time = to_string(processMap[processorQueue[i][j]].burst_time);
      string affinity = to_string(processMap[processorQueue[i][j]].affinity);

      if(id==processMap[processorQueue[i][j]].pid)
      {
        //SetConsoleTextAttribute(hConsole, 12);
        cout<<yellow<<setw(10)<<(" Process: ")<<setw(5)<<(pid)<<setw(20)<<("|\tArrival Time: ")<<setw(5)<<(arrival_time)<<setw(15)<<("|\tStart Time:")<<setw(5)<<
          (start_time)<<setw(15)<<("|\tEnd Time:")<<setw(5)<<(end_time)<<setw(15)<<("\t|    Burst Time: ")<<setw(5)<<(burst_time)<<setw(15)<<("|\tAffinity:")<<setw(5)<<(affinity)<<reset<<"\n";
      
      }
      else
      {
        //SetConsoleTextAttribute(hConsole, 15);
        cout<<setw(10)<<(" Process: ")<<setw(5)<<(pid)<<setw(20)<<("|\tArrival Time: ")<<setw(5)<<(arrival_time)<<setw(15)<<("|\tStart Time:")<<setw(5)<<
          (start_time)<<setw(15)<<("|\tEnd Time:")<<setw(5)<<(end_time)<<setw(15)<<("\t|    Burst Time: ")<<setw(5)<<(burst_time)<<setw(15)<<("|\tAffinity:")<<setw(5)<<(affinity)<<"\n";
      }
    }
  }  
}

int main()
{
  //seed for generating random numbers
  srand(time(0));
  //Open an input stream to the file process.txt
  ifstream in("process1.txt");
  string temp;
  in >> temp >> temp >> temp >> temp;
  int pid,arrival_time,burst_time,affinity,i;
  vector<Process> pq;
  //Accept the input from the file
  while(in >> pid >> arrival_time >> burst_time>>affinity)
  {
    Process p;
    p.pid = pid;
    p.arrival_time = arrival_time;
    p.burst_time = burst_time;
    p.affinity=affinity;
    pq.pb(p);
  }
  sort(pq.begin(),pq.end());
  //Initialisation Loop . Initialise all the data structures
  for(i=0;i<N;i++)
  {
    threshold.pb(rand()%30+20);
    queuePointer.pb(-1);
    vector<int> v;
    processorQueue.pb(v);
  }
  //Assign the process to the affinity processor or poll all the nodes to find the first node that will accept the process.
  //If process cannot be assigned to the affinity processor then assign process to the first node which has not reached the threshold
  //All scheduling within indivisual processors is FCFS
  for(i=0;i<pq.size();i++)
  {
    int processor = pq[i].affinity;
    //If no processor assigned then randomly select a affinity processor
    if(processor==-1)
    {
      processor = rand()%N;
    }
    //update all  the nodes
    updateAllNodes(pq[i].arrival_time);
    //Check if threshold of the assigned processor has been reached
    //If true then find the first node able to accept the process
    if(isThresholdReached(processor))
    {
      processor = pollAllNodes(processor);
    }
    vector<int> vpp = processorQueue[processor];
    int pp,stime;
    //Calculate the start time of the process
    if(vpp.size()!=0)
    {
      pp = vpp[vpp.size()-1];
      stime = processMap[pp].start_time;
      stime+= processMap[pp].burst_time;
    }
    else
    {
      stime = pq[i].arrival_time;
    }
    //Take the maximum of the arrival time and calculated start time as the start time of the process
    pq[i].start_time=max(stime,pq[i].arrival_time);
    //Insert the process entry into the Gantt Chart
    processMap[pq[i].pid]=pq[i];
    //Enter the process into the ready queue of the assigned processor
    processorQueue[processor].pb(pq[i].pid);
    //refresh the Gantt Chart 
    printGaantChart(pq[i].pid);
    sleep(2);
    cout<<"\n***************************************************************************************************************\n";
  }  			
  //printGaantChart();
  cout << "\n----------------------------------------------Simulation Ends----------------------------------------------------"<<endl;
  return 0;
}











