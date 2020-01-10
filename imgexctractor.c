#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{
	IMG_ERROR=0,
	BMP=1,
	JPEG=2,
	GIF=3,
	PNG=4
} eImageTypes;
struct RawImage{
	eImageTypes imgType;
	unsigned int Xsize;
	unsigned int Ysize;
	unsigned short int bytesPerPixel;
	char* imgBitmapVector;
} extractedPic;

eImageTypes CheckFileType(FILE* openedFile);

int main(int argc, char** argv){
	FILE* inputFile;

	if(argc<=1) printf("No arguments supplied. Exiting."); // do nothing
	if(argc>1){
		printf("Output path not mentioned. Output will be stored in the same directory named: out+%s.\n",argv[1]);
		inputFile=fopen(argv[1],"r+");	
		if(inputFile!=NULL && ferror(inputFile)==0){
			extractedPic.imgType=CheckFileType(inputFile);
      if(extractedPic.imgType==BMP){
        // check file contents
        // extract the header to read the file
        // extract the bitmap  
      }
      //do the same for other formats
		}
    else{
      perror("Error occured while opening file: ");
      printf("%d\n",ferror(inputFile));
    }
		(void)getchar();
	}

	return 0;
}

eImageTypes CheckFileType(FILE* openedFile){
	//check file format no matter what extension is.
	//returns 0 if format is unrecognized or error happened.
	//
	const unsigned char bmp_mark1[3]={0x4d, 0x42, 0x0}; //BMP
  const unsigned char bmp_mark2[3]={0x42, 0x4d, 0x0}; //BMP
  const unsigned char jpeg_mark[3]={0xff, 0xd8, 0x0}; //JPEG
  const unsigned char gif_mark[4]={0x47, 0x49, 0x46, 0x0}; //GIF
  const unsigned char png_mark[9]={0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x0}; //PNG
  unsigned short i=0;
  eImageTypes imgType=IMG_ERROR;
  unsigned char* nextByte;
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
  return imgType;
}
