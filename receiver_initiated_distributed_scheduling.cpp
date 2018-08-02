#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;
#define pb push_back
#define N 10
#define bThreshold 100 

// constants defined for printing colored lines on console
const std::string red("\033[0;31m");
const std::string green("\033[1;32m");
const std::string yellow("\033[1;33m");
const std::string cyan("\033[0;36m");
const std::string magenta("\033[0;35m");
const std::string reset("\033[0m");

int timer = 0; // global timer for simulation

// struct for Process
struct Process{
  int pid;
  int arrival_time;
  int burst_time;
  int start_time;
  int affinity;
  bool operator < ( const Process &str) const // comparator for sorting
  {
    return arrival_time < str.arrival_time;
  }
  void printProcess()
  {
    cout << "Process : " << pid << "Arrival Time : "<< arrival_time << "Burst Time : "<< burst_time << "Start Time : "<<start_time;
  }
};

vector<int> threshold;                                // to store threshold values of all nodes (processors)
vector< vector<int> > processorQueue;                 // 2D vector : stores the process allocation queue for all processors
vector<int> queuePointer;                             // points to currently executing process in the processorQueue for each processor 
unordered_map<int,Process> processMap;                // hash map to store all the processes by their pid

// returns the most underloaded proceesor id : overloading and underloading are defined in terms of threshold - scheduled burst time
int getUnderloadedNode()
{
  int maximum=0,ret=0;
  for(int i=0;i<N;i++)
  {
    int pointer=queuePointer[i];                      // get currrent position in ready queue for processor i
    if(pointer == -1)
    {
      if(maximum<threshold[i])
      {
        maximum =threshold[i];
        ret=i;
      }
      continue;
    }
    vector<int> readyQueue=processorQueue[i];         // get ready queue of processor i
    int burstTimeSum=0;                               // stores the total burst time scheduled on processor i
    for(int j=pointer;j<readyQueue.size();j++)        // iterate the ready queue from the current pointer till end
    {
      Process p=processMap[readyQueue[j]];            // get current process being run
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
    if((threshold[i]-burstTimeSum)>maximum)
    {
      maximum=threshold[i]-burstTimeSum;
      ret=i;
    }
  }
  return ret;
}


// returns the most overloaded proceesor id : overloading and underloading are defined in terms of threshold - scheduled burst time
int getOverloadedNode()
{
  int minimum=INT_MAX,ret=0;
  for(int i=0;i<N;i++)
  {
    int pointer=queuePointer[i];                      // get currrent position in ready queue for processor i
    if(pointer == -1)                                 // if the processor is idle
    {
      if(minimum>threshold[i])                        
      {
        minimum=threshold[i];
        ret=i;
      }
      continue;
    }
    vector<int> readyQueue=processorQueue[i];         // get ready queue of processor i
    if(pointer == readyQueue.size()-1)
    {
      continue;
    }
    int burstTimeSum=0;                               // stores the total burst time scheduled on processor i
    for(int j=pointer;j<readyQueue.size();j++)        // iterate the ready queue from the current pointer till end
    {
      Process p=processMap[readyQueue[j]];            // get current process being run
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
    if((threshold[i]-burstTimeSum)<minimum)
    {
      minimum=threshold[i]-burstTimeSum;
      ret=i;
    }
  }
  return ret;
}

// update all the nodes at current arrival time
void updateAllNodes(int arrivalTime){
  for(int i=0; i<N; i++)
  {
    queuePointer[i]=-1;
    for(int j=0;j<processorQueue[i].size();j++)
    {
      Process p = processMap[processorQueue[i][j]];
      if(p.start_time<=arrivalTime&&(p.start_time + p.burst_time) >= arrivalTime)// if the process is currently executing
      {
        queuePointer[i]=j; // update the queuePointer for the current processor
        break;
      }
    }
  }
} 

// gets the last scheduled process (only if it is not currently being executed) for any processor
int getLastProcess(int node)
{
  vector<int> readyQueue = processorQueue[node];    // gets all the processes on node
  if(queuePointer[node]==-1)                        // if the processor has no scheduled process
    return -1;
  if(queuePointer[node]==readyQueue.size()-1)       // if the is no process in ready queue ie processor is executing the last process scheduled on it 
    return -1;
  else
    return readyQueue[readyQueue.size()-1];
}

// allocates process pid to processor node
void allocateProcess(int pid,int node)
{
    vector<int> readyQueue = processorQueue[node];
    int id,stime;
    Process process = processMap[pid];
    if(readyQueue.size()!=0)                        // calculate the start time of the the new process based on last scheduled process on processor
    {
      id = readyQueue[readyQueue.size()-1];
      stime = processMap[id].start_time;
      stime+= processMap[id].burst_time;
    }
    else
    {
      stime = timer;                                // if the processor has no scheduled processes, set its start time to now (timer)
    }
    process.start_time=max(stime,process.arrival_time);
    processMap[pid]=process;
    processorQueue[node].pb(pid);
}

// prints all the processing information about all the processors
void printGaantChart(int id){
  printf("\033c"); // clear the screen
  cout<<"timer : "<<timer<<"\n";
  for(int i=0;i<processorQueue.size();i++){
    cout<<"\n--------------------------------------PROCESSOR "<<(i)<<" (THRESHOLD : "<<threshold[i]<<") ---------------------------------------------------\n\n";
    for(int j=0;j<processorQueue[i].size();j++){
      string pid = to_string(processorQueue[i][j]);
      string arrival_time = to_string(processMap[processorQueue[i][j]].arrival_time);
      string start_time = to_string(processMap[processorQueue[i][j]].start_time);
      string end_time = to_string(processMap[processorQueue[i][j]].start_time+processMap[processorQueue[i][j]].burst_time);
      string burst_time = to_string(processMap[processorQueue[i][j]].burst_time);
      string affinity = to_string(processMap[processorQueue[i][j]].affinity);

      if(id==processMap[processorQueue[i][j]].pid&&stoi(end_time)>=timer) // current change
      {
        cout<<yellow<<setw(10)<<(" Process: ")<<setw(5)<<(pid)<<setw(20)<<("|\tArrival Time: ")<<setw(5)<<(arrival_time)<<setw(15)<<("|\tStart Time:")<<setw(5)<<
          (start_time)<<setw(15)<<("|\tEnd Time:")<<setw(5)<<(end_time)<<setw(15)<<("\t|    Burst Time: ")<<setw(5)<<(burst_time)<<setw(15)<<("|\tAffinity:")<<setw(5)<<(affinity)<<reset<<"\n";
      
      }
      else if(stoi(end_time)<(timer)) // dead process
      {
        cout<<red<<setw(10)<<(" Process: ")<<setw(5)<<(pid)<<setw(20)<<("|\tArrival Time: ")<<setw(5)<<(arrival_time)<<setw(15)<<("|\tStart Time:")<<setw(5)<<
          (start_time)<<setw(15)<<("|\tEnd Time:")<<setw(5)<<(end_time)<<setw(15)<<("\t|    Burst Time: ")<<setw(5)<<(burst_time)<<setw(15)<<("|\tAffinity:")<<setw(5)<<(affinity)<<reset<<"\n";
      
      }
      else // executing process
      {
        cout<<setw(10)<<(" Process: ")<<setw(5)<<(pid)<<setw(20)<<("|\tArrival Time: ")<<setw(5)<<(arrival_time)<<setw(15)<<("|\tStart Time:")<<setw(5)<<
          (start_time)<<setw(15)<<("|\tEnd Time:")<<setw(5)<<(end_time)<<setw(15)<<("\t|    Burst Time: ")<<setw(5)<<(burst_time)<<setw(15)<<("|\tAffinity:")<<setw(5)<<(affinity)<<"\n";
      }
    }
  }  
}

int main()
{
  
  srand(time(0));
  ifstream in("process2.txt");
  string temp;
  in >> temp >> temp >> temp >> temp;
  int pid,arrival_time,burst_time,affinity,i;
  vector<Process> pq;
  // read from file
  while(in >> pid >> arrival_time >> burst_time>>affinity)
  {
    Process p;
    p.pid = pid;
    p.arrival_time = arrival_time;
    p.burst_time = burst_time;
    p.affinity=affinity;
    pq.pb(p);
  }
  
  // sort on the basis of arrival time
  sort(pq.begin(),pq.end());
  
  //Initialisation Loop
  for(i=0;i<N;i++)
  {
    //Threshold is same for all for better simulation
    threshold.pb(40);
    queuePointer.pb(-1);
    vector<int> v;
    processorQueue.pb(v);
  }
  
  int processesScheduled=0; // index into the pq vector which stores the contents of file
  int maxTimer=0;
  while(processesScheduled<pq.size()||timer<=maxTimer) // while all processes have not stopped running
  {
    // check if any processes arrive at this time and put them in queue
    while(processesScheduled<pq.size())
    {
      if(pq[processesScheduled].arrival_time==timer) // if current time is the arrival time of the process
      {
        int processor = pq[processesScheduled].affinity; // allocate processor based on affinity
        if(processor==-1)
        {
          processor = rand()%N;
        }
        
        updateAllNodes(pq[processesScheduled].arrival_time); // update all the processors current executing process
        processMap[pq[processesScheduled].pid]=pq[processesScheduled];
        allocateProcess(pq[processesScheduled].pid, processor);// allocate the process to processor
        
        maxTimer = max(maxTimer, processMap[pq[processesScheduled].pid].start_time+pq[processesScheduled].burst_time);
        processesScheduled++;
      }
      else
        break;
    }

    updateAllNodes(timer);

    int lowestOccupied = getUnderloadedNode(); // get most underloaded node
    int highestOccupied = getOverloadedNode(); // get most overloaded node
    
    int pid = getLastProcess(highestOccupied); // get last scheduled process from most overloaded node. -1 if no such process exists
    
    if(pid!=-1&&highestOccupied!=lowestOccupied){ // if we have a valid process which needs to be rescheduled
      
      // remove process from most overloaded node
      processorQueue[highestOccupied].pop_back();
      // allocate the process to most underloaded node
      allocateProcess(pid, lowestOccupied);
      // update all the processors
      updateAllNodes(timer);
    }
    if(highestOccupied==lowestOccupied){
      pid=-1;
    }
    
    // print the gaant chart for all the processors
    printGaantChart(pid);
    sleep(1);
    timer++;
  }
  
}