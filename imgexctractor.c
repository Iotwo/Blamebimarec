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
	unsigned long int Xsize;
	unsigned long int Ysize;
	unsigned char bytesPerPixel;
	unsigned char* imgBitmapVector;
} extractedPic;

eImageTypes_t CheckFileType(FILE* const openedFile);
unsigned long long int CheckBMPSizeAndConsist(FILE* const openedFile);
void GetBMPFileInfo(FILE* const openedFile, struct RawImage imgDataKeeper);
unsigned char ProcessBMPInfo(FILE* const openedFile, struct RawImage infoStructure); //not ready
//unsigned char* ExtractBitmap(FILE* const openedFile); //not ready
unsigned int InverseByteOrder(unsigned char* vector, unsigned int vectorLength);
unsigned long int ConvertHexToULI(unsigned char* inputHexString, unsigned char strLength);

int main(int argc, char** argv){
	FILE* restrict inputFile;
	if(argc<=1) printf("No arguments supplied. Exiting."); // do nothing
	if(argc>1){
		printf("Output path not mentioned. Output will be stored in the same directory named: out+%s.\n",argv[1]);
		inputFile=fopen(argv[1],"r+");	
		if(inputFile!=NULL && ferror(inputFile)==0){
			extractedPic.imgType=CheckFileType(inputFile);
      printf("Extracted type:%d\n",extractedPic.imgType);
      if(extractedPic.imgType==BMP){ // maybe case?
        if(CheckBMPSizeAndConsist(inputFile)!=0){
          ProcessBMPInfo(inputFile,extractedPic);
          //redraw 
        }
        else{
          printf("Error in file consistency.\n");
          exit(EXIT_FAILURE);
        } 
        
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
  int c='\0';
  unsigned char * restrict bmFileSizeField; //readed data
  bmFileSizeField=(unsigned char*)calloc(5,sizeof(unsigned char));
  
  (void)fseek(openedFile,2,SEEK_SET);
  while(i<4){
    *(bmFileSizeField+i)=(unsigned char)fgetc(openedFile);
    i++;
  }
  *(bmFileSizeField+i)='\0';
  fileBytesCount=ConvertHexToULI(bmFileSizeField, 4);
  
  (void)rewind(openedFile);
  i=0;
  printf("EOF=%d\n",EOF);
  //while(fgetc(openedFile)!=EOF) i++;
  while((c=fgetc(openedFile))!=EOF && feof(openedFile)==0 && ferror(openedFile)==0) i++;
  printf("feof=%x, ferror=%x, error=%x symbol=%d\n", feof(openedFile),ferror(openedFile),errno, c);
  if(c==EOF) printf("Char = EOF!\n");
  //printf("err=%d\n",feof(openedFile));
  printf("Count: %lld, field: %lld;\n",i,fileBytesCount);
  if(i==fileBytesCount) return fileBytesCount;
  else return 0;
}

unsigned char ProcessBMPInfo(FILE* const openedFile, struct RawImage infoStructure){
  //find and read the file's header
  // -copy datavector
  const unsigned char dataFieldLen=4;
  unsigned char* restrict picData;
  picData = (unsigned char*)calloc(4,sizeof(unsigned char));
  unsigned long int i=0, j=0, dataBytesCnt=0;
  (void)rewind(openedFile);
  //get width, heigth, color depth
  (void)fseek(openedFile,0x12,SEEK_SET);
  for(i=0;i<dataFieldLen;i++) *(picData+i)=fgetc(openedFile);
  infoStructure.Xsize=ConvertHexToULI(picData,dataFieldLen);
  for(i=0;i<dataFieldLen;i++) *(picData+i)=fgetc(openedFile);
  infoStructure.Ysize=ConvertHexToULI(picData,dataFieldLen);
  (void)fseek(openedFile,0x1C,SEEK_SET);// skip biPlanes field
  for(i=0;i<dataFieldLen-2;i++) *(picData+i)=fgetc(openedFile);
  infoStructure.bytesPerPixel=ConvertHexToULI(picData,dataFieldLen-2)/8;
  
  //copy color vector
  (void)fseek(openedFile,0xA,SEEK_SET);
  for(i=0;i<dataFieldLen;i++) *(picData+i)=fgetc(openedFile);
  i=ConvertHexToULI(picData,dataFieldLen);
  dataBytesCnt=infoStructure.Xsize*infoStructure.Ysize*infoStructure.bytesPerPixel;
  infoStructure.imgBitmapVector=(unsigned char*)calloc(dataBytesCnt, sizeof(unsigned char));
  (void)fseek(openedFile,i,SEEK_SET);
  for(j=0; feof(openedFile)==0; j++) *(infoStructure.imgBitmapVector+j)=(unsigned char)fgetc(openedFile);
    //debug info
  printf("X: %d\nY: %d\nD: %d\nA: %x\n", infoStructure.Xsize, infoStructure.Ysize, infoStructure.bytesPerPixel,i);

  return 0;
}

//=======================================================================
unsigned char* ExtractBitmap(FILE* const openedFile, unsigned int imageVectorSize){
  unsigned char* restrict bitmapVector;
  bitmapVector=(unsigned char*)calloc(imageVectorSize, sizeof(unsigned char));

  return bitmapVector;
}

unsigned long int ConvertHexToULI(unsigned char* inputHexString, unsigned char strLength){
  const unsigned char base=0xff;
  unsigned char i=0;
  unsigned long int result=0;

  while(i<strLength){
    result+=*(inputHexString+i)*pow(base+1,i);
    i++;
  }

  return result;
}

unsigned int InverseByteOrder(unsigned char* vector, unsigned int vectorLength){
  unsigned int byteCnt=0;
  for(;byteCnt<vectorLength/2;byteCnt++){
    vector[byteCnt]=*(vector+byteCnt)+*(vector+vectorLength-1-byteCnt);
    *(vector+vectorLength-1-byteCnt)=*(vector+byteCnt)-*(vector+vectorLength-1-byteCnt);
    *(vector+byteCnt)=*(vector+byteCnt)-*(vector+vectorLength-1-byteCnt);
  }
  return byteCnt;
}