#include "linklayer.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define APP_START 0x02;
#define APP_END 0x03;
#define C_DATA 0x01;
#define T_SIZE 0x00; //Utilizar outros valores caso queiramos (nome, etc...)
#define APP_MAX_SIZE 1024;
#define TRANSMITTER 1;

#define DEBUG 1

//[IMPORTANTE] em linklayer.c, não deviam ser unsigned chars em vez de int??

struct applicationLayerstruct{
	
	int fileDescriptor;
	int status;
	//unsigned int sequenceNumber;  //Deixar como unsigned int??
	unsigned char sequenceNumber;
	unsigned char app_frame[APP_MAX_SIZE];
	
}applicationLayer;

int sendAppControlFrame(unsigned char C,/*unsigned char* V*/int V, unsigned char L){
	
	applicationLayer.app_frame[0] = C;
	applicationLayer.app_frame[1] = T_SIZE;
	applicationLayer.app_frame[2] = L;
	memcpy(applicationLayer.app_frame+3, &V, L); //ERRO POSSIVEL  
	
	return (int) llwrite(applicationLayer.fileDescriptor, applicationLayer.app_frame, L+3);
}

int sendAppDataFrame(unsigned char * data, int length){
	
	unsigned char L[2];
	
	L[0] = length & 0xFF;
	L[1] = length >> 8;
	
	applicationLayer.app_frame[0] = C_DATA;
	applicationLayer.app_frame[1] = applicationLayer.sequenceNumber;
	applicationLayer.app_frame[2] = L[1];
	applicationLayer.app_frame[3] = L[0];
	memcpy(applicationLayer.app_frame+4, data, length);  
	
	if(applicationLayer.sequenceNumber == 0xFF)
		applicationLayer.sequenceNumber = 0x00;
	else 
		applicationLayer.sequenceNumber++; 
	
	return (int) llwrite(applicationLayer.fileDescriptor, applicationLayer.frame, L+3);
}

/*
int analyseControlFrame(void){

	if(applicationLayer.app_frame[0] == APP_START)
		printf("[APPLICATION] START FRAME RECEIVED");
	else if(applicationLayer.app_frame[0] == APP_END)
		printf("[APPLICATION] END FRAME RECEIVED");
	else{
		printf("[APPLICATION] INVALID CONTROL FRAME RECEIVED");
		return -1;
	}
	//to continue..
	application	
	
}
*/



int main (int argc, char** argv) {
	int txrx;
	int fd;
	int i;
	FILE *pFile;
	int lSize = 10968; 
	unsigned char L = 0; //Representa quantos bytes ocupa lSize
	int aux = lSize;
	
	while(aux != 0){
		aux = aux>>8;
		L++;
	}
		
	printf("Reciver - 0\nTransmitter -1\n");
	scanf("%d", &txrx);

	applicationLayer.status = txrx;

	printf("[MAIN] Opening llopen with %d\n", applicationLayer.status);
	applicationLayer.fileDescriptor = llopen(argv[1], applicationLayer.status);
	printf("[MAIN] LLOPEN SUCCESFULL\n");

	if(txrx == 1)  //TRANSMITTER , ainda não testado
	{
		int completeFrames = lSize/ APP_MAX_SIZE;
		int remainingBytes = lSize % APP_MAX_SIZE;
		
		
		unsigned char* buffer = (unsigned char*) malloc (sizeof(unsigned char) * lSize);
		//pFile = fopen("test.txt", "r" );
		//pFile = fopen("test_file.dat", "rb");
		pFile = fopen("penguin.gif", "rb");
		fread(buffer,sizeof(unsigned char), lSize, pFile);
		
		sendAppControlFrame(APP_START,T_SIZE,L)//START
		
		//Sending Data
		for (i = 0; i < completeFrames; i++)
		{
				sendAppDataFrame(buffer+i*APP_MAX_SIZE, APP_MAX_SIZE);
		}
		sendAppDataFrame(buffer+i*APP_MAX_SIZE, remainingBytes);
		
		sendAppControlFrame(APP_END,T_SIZE,L)//END
		
		llclose(fd); //Closing Connection
	}
	else  //RECEIVER
	{
		//Devemos assumir que o receptor não faz ideia de quanto é o tamanho do ficheiro
		int RFileSize;
		
		unsigned char* buffer = (unsigned char*) malloc (sizeof(unsigned char) * lSize);//Claro que nao podemos agora definir um buffer assim
		//pFile = fopen("test_clone.txt", "w");
		//pFile = fopen("test_file_clone.dat", "wb");
		pFile = fopen("penguin_clone.gif", "wb");
		
		
		//[IMPORTANTE]  Vamos ter de mudar o llread para fazer esta parte. RIP	
			
			
		llread(fd,buffer);
		fwrite( buffer,sizeof(unsigned char), lSize, pFile);
		//printf("Buffer read: %s\n", buffer);
	}

	return 1;
}

