#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef enum{
	IMG_ERROR=0,
	BMP=1,
	JPEG=2,
	GIF=3,
	PNG=4
} eImageTypes_t;
struct RawImage{
	eImageTypes_t imgType;
	unsigned int Xsize;
	unsigned int Ysize;
	unsigned short int bytesPerPixel;
	char* imgBitmapVector;
} extractedPic;

eImageTypes_t CheckFileType(FILE* const openedFile);
unsigned long long int CheckBMPSizeAndConsist(FILE* const openedFile);
unsigned int InverseByteOrder(unsigned int length, unsigned char* vector);
unsigned long int ConvertHexToULI(unsigned char* inputHexString, unsigned char strLength);

int main(int argc, char** argv){
	FILE* inputFile;
	if(argc<=1) printf("No arguments supplied. Exiting."); // do nothing
	if(argc>1){
		printf("Output path not mentioned. Output will be stored in the same directory named: out+%s.\n",argv[1]);
		inputFile=fopen(argv[1],"r+");	
		if(inputFile!=NULL && ferror(inputFile)==0){
			extractedPic.imgType=CheckFileType(inputFile);
      printf("Extracted type:%d\n",extractedPic.imgType);
      if(extractedPic.imgType==BMP){ // maybe case?
        if(CheckBMPSizeAndConsist(inputFile)!=0){

        }
        else{
          printf("Error in file consistency.\n");
          exit(EXIT_FAILURE);
        } 
        // extract the header to read the file
        // extract the bitmap  
      }
      //do the same for other formats later
		}
    else{
      perror("Error occured while opening file: ");
      exit(ferror(inputFile));
    }
    
	}
  exit(EXIT_SUCCESS);
}

eImageTypes_t CheckFileType(FILE* const openedFile){
	//check file format no matter what extension is.
	//returns 0 if format is unrecognized or error happened.

	const unsigned char bmp_mark1[3]={0x4d, 0x42, 0x0}; //BMP
  const unsigned char bmp_mark2[3]={0x42, 0x4d, 0x0}; //BMP
  const unsigned char jpeg_mark[3]={0xff, 0xd8, 0x0}; //JPEG
  const unsigned char gif_mark[4]={0x47, 0x49, 0x46, 0x0}; //GIF
  const unsigned char png_mark[9]={0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x0}; //PNG
  unsigned char i=0;
  eImageTypes_t imgType=IMG_ERROR;
  unsigned char * restrict nextByte;
  nextByte=(unsigned char*)calloc(9,sizeof(unsigned char));
  (void)rewind(openedFile);
  while(i<8){
  	*(nextByte+i)=fgetc(openedFile);
  	i++;
  }
  *(nextByte+8)='\0';
  if(feof(openedFile)==0 && ferror(openedFile)==0){
  	switch(*(nextByte+0)){
  		case 0x4d:{
  			if(memcmp(nextByte,bmp_mark1,2)==0) imgType=BMP;
  			else imgType=IMG_ERROR;
  			break;
  		}
  		case 0x42:{
  			if(memcmp(nextByte,bmp_mark2,2)==0) imgType=BMP;
  			else imgType=IMG_ERROR;
  			break;
  		}
  		case 0xff:{
  			if(memcmp(nextByte,jpeg_mark,2)==0) imgType=JPEG;
  			else imgType=IMG_ERROR;
  			break;
  		}
  		case 0x47:{
  			if(memcmp(nextByte,gif_mark,2)==0) imgType=GIF;
  			else imgType=IMG_ERROR;
  			break;
  		}
  		case 0x89:{
  			if(memcmp(nextByte,png_mark,2)==0) imgType=PNG;
  			else imgType=IMG_ERROR;
  			break;
  		}
  		default:{imgType=IMG_ERROR;break;}
  	}
  }
  else imgType=IMG_ERROR;

  free(nextByte);

  return imgType;
}
unsigned long long int CheckBMPSizeAndConsist(FILE* const openedFile){
  //checks size of BMP-file reading it byte by byte
  //0 - something went wrong.

  unsigned long long int fileBytesCount=0; //how large the file is
  unsigned long long int i=0;
  unsigned char * restrict bmFileSizeField; //readed data
  bmFileSizeField=(unsigned char*)calloc(5,sizeof(unsigned char));
  
  (void)fseek(openedFile,2,SEEK_SET);
  printf("filling array\n");
  while(i<4){
    *(bmFileSizeField+i)=(unsigned char)fgetc(openedFile);
    i++;
  }
  *(bmFileSizeField+i)='\0';
  //InverseByteOrder(i,bmFileSizeField); // 4 is to avoid EOL-marker switching
  fileBytesCount=ConvertHexToULI(bmFileSizeField, 4);
  (void)rewind(openedFile);
  i=0;
  printf("check sizes; EOF is %d\n",EOF);
  while(fgetc(openedFile)!=EOF) printf("%d;",i++);
  printf("checked=%d;\n",i);
  printf("comparing...\n");
  if(i==fileBytesCount) return fileBytesCount;
  else return 0;
}

unsigned long int ConvertHexToULI(unsigned char* inputHexString, unsigned char strLength){
  const unsigned char base=0xf;
  unsigned char i=0;
  unsigned long int result=0;

  while(i<strLength){
    result+=*(inputHexString+i)*pow(base,i);
    i++;
  }

  return result;
}

unsigned int InverseByteOrder(unsigned int length, unsigned char* vector){
  unsigned int byteCnt=0;
  for(;byteCnt<length/2;byteCnt++){
    //printf("START: BMPfile-%d; pair-%d\n",*(vector+i),*(vector+length-1-i));
    vector[byteCnt]=*(vector+byteCnt)+*(vector+length-1-byteCnt); //printf("ACT1: BMPfile-%d; pair-%d\n",*(vector+i),*(vector+length-1-i));
    *(vector+length-1-byteCnt)=*(vector+byteCnt)-*(vector+length-1-byteCnt); //printf("ACT2: BMPfile-%d; pair-%d\n",*(vector+i),*(vector+length-1-i));
    *(vector+byteCnt)=*(vector+byteCnt)-*(vector+length-1-byteCnt); //printf("ACT3-END: BMPfile-%d; pair-%d\n",*(vector+i),*(vector+length-1-i));
  }
  return byteCnt;
}