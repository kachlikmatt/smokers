// smokers.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
/* Author: S. Renk */
/* CSC 420/520 */
/* Parallel Cigarette-Smoker Problem */
// include headers
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>
using namespace std;


void agent();
void smoker(int);


// shared vars for the smokers
enum itemState {EMPTY};
enum iState {PAPER, MATCHES, TOBACCO};
enum sState {GONE =0 , PRESENT, CRAVING, SMOKING, DONE};

int item[3] = {EMPTY}; /* holds iState */
int smokerCount = 0;
int smokerState[3] = {GONE};
int numTimesSmoked[3] = {0,0,0};

// locks to create mutual exclusion
HANDLE mutex=CreateMutex(NULL, FALSE, NULL);
HANDLE agentReady=CreateEvent(NULL, TRUE,FALSE, NULL);
HANDLE moreItems = CreateEvent(NULL,TRUE,FALSE,NULL);

// any others you need

// ********** Create 3 smokers and an agent & set them smoking ****************
void main()
{ // Set up 4 threads
  HANDLE sThread[4];                  // 4 smokers
  DWORD  sThreadID[4];                // PID of thread
  DWORD WINAPI proc(LPVOID);          // code for the 4 smokers

  // seed rand # generator
  //srand (time(0));   // uncomment this when you have it running

  // start 4 threads
  for(int smokerNbr = 0; smokerNbr < 4; smokerNbr++)
  {  
	  sThread[smokerNbr]=CreateThread(NULL,0,proc,NULL,0,&sThreadID[smokerNbr]);
  }

  WaitForMultipleObjects(4, sThread, true, INFINITE); // wait for threads to finish
  cout << "press CR to end."; while (_getch()!='\r');
  return;
}



DWORD WINAPI proc(LPVOID)    // forman/worker solution rather than peer2peer
{ int myID;

   WaitForSingleObject(mutex, INFINITE);   // lock the lock
       myID = smokerCount++;
   ReleaseMutex(mutex);                    // unlock the lock
   smokerState[myID] = PRESENT;

  if (myID == 3)
  {    cout << "\n Calling Agent \n"; 
       agent();
  }
  else
  {   cout <<"\n Calling Smoker \n ";
      smokerState[myID]=CRAVING;
	  smoker(myID);
  }
  return 0;
}


void agent()
{ /* wait for all three smokers to report */
		  

  while(smokerCount < 3)Sleep(0);
  int placed = 0;
  int spot = 0;
  while(smokerCount > 1)
  {
	 
	  
		int randomNumber = rand()%3;
		//check if the smoker is still there
		while(smokerState[randomNumber] == DONE)
			randomNumber = rand()%3;
		WaitForSingleObject(mutex, INFINITE);   // lock the lock
		item[(randomNumber+1)%3]++;
		item[(randomNumber+2)%3]++;
		cout<<"items counts:"<<item[0]<<" "<<item[1]<<" "<<item[2]<<endl;
		ReleaseMutex(mutex); 

		while(item[0] == 1 || item[1] == 1 || item[2] == 1)
			Sleep(0);
		
		
  }
}
  


//looking for myID +1%3, myID+2%3
void smoker(int myID)
{
	while(numTimesSmoked[myID] < 100000)
	{
		smokerState[myID] = CRAVING;

		if(item[myID] != 1 && item[(myID+1)%3] == 1 && item[(myID+2)%3] ==1)
		{
			WaitForSingleObject(mutex, INFINITE);   // lock the lock
			item[(myID+1)%3] --;
			item[(myID+2)%3] --;
			numTimesSmoked[myID] ++;
			if(numTimesSmoked[myID] == 100000)
			{
				smokerCount --;
				smokerState[myID] = DONE;
			}
			else
			{
				smokerState[myID] = SMOKING;
			}

			ReleaseMutex(mutex);                    // unlock the lock

			//checks to make sure correct items are taken
			WaitForSingleObject(mutex, INFINITE);   // lock the lock
			cout<<"smoker "<<myID<<" smoked " << numTimesSmoked[myID]<<endl;

			ReleaseMutex(mutex);                    // unlock the lock
		

			 Sleep(rand()%10);
		}
		else
		{
			Sleep(0);
		}
	}
  // check for yor items & smoke when you can

	
}


