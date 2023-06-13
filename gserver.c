#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

//read all the quotes from quotes.txt
//when client asks for a motivational quote, select one randomly and send it out.

#define MAXWORDS 100000
#define MAXLEN 1000

char *words[MAXWORDS];
int numWords=0;
int main() {
	char line[MAXLEN];

	FILE *fp = fopen("dictionary.txt", "r");
	if (!fp) {
		puts("quotes.txt cannot be opened for reading.");
		exit(1);
	}
	int i=0;
	//read one line at a time, allocate memory, then copy the line into array
//	printf("in front of while \n");
	while (fscanf(fp,"%s",line) !=EOF)
	{
		words[i] = (char *) malloc(strlen(line)+1);
		strcpy(words[i], line);
		i++;
	//	printf("here %d, %s\n",i,words[i]);
	}
	numWords = i;
//	printf("After while \n");
	fclose(fp);


	srand(getpid() + time(NULL) + getuid());
	// puts(quotes[rand() % numQuotes]);
	
	//create a named pipes to read client's requests
	char filename[MAXLEN];
	sprintf(filename, "/tmp/%s-%d", getenv("USER"), getpid());
	mkfifo(filename, 0600);
	chmod(filename, 0622);
	printf("Send your requests to %s\n", filename);

	while (1) {
		FILE *fp = fopen(filename, "r");
		if (!fp) {
			printf("FIFO %s cannot be opened for reading.\n", filename);
			exit(2);
		}
		printf("Opened %s to read...\n", filename);

		// select random word,store the length
		int ranInd = rand() %numWords;
		int wordLen = strlen(words[ranInd]);
		//printf("wordlen %d\n",wordLen);
		
		printf("selected word is %s \n",words[ranInd]);
		char key[MAXLEN];
		strcpy(key,words[ranInd]);	// store the key word		
			char tempWd [MAXLEN];
			for( int i = 0; i<wordLen;i++)			
			{
				tempWd[i] = '*';
		
			}
			tempWd[wordLen] = '\0';
	
		//wait for clients' requests
		while (fscanf(fp,"%s",line)!=EOF) {
		//	printf("In client's req\n");
			char *cptr = strchr(line, '\n');
			if (cptr) 
				*cptr = '\0';
			//create a child to work with this client
			if (fork() == 0)
			{
				//??????????
				
				
			//printf("infork \n");  //---------- working 	

				FILE *clientfp = fopen(line, "w");

				//create and send new server fifo to the client
				//for private one-on-one communcations
				char serverfifo[MAXLEN];
				sprintf(serverfifo, "/tmp/%s-%d", getenv("USER"), getpid());
				mkfifo(serverfifo, 0600);
				chmod(serverfifo, 0622);
				
				// send serverfifo to client to establish connection 
				fprintf(clientfp, "%s\n", serverfifo);
				fflush(clientfp);
				
				// problem
				FILE *serverfp = fopen(serverfifo, "r");
				//send client prompt
				fprintf(clientfp,"Enter a letter in word %s \n> ",tempWd);
				fflush(clientfp);
			//	printf("After sending line to client to promp\n");
				//client return	
				char guessCh[MAXLEN];
				int missed=0;
			//problem

				while (fscanf(serverfp, "%s", &guessCh)!=EOF)
				{
					
					int flag = 0;
					int repeat;
				//	printf("in while comparing\n");
					//Compare the letter in word
					//if client guessed wrong
					for ( int i= 0; i<strlen(key);i++)
					{
						if( key[i]== guessCh[0])
						{
							flag = 1;
							repeat = 0;
							if(tempWd[i]== guessCh[0])
							{	// if word already exists
								repeat = 1;
							}

							//if no repeat,  replace ttempwd with client's char
							tempWd[i] = guessCh[0]; 
							// check if guessed complete word
							//replace the char with matching char in key
							if(strcmp(tempWd,key)==0)
							{
								fprintf(clientfp,"The word is %s, you missed %d times\n",key,missed);
								fflush(clientfp);
							}

					
						}
						
					}
					if(flag==0)
					{	//printf("guessed wtrong\n")
						missed++;
					}
					

					if(repeat ==1)
					{
						fprintf(clientfp,"%s is already in the  word\n, Enter a letter in word %s \n> ",guessCh,tempWd);
						fflush(clientfp);
					}
					else 
					{
					// by the end of key , if no matching ,increment missed
					// promp again for client to guess
					fprintf(clientfp,"Enter a letter in word %s \n> ",tempWd);
					fflush(clientfp);
					}

				}
				exit(0);
				}
		}
		fclose(fp);
	}

		


}


