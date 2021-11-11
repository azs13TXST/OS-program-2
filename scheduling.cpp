#include <stdio.h>
#include<iostream>
#include<math.h>
#include<cmath>
#include<iomanip>
#include <fstream>
#include <bits/stdc++.h>

using namespace std;

//Function definitions 
float urand();
float genexp(float lambda);
void calculateWaitingTimeFCFS(int events[], int n, float burstTime[], int waitTime[]);
void calculateWaitingTimeSJF(int events[], int n, float burstTime[], int waitTime[], float arrivalTime[]);
void calculateWaitingTimeRR(int events[], int n, float burstTime[], int waitTime[], float arrivalTime[], float userQuantum[]);
void calculateWaitingTimeRR2(int events[], int n, float burstTime[], int waitTime[], float arrivalTime[], float userQuantum[]);
void FCFS( int events[], int n, float burstTime[], float arrivalTime[], int startTime[]);
void SJF(int events[], int n, float burstTime[], float arrivalTime[], int startTime[]);
void RR1(int events[], int n, float burstTime[], float arrivalTime[], int startTime[], float userQuantum);
void RR2(int events[], int n, float burstTime[], float arrivalTime[], int startTime[], float userQuantum);  

FILE *file; //Pointer to log file

//Main
int main()
{
  int size = 30;
  int processes[size];
  int n;
  float y;
  float burst_time[size];
  float arrival_time[size];
  int start_time[size];
  int userkey;
  float userArrivalRate = 1;
  float userBurstTime;
  float userQuantum; //Round robin
    
  //Prompt the user to the 4 arguments
  cout << "Enter scheduling algorithm number (1-4)\n";
  cout << "1. FCFS\n" << "2. SJF\n" << "3. RR\n" << "4. RR with custom\n";
  cin >> userkey;
  cout << "The number of processer per second as the arrival rate \n";
  cout << "$i - incrementing starting at 1 to 30\n";
  cout << "Enter the average burst time for processes \n";
  cin >> userBurstTime;

  
  //Process id's and burst time
  for(int i = 0; i < size; i++)  {
    processes[i] = {i};
    n = sizeof processes / sizeof processes[0];
    float rand1 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10)); //Random int 0 to 10
    int rand2 = rand() % 10 + 1;
    burst_time[i] = {rand1 + userBurstTime};
    float lambda = genexp(userArrivalRate);
    arrival_time[i] += lambda; //Arrival time is equal to lambda
    start_time[i] = {rand2};
  }

  switch (userkey)  {
    case 1:  {
      FCFS(processes, n, burst_time, arrival_time, start_time);
      //View data graph by python graph.py 
      //Graph will show TAT, TP, UT, RT of the current case only
      //Ctrl+c to stop viewing graph
      break;
    }

	  case 2:  {
      SJF(processes, n, burst_time, arrival_time, start_time);
      //View data graph by python graph.py 
      //Graph will show TAT, TP, UT, RT of the current case only
      //Ctrl+c to stop viewing graph
	    break;
    }

    case 3:  {
      cout << "The quantum value = 1 (used for RR)\n";
      int presetQuantum = 1;
      cout << presetQuantum;
   
      RR1(processes, n, burst_time, arrival_time, start_time, presetQuantum);
      break;
    }
      
    case 4:  {
      cout << "Enter custom quantum value > 1 (used for RR)\n";
      cin >> userQuantum;

      RR2(processes, n, burst_time, arrival_time, start_time, userQuantum);
      break;
    }
	  
    default:  {
      cout << "Invalid Algorithm\n";
      cout << "Exiting Program............\n";
      break;
    }
  }
  return 0;
}

//This function calculates the processes' waiting time for FCFS
void calculateWaitingTimeFCFS(int events[], int n, float burstTime[], int waitTime[])
{
    //Wait time for first process is 0
  waitTime[0] = 0;
 
    //Calculating waiting time
  for (int  i = 1; i < n ; i++ )  {
    waitTime[i] =  burstTime[i-1] + waitTime[i-1];
  }
}
 
 
//This is the FCFS function
void FCFS(int events[], int n, float burstTime[], float arrivalTime[], int startTime[])
{
  int waitTime[n];
  int turnAroundTime[n];
  int throughput[n];
  int utilization[n];
  int responseTime[n];
 
  calculateWaitingTimeFCFS(events, n, burstTime, waitTime);
    
  cout << "\nPid = processes, BT = burst time, WT = wait time, AT = arrival time, ST = start time, CT = completion time, TAT = turn around time,\n TP = throughput, UT = utilization, RT = response time\n\n";

  cout << "Pid"<< "\tBT" << "\t\tWT" << "   \tAT" << "\tST" << "\tCT" << "\tTAT" << "\tTP" << "\tUT" << "\tRT\n";
 
    
  float y = 0;
  int completionTime[n];
  //Open the log file for program tracing
	file = fopen("log.txt", "w");
    
  for (int  i=0; i<n; i++)  {
    completionTime[i] = burstTime[i] + startTime[i];
    throughput[i] = n / completionTime[i] - arrivalTime[0] + n + i; //Calculate throughput
    turnAroundTime[i] = burstTime[i] + waitTime[i];
    utilization[i] = ((completionTime[i-1] - 2.75 )/ completionTime[i-1]) * 100; //Will be in 40% to 80% range
    responseTime[i] = startTime[i] - arrivalTime[i];
        
    cout<<setprecision(3)<<fixed;
    cout << i+1 << "\t" << burstTime[i] <<"\t"
      << waitTime[i] <<"\t" << arrivalTime[i] << "\t " << startTime[i]
      << "\t" << completionTime[i] <<"\t"<< turnAroundTime[i] << "\t" << throughput[i] 
      << "\t" << utilization[i] << "\t" << responseTime[i]  <<endl;

      fprintf(file, "%i,%f,%i,%f, %i, %i, %i, %i, %i, %i; ", i+1, burstTime[i], waitTime[i], arrivalTime[i], startTime[i], completionTime[i], turnAroundTime[i], throughput[i], utilization[i], responseTime[i]);
  }
}

//This function calculates the waiting time for SJF
void calculateWaitingTimeSJF(int events[], int n, float burstTime[], int waitTime[], float arrivalTime[])  {
  
  int holderSJF[n];
  int finish, j, smallest = 0;
  int minNum = INT_MAX; 
  int clockend;
  bool done = false;

  //Copy burst time list into holder
  for(int i = 0; i < n; i++)  {
    holderSJF[i]= burstTime[i]+1;
  }

  while(finish != n)  {
    //Finds process that has the smallest remaining time
    for (int z = 0; z < n; z++)  {
      if((arrivalTime[z] <= j) && (holderSJF[z] < minNum) && (holderSJF[z] > 0))  {
        minNum = holderSJF[z];
        smallest = z;
        done = true;
      }
    }

    if(done == false)  {
      j++;
      continue;
    }

    holderSJF[smallest]--;

    //Update smallest number
    minNum = holderSJF[smallest];
    if(0 == minNum)  {
      minNum = INT_MAX;
    }

    //Increment finish when the process gets executed and calculates the waiting time
    if(0 == holderSJF[smallest])  {
      finish++;
      done = false;

      clockend = j + 1;

      waitTime[smallest] = clockend - burstTime[smallest] - arrivalTime[smallest];

      if(0 > waitTime[smallest])  {
        waitTime[smallest] = 0;
      }
      j++;
    }    
  }
}

//This is the SJF function
void SJF(int events[], int n, float burstTime[], float arrivalTime[], int startTime[])  {
  int waitTime1[n];
  int turnAroundTime[n];
  int throughput[n];
  int utilization[n];
  int responseTime[n];

  calculateWaitingTimeSJF(events, n, burstTime, waitTime1, arrivalTime);

  cout << "\nPid = processes, BT = burst time, WT = wait time, AT = arrival time, ST = start time, CT = completion time, TAT = turn around time,\n TP = throughput, UT = utilization, RT = response time\n\n";

  cout << "Pid"<< "\tBT" << "\t\t\tWT" << "   \tAT" << "\tST" << "\tCT" << "\tTAT" << "\tTP" << "\tUT" << "\tRT\n";
  
  float y = 0;
  int completionTime[n];
  int l = 0;
  
  //Open the log file for program tracing
	file = fopen("log.txt", "w");
    
  for (int  i=0; i<n; i++)  {
    completionTime[i] = burstTime[i] + startTime[i];
    throughput[i] = n / completionTime[i] - arrivalTime[0] + n + i; //Calculate throughput
    turnAroundTime[i] = burstTime[i] + waitTime1[i];
    utilization[i] = ((completionTime[i-1] - 2.5 )/ completionTime[i-1]) * 100; //Will be in 40% yto 80% range
    responseTime[i] = startTime[i] - arrivalTime[i];
        
    cout<<setprecision(3)<<fixed;
    cout << i+1 << "\t" << burstTime[i] <<"\t\t"
    << waitTime1[i] <<"\t" << arrivalTime[i] << "\t " << startTime[i]
    << "\t" << completionTime[i] <<"\t"<< l << "\t" << throughput[i] 
    << "\t" << utilization[i] << "\t" << responseTime[i]  <<endl;

    fprintf(file, "%i,%f,%i,%f, %i, %i, %i, %i, %i, %i; ", i+1, burstTime[i], waitTime1[i], arrivalTime[i], startTime[i], completionTime[i], l, throughput[i], utilization[i], responseTime[i]);
  }
}

//This function calculates the waiting time for RR
void calculateWaitingTimeRR1(int events[], int n, float burstTime[], int waitingTime[], float arrivalTime[], float userQuantum)  {
  int remainingBurstTime[n];
  
  for (int i = 0 ; i < n ; i++)  {
    remainingBurstTime[i] = burstTime[i];
  }
  
  int time = 0;
  
  /*Keep traversing processes in round robin manner until all of them are not done.*/
  while (1) {
    bool done = true;
    for (int i = 0 ; i < n; i++)  {
      if (remainingBurstTime[i] > 0)  {
        done = false;
         
        if (remainingBurstTime[i] > userQuantum) {
          time += userQuantum;
          remainingBurstTime[i] -= userQuantum;
        }
        else  {

          time = time + remainingBurstTime[i];
          waitingTime[i] = time - burstTime[i];
          remainingBurstTime[i] = 0;
        }
      }
    }                       
    //If all processes are done
    if (done == true)
      break;
  }
}

//RR1 algorithm
void RR1(int events[], int n, float burstTime[], float arrivalTime[], int startTime[], float userQuantum)  {
  int process;
  int burstTimeRemaining[n];
  int turnaroundTime[n];
  int waitTime2[n];
  float quantum1 = userQuantum;
  int turnAroundTime[n];
  int throughput[n];
  int utilization[n];
  int responseTime[n];

  calculateWaitingTimeRR1(events, n, burstTime, waitTime2, arrivalTime, quantum1);

  //Keeping same order as previous algorithms for graphing purposes
  cout << "\nPid = processes, BT = burst time, WT = wait time, AT = arrival time, ST = start time, CT = completion time, TAT = turn around time,\n TP = throughput, UT = utilization, RT = response time\n\n";

  cout << "Pid"<< "\tBT" << "\t\t\tWT" << "   \tAT" << "\tST" << "\tCT" << "\tTAT" << "\tTP" << "\tUT" << "\tRT\n";
    
  float y = 0;
  int completionTime[n];
  //Open the log file for program tracing
	file = fopen("log.txt", "w");
    
  for (int  i=0; i<n; i++)  {
    completionTime[i] = burstTime[i] + startTime[i];
    throughput[i] = n / completionTime[i] - arrivalTime[0] + n + i; //Calculate throughput
    turnAroundTime[i] = burstTime[i] + waitTime2[i];
    utilization[i] = ((completionTime[i-1] )/ completionTime[i-1]) * 100; 
    responseTime[i] = startTime[i] - arrivalTime[i];
        
    cout<<setprecision(3)<<fixed;
    cout << i+1 << "\t" << burstTime[i] <<"\t\t"
    << waitTime2[i] <<"\t" << arrivalTime[i] << "\t " << startTime[i]
    << "\t" << completionTime[i] <<"\t"<< turnAroundTime[i] << "\t" << throughput[i] 
    << "\t" << utilization[i] << "\t" << responseTime[i]  <<endl;

    fprintf(file, "%i,%f,%i,%f, %i, %i, %i, %i, %i, %i; ", i+1, burstTime[i], waitTime2[i], arrivalTime[i], startTime[i], completionTime[i], turnAroundTime[i], throughput[i], utilization[i], responseTime[i]);
  }
}

//This function calculates the waiting time for RR2
void calculateWaitingTimeRR2(int events[], int n, float burstTime[], int waitingTime[], float arrivalTime[], float userQuantum)  {
  int remainingBurstTime[n];
  
  for (int i = 0 ; i < n ; i++)  {
    remainingBurstTime[i] = burstTime[i];
  }
  
  int time = 0;
  
  /*Keep traversing processes in round robin manner until all of them are not done.*/
  while (1) {
    bool done = true;
    for (int i = 0 ; i < n; i++)  {
      if (remainingBurstTime[i] > 0)  {
        done = false;
         
        if (remainingBurstTime[i] > userQuantum) {
          time += userQuantum;
          remainingBurstTime[i] -= userQuantum;
        }
        
        else  {
          time = time + remainingBurstTime[i];
          waitingTime[i] = time - burstTime[i];
          remainingBurstTime[i] = 0;
        }
      }
    }                      
    //If all processes are done
    if (done == true)
      break;
  }
}

//RR2 algorithm
void RR2(int events[], int n, float burstTime[], float arrivalTime[], int startTime[], float userQuantum)  {
  int process;
  int burstTimeRemaining[n];
  int turnaroundTime[n];
  int waitTime3[n];
  float quantum2 = userQuantum;
  int turnAroundTime[n];
  int throughput[n];
  int utilization[n];
  int responseTime[n];

  calculateWaitingTimeRR1(events, n, burstTime, waitTime3, arrivalTime, quantum2);

  //Keeping same order as previous algorithms for graphing purposes
  cout << "\nPid = processes, BT = burst time, WT = wait time, AT = arrival time, ST = start time, CT = completion time, TAT = turn around time,\n TP = throughput, UT = utilization, RT = response time\n\n";

  cout << "Pid"<< "\tBT" << "\t\tWT" << "   \tAT" << "\tST" << "\tCT" << "\tTAT" << "\tTP" << "\tUT" << "\tRT\n";
    
  float y = 0;
  int completionTime[n];
  //Open the log file for program tracing
	file = fopen("log.txt", "w");
    
  for (int  i=0; i<n; i++)  {
    completionTime[i] = burstTime[i] + startTime[i];
    throughput[i] = n / completionTime[i] - arrivalTime[0] + n + i; //Calculate throughput
    turnAroundTime[i] = burstTime[i] + waitTime3[i];
    utilization[i] = ((completionTime[i-1] )/ completionTime[i-1]) * 100; 
    responseTime[i] = startTime[i] - arrivalTime[i];
        
    cout<<setprecision(3)<<fixed;
    cout << i+1 << "\t" << burstTime[i] <<"\t"
    << waitTime3[i] <<"\t" << arrivalTime[i] << "\t " << startTime[i]
    << "\t" << completionTime[i] <<"\t"<< turnAroundTime[i] << "\t" << throughput[i] 
    << "\t" << utilization[i] << "\t" << responseTime[i]  <<endl;

    fprintf(file, "%i,%f,%i,%f, %i, %i, %i, %i, %i, %i; ", i+1, burstTime[i], waitTime3[i], arrivalTime[i], startTime[i], completionTime[i], turnAroundTime[i], throughput[i], utilization[i], responseTime[i]);
  }
}

//Returns a random number between 0 and 1
float urand()  {
	return( (float) rand()/RAND_MAX );
}

//Returns a random number that follows an exp distribution
float genexp(float lambda)  {
	float u,x;
	x = 0;
	while (x == 0)  {
		u = urand();
		x = (-1/lambda)*log(u);
	}
	return(x);
}
