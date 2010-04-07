#include "stdafx.h"
#include <stdlib.h>   
#include <windows.h>   
#include <stdio.h>   
#include <malloc.h>   
#include <time.h>
#include <Shellapi.h>
#include <string.h>
#include <iostream>


using namespace std;

   
#define random(x) (rand()%x) //rand number   
#define bufferSize 5         //size of buffer   
   
DWORD WINAPI Producer(LPVOID);// producer thread   
DWORD WINAPI Consumer(LPVOID);//consumer thread   
   
int insert_item(int item);    //insert product to buffer   
int remove_item(int *item);   //remove product from buffer   
   
HANDLE Mutex;                 //Semaphore Mutex,to ensure mutual excute   
HANDLE Full, Empty;           //Semaphore Full,to indicate the buffer HAS product or not   
                              //Semaphore Empty,to indicate total number of products in buffer   
int product[bufferSize-1];    //act as buffer,to save products   
   
int i,j;                      //counters   
int ip=0,ic=0,ipN,icN;        //ip: The id of Producer threads   
                              //ic: The id of Consumer threads   
                              //ipN: The total number of Producer threads   
                              //icN: The total number of Consumer threads   
int sleepTime;                //sleepTime: time for main function to sleep before termination    
int randNum;                  //to save the value of product   
   
int buffer=0;                 //to remeber total number of products in buffer   
char openrtspCommand[300],curlCommand[300],delCommand[300];
char sourceurl[50],desturl[50],vid[10],bid[5],duration[5];
  
//using namespace std;

//int main()//main   
int main(int argc, char* argv[])//main 
{   


	strcpy(sourceurl,argv[1]);
	strcpy(desturl,argv[2]);
	strcpy(vid,argv[3]);
	strcpy(bid,argv[4]);
	strcpy(duration,argv[5]);

	printf("\n Source  = %s\n Dest	 = %s\n vid	 = %s\n bid	 = %s\n durat   = %s\n",sourceurl, desturl, vid, bid,duration);
	

	strcpy(openrtspCommand,"F:\\Desktop\\RAYS\\live555\\live\\testProgs\\openrtsp -v -4 -d ");
	strcat(openrtspCommand,duration);	
	strcat(openrtspCommand," -t ");	
	strcat(openrtspCommand,sourceurl);	
	strcat(openrtspCommand,"1>F:\\");	

	printf(" openrtsp -> %s\n",openrtspCommand);
	
	strcpy(curlCommand,"F:\\Desktop\\RAYS\\curl\\curl -F \"F1=@F:\\temp10.mp4\" -F \"requestType=put\" -F \"videoID=");
	strcat(curlCommand,vid);
	strcat(curlCommand,"\" -F \"blockID=");
	strcat(curlCommand,bid);
	strcat(curlCommand,"\" ");
	strcat(curlCommand,desturl);
	
	printf(" openrtsp -> %s\n",openrtspCommand);
	printf(" curl -> %s\n",curlCommand);

	//printf("How long to sleep before terminating?\n");   
	//scanf("%d",&sleepTime);   
	sleepTime=100;
	//printf("The number of Producer threads?\n");        
	//scanf("%d",&ipN); 
	ipN=1;   
	//printf("The number of Consumer threads?\n");        
	//scanf("%d",&icN); 
	icN=1;  
	printf("\n\n");   
	   
	Mutex=CreateMutex(NULL,FALSE,NULL);               //Create Semaphores   
	Full=CreateSemaphore(NULL,0,bufferSize,NULL);   
	Empty=CreateSemaphore(NULL,bufferSize,bufferSize,NULL);   
	   
	HANDLE p[1],c[1];                         //Create threads: p[]:producer;c[]:consumer    
	   
	for(ip=0;ip<=ipN-1;ip++)   
	{   
		p[ip]=CreateThread(NULL,0,Producer,NULL,ip,NULL);    
		printf("PRODUCER created \n\n",ip+1);   
	       
	       
		if (p[ip]==NULL) return -1;  //If fail,terminate   
	}   
	for(ic=0;ic<=icN-1;ic++)   
	{   
		c[ic]=CreateThread(NULL,0,Consumer,NULL,ic,NULL);    
		printf("CONSUMER created \n\n",ic+1);       
	       
		if (c[ic]==NULL) return -1; //If fail,terminate   
	}   
	   
	Sleep(sleepTime);   //sleep~   
	   
	system("pause");   
	return 0;   
}   
   
   
   
DWORD WINAPI Producer(LPVOID lpPara) //Producer threads   
{   
while(TRUE){   
       
    WaitForSingleObject(Empty,INFINITE); //if kernal is free && buffer is not full   
    WaitForSingleObject(Mutex,INFINITE); //START PRODUCING   
       
    srand(time(NULL)+i);                 //put out a random number as a product    
    randNum=random(10000000);   
    int Presult;   
    Presult=insert_item(randNum);       //insert product to buffer
	//wstring ss="open";
	
	//ShellExecute( NULL, _T("open"), _T("C:\\WINDOWS\\system32\\cmd.exe"),_T("ipconfig"), _T("C:\\"), SW_SHOWNORMAL);
	
	printf(" and again openrtsp -> %s\n",openrtspCommand);
	char currentCommand[300];
	strcpy(currentCommand,openrtspCommand);
	strcat(currentCommand,vid);
	strcat(currentCommand,"_");
	strcat(currentCommand,bid);
	strcat(currentCommand,".mp4");


	system(currentCommand);
	
	/*
	ShellExecute( 
		NULL,		// 
		"open",		//
		"C:\\WINDOWS\\system32\\cmd.exe", //
		"F:\Desktop\RAYS\curl>openrtsp -v -4 -d 5 rtsp://128.125.91.194/img/video.sav 1>temp10.mp4", //
		NULL,		//
		SW_SHOWNORMAL // 
		);
*/
    if(Presult==1)                      //sucessful   
    {   
       printf("Producer produced %d \t buffer now: %d/5 \n\n",randNum,buffer);   
       if(buffer==bufferSize) printf("\t buffer is full,please wait\n\n");   
       ReleaseMutex(Mutex);   
       ReleaseSemaphore(Full,1,NULL);    
    }   
   
    else if(Presult==0)                 //error   
    {   
       printf("produce failed.Report error condition\n");   
       ReleaseMutex(Mutex);   
    }   
   
    i++;  /*i: the function time() output current time every second  
               but the threads run-over-time is less than 1s  
               so set this variable to ensure the value won't be same each time*/   
    Sleep(10000);//sleep   
    }   
   
return 0;   
}   
   
   
DWORD WINAPI Consumer(LPVOID lpPara)//consumer threads   
{   
while(TRUE){   
       
       
    WaitForSingleObject(Full,INFINITE);  //if kernal is free && buffer is not empty   
    WaitForSingleObject(Mutex,INFINITE); //START CONSUMING   
       
    int num;  
    int result=remove_item(&num);        //get product from buffer and delete it   
    //system("F:\\Desktop\\RAYS\\curl\\curl -F \"F1=@F:\\Documents\\Visual Studio 2008\\Projects\\ProducerConsumer\\ProducerConsumer\\temp11.mp4\" -F \"requestType=put\" -F \"videoID=99901\" -F \"blockID=1\" http://localhost/upload.php");   
	
	strcpy(curlCommand,"F:\\Desktop\\RAYS\\curl\\curl -F \"F1=@F:\\");
	strcat(curlCommand,vid);
	strcat(curlCommand,"_");
	strcat(curlCommand,bid);
	strcat(curlCommand,".mp4\"");
	strcat(curlCommand," -F \"requestType=put\" -F \"videoID=");
	strcat(curlCommand,vid);
	strcat(curlCommand,"\" -F \"blockID=");
	strcat(curlCommand,bid);
	strcat(curlCommand,"\" ");
	strcat(curlCommand,desturl);
	
	
	printf("\n\n and again curl -> %s\n",curlCommand);
	system(curlCommand);

	strcpy(delCommand,"DEL /F F:\\");
	strcat(delCommand,vid);
	strcat(delCommand,"_");
	strcat(delCommand,bid);
	strcat(delCommand,".mp4");
	printf("\n\n Deleting -> %s\n",delCommand);
	system(delCommand);

	strcpy(bid,itoa(atoi(bid)+1,bid,10));

	cout<<"==========================\n\n\nbid "<<bid<<endl<<endl;

	if(result>0)//remove successful   
    {   
       printf("Consumer consumed %d \t buffer now: %d/5 \n\n",product[buffer],buffer);   
       product[buffer]=0;   
       if(buffer==0) printf("\t buffer is empty,please wait\n\n");   
          
       ReleaseMutex(Mutex);   
       ReleaseSemaphore(Empty,1,NULL);   
       Sleep(2000);   
    }   
   
    else//error   
    {    
       printf("comsume failed.Report error condition\n");   
       ReleaseMutex(Mutex);   
    }   
}   
return 0;   
}   
   
int insert_item(int item)//insert product to buffer   
{      
    int n;   
    if(buffer<=bufferSize)//put in   
    {   
        product[buffer]=item;   
        buffer++;   
        return 1;   
    }   
   
    else return 0;//error   
 }   
    
int remove_item(int *item)//remove product to buffer   
{   
    if(buffer<=bufferSize)//move out   
    {   
        item=& product[buffer];   
        product[buffer]=0;   
        buffer--;   
        return (int)&item;   
    }   
    else return 0;//error   
}   