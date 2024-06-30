#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int frameRef[1000];
int frame[100];
int frameTotal;
int frameCount[100];

typedef struct{
int stack[100];
int sp;
}stacktype;

stacktype dt;

void fifo();
void lru1();
void lru2();
void fifo_2chance();
void lru_2chance();
void showbinary(int bi);

void separateFrameInfo(int f[],const char *st);
void displaySequence(int f[],int count);
void displayFrame(int f[],int count,int flag);
void displayFrame2(int f[],int count,int flag);

void stackInit(stacktype *s);
int push(stacktype *s,int *data);
int pop(stacktype *s,int *data);
int popdata(stacktype *s,int *data);
void queueInit(stacktype *s);
int enqueue(stacktype *s,int *data);
int dequeue(stacktype *s,int *data);
void showStack(stacktype *s);
void showQueue(stacktype *s);

int findMin(int data[],int count);
int findMinZero(int data[],int count);
int find2chance(int data[],int count,int start);

int main(){
    char frameString[2048];
    char ch;

	stackInit(&dt);

	printf("PAGE REPLACEMENT ALGORITHMS\n");
	printf("===========================\n");
	printf("Please enter frame sequence (please separate each one with comma)\n");
	fgets(frameString,2048,stdin);
	separateFrameInfo(frameRef,frameString);

	for(int i=0;i<10;i++)frame[i]=-1;
	printf("Please enter number of frame allocated by OS :");
	fgets(frameString,2048,stdin);
    frameTotal = atoi(frameString);

    printf("Please select replacement algorithm\n");
	printf("1:First-In-First-Out algorithm (FIFO)\n");
	printf("2:Leased Recently Used algorithm (LRU), using counter\n");
	printf("3:Leased Recently Used algorithm (LRU), using stack\n");
	printf("4:First-In-First-Out with second chance\n");
	printf("5:LRU-Approximation algorithm , Second chance algorithm\n");
	ch = _getch();

	switch(ch){
		case '1': fifo();break;
		case '2': lru1();break;
		case '3': lru2();break;
		case '4': fifo_2chance();break;
		case '5': lru_2chance();break;
		default : printf("exiting without doing anything...\n");
	}
	return 0;
}

void separateFrameInfo(int f[],const char *st){
    int c=0;
	int cc;
	char stemp[40];
	int fcount=0;

	cc=0;
	while(st[c]!=0){

		 stemp[cc++]=st[c++];
		 if(stemp[cc-1]<=','){
			stemp[cc-1]=0;
			if(cc<=1) continue;
			f[fcount++]=atoi(stemp);
			cc=0;
		 }
	}
	f[fcount]=-1;
}

void displaySequence(int f[],int count){
    int c;

	printf("Reference String:\n");
	for(c=0;f[c]>=0;c++)
		printf(" %2d",f[c]);
	printf("\n");
	for(c=0;f[c]>=0;c++)
		if(c==count)
		     printf(" ==");
		else
			 printf("   ");
	printf("\n");
}

void displayFrame(int f[],int count,int flag){
    int c;

	if (flag) {
		if (flag > 1)
			printf("Reference Bit\n");
		else
			printf("Frame Allocation:\n");
	}
	else
		printf("Frame Count:\n");
	printf("_");
	for(c=0;c<count;c++)
		printf("________|");
	printf("\n");
	printf("|");
	for(c=0;c<count;c++)
		if(f[c]<0) printf("        |");
		else  printf("%8d|",f[c]);
	printf("\n");
	printf("_");
	for(c=0;c<count;c++)
		printf("________|");
	printf("\n");
	printf(" ");
	for(c=0;c<count;c++)
		printf("%8d ",c);
	printf("\n");
}

void displayFrame2(int f[],int count,int flag){
    int c;

	if(flag)
		printf("Frame Allocation:\n");
	else
		printf("Frame Count:\n");
	printf("_");
	for(c=0;c<count;c++)
		printf("________|");
	printf("\n");
	printf("|");
	for(c=0;c<count;c++)
		if(f[c]<0) printf("        |");
		else{
			showbinary(f[c]);
			printf("|");
		}
	printf("\n");
	printf("_");
	for(c=0;c<count;c++)
		printf("________|");
	printf("\n");
	printf(" ");
	for(c=0;c<count;c++)
		printf("%8d ",c);
	printf("\n");
}

void stackInit(stacktype *s){
   s->sp=0;
}

int push(stacktype *s,int *data){
   if(s->sp>=100) return 0;
   s->stack[s->sp]=*data;
   s->sp++;
   return 1;
}

int pop(stacktype *s,int *data){
   if(s->sp<=0) return 0;
   s->sp--;
   *data = s->stack[s->sp];
   return 1;
}

int popdata(stacktype *s,int *data){
   int i=0;
   if(s->sp<=0) return 0;
   for(i=0;(i<s->sp)&&(s->stack[i]!=*data);i++);

   if(i >= s->sp) return 0; // Data not found

   for(i++;i<s->sp;i++)
		s->stack[i-1]=s->stack[i];
   s->sp--;
   return 1;
}

void queueInit(stacktype *s){
   s->sp=0;
}

int enqueue(stacktype *s,int *data){
   if(s->sp>=100) return 0;
   s->stack[s->sp]=*data;
   s->sp++;
   return 1;
}
int dequeue(stacktype *s,int *data){
	int i;
	if(s->sp<=0) return 0;
   *data = s->stack[0];
    for(i=1;i<s->sp;i++)
		s->stack[i-1]=s->stack[i];
	s->sp--;
	return 1;
}

int findMin(int data[],int count){
   int x;
   int min=0;

   for(x=1;x<count;x++){
        if(data[x]<data[min])
			min=x;
   }
   return min;
}

int findMinZero(int data[],int count){
   int x;
   int min=0;

   if(data[0]==0) return 0;

   for(x=1;x<count;x++){
        if(data[x]<data[min])
			min=x;
		if(min==0)break;
   }
   return min;
}

int find2chance(int data[],int count,int start){
   int x=start;

   while(data[x]){
	   if(data[x]){
			data[x]=0;
			x = (x+1)%count;
	   }
	   displayFrame(frameCount, frameTotal, 2);
	   printf("Current victim pointer is at: %d\n", x);
   }
   return x;
}

void showStack(stacktype *s){
   printf("STACK LIST >>(BOTTOM)");
   int i;

   for(i=0;i<s->sp;i++)
	   printf(" %2d ",s->stack[i]);

   printf("(TOP)\n");
}

void showQueue(stacktype *s){
   printf("QUEUE LIST >>(HEAD)");
   int i;

   for(i=0;i<s->sp;i++)
	   printf(" %2d ",s->stack[i]);

   printf("(TAIL)\n");
}
////////////////////////////////////////////////////////

void fifo(){
   int round,i;
   int numFrame=0;
   int victim;

   displaySequence(frameRef,-1);
   displayFrame(frame,frameTotal,1);
   showQueue(&dt);

   for(round=0;frameRef[round]>=0;round++){

		if(numFrame<frameTotal){ //free frame available
		    for(i=0;(frame[i]>=0)&&(frame[i]!=frameRef[round]);i++);
			frame[i]=frameRef[round];
			if(i==numFrame){
				enqueue(&dt,&frameRef[round]);
				numFrame++;
			}
		}else{  // Page replacement using FIFO
			// Search if the page already in the frame list
		    for(i=0;(i<frameTotal)&&(frame[i]!=frameRef[round]);i++);

			if(i==frameTotal){ // Frame does not exists
				dequeue(&dt,&victim);
				for(i=0;(i<frameTotal)&&(frame[i]!=victim);i++);
				if(i==frameTotal) printf("Error occured\n");
				else{
					frame[i]=frameRef[round];
					enqueue(&dt,&frameRef[round]);
				}
			}
		}
		displaySequence(frameRef,round);
		displayFrame(frame,frameTotal,1);
	    showQueue(&dt);
		_getch();
   }
}

void lru1(){
   int round,i;
   int numFrame=0;
   int victim;

   for(i=0;i<frameTotal;i++)
	   frameCount[i]=0; // Clear frame count

   displaySequence(frameRef,-1);
   displayFrame(frame,frameTotal,1);
   displayFrame(frameCount,frameTotal,0);

   for(round=0;frameRef[round]>=0;round++){

		if(numFrame<frameTotal){ //free frame available
		    for(i=0;(frame[i]>=0)&&(frame[i]!=frameRef[round]);i++);
			frame[i]=frameRef[round];
			frameCount[i]=clock(); // Renew time stamp
			if(i==numFrame){
				numFrame++;
			}
		}else{  // Page replacement using LRU using timestamp
			// Search if the page already in the frame list
		    for(i=0;(i<frameTotal)&&(frame[i]!=frameRef[round]);i++);

			if(i==frameTotal){ // Frame does not exists
				victim = findMin(frameCount,frameTotal);
					frame[victim]=frameRef[round];
					frameCount[victim]=clock();
			}else{
				// Access the exising frame
				frameCount[i]=clock(); // Renew time stamp
			}
		}
		displaySequence(frameRef,round);
		displayFrame(frame,frameTotal,1);
		displayFrame(frameCount,frameTotal,0);
		_getch();
   }
}

void lru2(){
   int round,i;
   int numFrame=0;
   int victim;

   displaySequence(frameRef,-1);
   displayFrame(frame,frameTotal,1);
   showStack(&dt);

   for(round=0;frameRef[round]>=0;round++){

		if(numFrame<frameTotal){ //free frame available
		    for(i=0;(frame[i]>=0)&&(frame[i]!=frameRef[round]);i++);
			frame[i]=frameRef[round];
			if(i==numFrame){
				push(&dt,&frameRef[round]);
				numFrame++;
			}else{
			// Page exists, move data in stack
				popdata(&dt,&frameRef[round]);
				push(&dt,&frameRef[round]);
			}
		}else{  // Page replacement using FIFO
			// Search if the page already in the frame list
		    for(i=0;(i<frameTotal)&&(frame[i]!=frameRef[round]);i++);

			if(i==frameTotal){ // Frame does not exists
				dequeue(&dt,&victim); // Move data from bottom of stack
				for(i=0;(i<frameTotal)&&(frame[i]!=victim);i++);
				if(i==frameTotal) printf("Error occured\n");
				else{
					frame[i]=frameRef[round];
					push(&dt,&frameRef[round]);
				}
			}else{
			// Page exists, move data in stack
				popdata(&dt,&frameRef[round]);
				push(&dt,&frameRef[round]);
			}
		}
		displaySequence(frameRef,round);
		displayFrame(frame,frameTotal,1);
		showStack(&dt);
		_getch();
   }
}

void fifo_2chance() {
	int round, i;
	int numFrame = 0;
	int victim = 0;

	for (i = 0; i < frameTotal; i++)
		frameCount[i] = 0; // Clear frame count

	displaySequence(frameRef, -1);
	displayFrame(frame, frameTotal, 1);
	displayFrame(frameCount, frameTotal, 2);
	showQueue(&dt);

	for (round = 0; frameRef[round] >= 0; round++) {

		if (numFrame < frameTotal) { //free frame available
			for (i = 0; (frame[i] >= 0) && (frame[i] != frameRef[round]); i++);
			frame[i] = frameRef[round];
			frameCount[i] = 1;
			if (i == numFrame) {
				enqueue(&dt, &frameRef[round]);
				numFrame++;
			}
		}
		else {  // Page replacement using FIFO with second-change
				// Search if the page already in the frame list
			for (i = 0; (i < frameTotal) && (frame[i] != frameRef[round]); i++);

			if (i == frameTotal) { // Frame does not exists
								   // Perform second-change algorithm
				do {
					dequeue(&dt, &victim);
					// search victim location
					for (i = 0; i < frameTotal; i++)
						if (frame[i] == victim) break;
					if (frameCount[i] > 0) {
						// give the victim a second-chance
						frameCount[i] = 0;
						enqueue(&dt, &victim);
						displayFrame(frameCount, frameTotal, 2);
						showQueue(&dt);
					}
					else {
						// This page is the victim
						frameCount[i] = 1;
						frame[i] = frameRef[round];
						enqueue(&dt, &frameRef[round]);
						break;
					}
				} while (1);
			}
			else {
				// Access the exising frame
				frameCount[i] = 1;
				victim = frame[i];
			}
		}
		displaySequence(frameRef, round);
		displayFrame(frame, frameTotal, 1);
		displayFrame(frameCount, frameTotal, 2);
		showQueue(&dt);
		_getch();
	}
}


void lru_2chance(){
   int round,i;
   int numFrame=0;
   int victim=0;

   for(i=0;i<frameTotal;i++)
	   frameCount[i]=0; // Clear frame count

   displaySequence(frameRef,-1);
   displayFrame(frame,frameTotal,1);
   displayFrame(frameCount,frameTotal,2);

   for(round=0;frameRef[round]>=0;round++){

		if(numFrame<frameTotal){ //free frame available
		    for(i=0;(frame[i]>=0)&&(frame[i]!=frameRef[round]);i++);
			frame[i]=frameRef[round];
			frameCount[i] = 1;
			if(i==numFrame){
				numFrame++;
			}
		}else{  // Page replacement using LRU using timestamp
			// Search if the page already in the frame list
		    for(i=0;(i<frameTotal)&&(frame[i]!=frameRef[round]);i++);

			if(i==frameTotal){ // Frame does not exists
				victim = find2chance(frameCount,frameTotal,victim);
					frame[victim]=frameRef[round];
					frameCount[victim] = 1;
			}else{
				// Access the exising frame
				frameCount[i] =1;
			}
		}
		displaySequence(frameRef,round);
		displayFrame(frame,frameTotal,1);
		displayFrame(frameCount,frameTotal,2);
	    printf("Current victim pointer is at: %d\n",victim);
		_getch();
   }
}

void showbinary(int bi){
	int i;

	for(i=7;i>=0;i--){
		if((bi>>i)&1)putchar('1');
		else putchar('0');
	}
}
