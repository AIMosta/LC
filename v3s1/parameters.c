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

void computeMetrics(char * nameResFile, double *precision, double * recall, double * FScore)
{
	char name[NAME_SIZE] ;
	FILE * resFile;
	double myPrecision, myRecall, myFscore;
	int corrIdCh,allFChu,allExChu; 

	 
	 // open the file	 
	 resFile=openFile(nameResFile, "r");
	 
	 
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
	 
	 
	 
	 *precision=myPrecision;
	 *recall=myRecall;
	 *FScore=myFscore;

	
	
	// to uncomment after the you completed the FRate function
	// 	 myFRate = FRate (resFile);

	//printf(" The F-Rate of the model is estimated at : %.2f \n", myFRate);
	
	
    fclose(resFile);	
	
}

int main(int argc, char *argv[])
{
	char name[NAME_SIZE] ;
	FILE * resFile;
	double myPrecision, myRecall,myFscore;
	char command_train[LINE_SIZE]="make CORPUS=train.data MODEL=model" ;
    char command_test[LINE_SIZE]="yamcha -m model" ;
	char cmd[LINE_SIZE]="";	
	int i,j; 
	char features[2][NAME_SIZE]={"\"F:-2..2:0.. T:-2..-1\"","\"F:-3..3:0.. T:-3..-1\""};
	char class_method[2][NAME_SIZE]={"1","2"};
	
	FILE *testFile;
	
	
	
	testFile=openFile("tests","w");
	  
	  for (i=0; i<2; i++)
	  {  // for each type of feature train and test with
		  for (j=0;j<2;j++)
		  {// each classification method
			  
	         fprintf(testFile,"----------------------------------------------------------------------------------\n");
		     fprintf(testFile,"           model_%d_%d- FEATURE=%s MULTI_CLASS=%S      \n",i,j,features[i],class_method[j]);
		     fprintf(testFile,"-----------------------------------------------------------------------------------\n\n");			  
		     
		     fprintf(testFile,"        ----- Training -----             \n");
			  
			  // train
		     strlcpy(cmd,"",LINE_SIZE);
		     sprintf(cmd,"%s_%d_%d FEAUTRE=%s MULTI_CLASS=%s  train",command_train,i,j,features[i],class_method[j]); 	     
		     system(cmd);		     
		      //test
		      
		     fprintf(testFile,"        ----- Testing -----             \n"); 
		     
		     strlcpy(cmd,"",LINE_SIZE);
		     sprintf(cmd,"%s_%d_%d.model < test.data > result_%d_%d.data",command_test,i,j,i,j);
		     system(cmd);		     
		     
		      // compute performance metrics
		      
		     
		          //the name of the results file
		     
		     strlcpy(name,"",NAME_SIZE);
		     sprintf(name,"result_%d_%d.data",i,j);
		     
		      
	 
	         // compute metrics
	        computeMetrics(name,&myPrecision,&myRecall,&myFscore);
	        	 
	         // print the precision, the recall, and the F1-score to the tests file 
	         fprintf(testFile,"\n The precision of the model is estimated at : %.2f \% \n\n", myPrecision*100);
	         fprintf(testFile," The recall of the model is estimated at : %.2f \% \n\n", myRecall*100);
	         fprintf(testFile," The F1-score of the model is estimated at : %.2f \% \n\n", myFscore *100);

    
             fprintf(testFile,"----------------end ----------------\n\n\n");

		     
		     
		  }
	  }
	
      fprintf(testFile,"-----------------------------------\n");
      fprintf(testFile,"-----------------------------------\n");


	
    fclose(testFile);
    	

	
	return 0;
}
