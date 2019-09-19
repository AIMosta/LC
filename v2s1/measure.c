#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <bsd/string.h>

#define NAME_SIZE 50
#define LINE_SIZE 1000

void decompose(char cop[4][NAME_SIZE], char *line)
{
	int i;
	char *k;
	char buffer[LINE_SIZE];

	
	// initialize cop conent
	
	 for (i=0; i< 4; i++ )
		 {
			 strlcpy(cop[i],"",NAME_SIZE); 
		 }
		
		i=0;
		
			/* decompose the line into the 4 parts: the word, the POS tag, the true chunk label, and finally
		 the predicted chunk lable. */ 
		
		k = strtok_r(line,"\t", &buffer);		
		while ( k!=NULL )
		{
			strlcpy(cop[i], k, NAME_SIZE);
			i++;
			k = strtok_r(NULL,"\t", &buffer);
			
		}
		
}


		



FILE * openFile (char * name, char * mode)
{
	FILE * tmp;
	tmp = fopen(name, mode);
	if (tmp == NULL)
	 { 
		 printf( " The file %s couldn't be open \n", name );
		 exit(EXIT_FAILURE);
	 }
	 
	 return tmp; 
}

int lengthChunk(FILE *lengthP,int colInd,long int *offset)
{ int len=1;
    char line[LINE_SIZE];
	char cop[4][NAME_SIZE];
	
  *offset=0;
	 
	
	while(!feof(lengthP))
	
	{           
				fgets(line, LINE_SIZE, lengthP);
				*offset=*offset+strlen(line);
			    decompose(cop, line);
			    
			    if (cop[colInd][0]!='I') break;
			     len++;
			 
	}
	 
	 
	 return len;
	 
}
int correcIdenChunks(FILE *resFile,int *allExiChu)
{
	char line[LINE_SIZE];
	double value;
	int trueB =0;
	int trueFoundB = 0;
	int foundB=0;
	int lengthTB=0;
	int lengthFB=0;
	char cop[4][NAME_SIZE];
	long int offset;
	FILE *lengthP;

   
	fseek(resFile, 0, SEEK_SET );
	while (! feof(resFile))
	{
		// read a line from the result file
		fgets(line, LINE_SIZE, resFile);
		
		/* decompose the line into the 4 parts: the word, the POS tag, the true chunk label, and finally
		 the predicted chunk lable. */ 
		 
		
		 decompose(cop, line);
	
		
         if (cop[2][0] == 'B' )
		        {	      
		       // the count of the true "B" label 
		        trueB++;
		         if (cop[3][0] == 'B' )
		          {	      
		           // the count of the found "B" label
		            foundB++;		            
					lengthTB=lengthChunk(resFile,2,&offset);
		            fseek(resFile, -offset, SEEK_CUR );

		            lengthFB=lengthChunk(resFile,3,&offset);
		            fseek(resFile, -offset, SEEK_CUR );

		              if (lengthTB==lengthFB)
		              trueFoundB++;
		              
		              		          

			       }  
			   }

	}
	
	
	 *allExiChu=trueB;
	 

	return trueFoundB;
	
}
int allFoundChun(FILE *resFile)
{
	char line[LINE_SIZE];
	int foundB=0;
	char cop[4][NAME_SIZE];

    int numberOfLines=0;
   
	fseek(resFile, 0, SEEK_SET );
	while (! feof(resFile))
	{
		numberOfLines++;
		// read a line from the result file
		fgets(line, LINE_SIZE, resFile);
		
		/* decompose the line into the 4 parts: the word, the POS tag, the true chunk label, and finally
		 the predicted chunk lable. */ 
		 
		
		
		 decompose(cop, line);
	
		
         if (cop[3][0] == 'B' )
		        {	      
		       // the count of the true "B" label 
		     
		        foundB++;
		           
			   }

	}
	
	

	return foundB;
	
	
}




int main(int argc, char *argv[])
{
	char name[NAME_SIZE] ;
	FILE * resFile;
	double myPrecision, myRecall, myFscore;
	int corrIdCh,allFChu,allExChu; 


	
	// verfiy that the user supplied an argument for the call of measure
   if (argc == 1)
	 
	 {
 	  printf( "You must specifiy the file that contains the result of the prediction !");
	  exit (EXIT_FAILURE);
	 }
	 
	 // copy the name of the file	 
	 
	 strlcpy(name,argv[1], NAME_SIZE);
	 
	 // open the file	 
	 resFile=openFile(name, "r");
	 
	 
	 /* compute correct identified chunks ,
	  * all existing chunks
	  */
	    
	   corrIdCh= correcIdenChunks(resFile,&allExChu); 
	   allFChu = allFoundChun(resFile);
	   
	 // compute precision value
     
     myPrecision = 1.*corrIdCh / allFChu;
     
	 
	// print the precision to the standard output 
	printf("\n The precision of the model is estimated at : %.2f \%  \n\n", myPrecision*100);
	 
	
	 // compute the recall value
   myRecall= 1.*corrIdCh / allExChu;
	 // print the recall to the standard output 	
	 printf(" The recall of the model is estimated at : %.2f \%   \n\n", myRecall*100);
	 
	  // compute the F1-score  value
	  
	  myFscore= 2 * myPrecision * myRecall / (myPrecision + myRecall);
     // print the F1-score to the standard output 	
	 printf(" The F1-score of the model is estimated at : %.2f \%   \n\n", myFscore*100);
	 
	 
	 	 // print all the computed numbers to the standard output 	

	 printf("**** all computed numbers **********\n\n");
	 
	 printf("The number of found chunks: %d\n",allFChu);
	 printf("The number of true chunks: %d\n",allExChu);
	 printf("The number of correct found chunks: %d\n",corrIdCh);
	 
	 
	 
	 

	
	
	// to uncomment after the you completed the FRate function
	// 	 myFRate = FRate (resFile);

	//printf(" The F-Rate of the model is estimated at : %.2f \n", myFRate);
	
	
    fclose(resFile);	
	
	return 0;
}
