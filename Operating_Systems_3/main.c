#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
	char** tokens;
	int numTokens;
} instruction;

struct bootsector
{
	char unused[11];
	uint16_t BytsPerSec;
	uint8_t SecPerClus;
	uint16_t RsvdSecCnt;
	uint8_t NumFATs;
	uint16_t RootEntCnt;
	char skip1[13];
	uint32_t TotSec32; 
	uint32_t FATSz32;
	char skip2[4];
	uint32_t RootClus;
} __attribute__ ((packed)) bs;

struct dirstruc{

	char Name[11];
	uint8_t Attr;
	uint8_t NTRes;
	uint8_t CrtTimeTenth;
	uint16_t CrtTime;
	uint16_t CrtDate;
	uint16_t LstAccDate;
	uint16_t FstClusHI;
	uint16_t WrtTime;
	uint16_t WrtDate;
	uint16_t FstClusLO;
	uint32_t FileSize;

} __attribute__ ((packed)) dir;

	void printList();
	void addToken(instruction* instr_ptr, char* tok);
	void printTokens(instruction* instr_ptr);
	void clearInstruction(instruction* instr_ptr);
	void addNull(instruction* instr_ptr);
	void info();
	void size(char file[], FILE *ifile, uint32_t currOffset);
	void ls(char dirname[], FILE *ifile, uint32_t CurrOffset);
	int getInfo(char name[], FILE *ifile, uint32_t currOffset);
	uint32_t getNextSector(uint32_t n, FILE *ifile);
	uint32_t FS();
	uint32_t cd (char dirname[], FILE *ifile, uint32_t CurrOffset);
	char* rmSpace(char* name);
	int creat(char* name, FILE *ifile, uint32_t currOffset, int direc);
	int EmptyFAT(FILE *ifile);
	uint32_t CS(uint32_t offset);
	uint32_t FATOffset(uint32_t currSector);
	int mkdir(char*name, FILE*ifile, uint32_t currOffset);
	int mv (char* src, char* dest, FILE *ifile, uint32_t currOffset);
	int open (char* name, char* mode, FILE  *ifile, uint32_t currOffset);
	int close (char*name, FILE *ifile, uint32_t currOffset);
	int read (char*name, FILE *ifile, int of, int size, uint32_t currOffset);
	int rm(char*name, FILE *ifile, uint32_t currOffset);
	int write(char*name, FILE *ifile, int of, int size, char *str, uint32_t currOffset);
	int cp(char*name, FILE *ifile, char* dest, uint32_t currOffset);
uint32_t getNextFat(uint32_t sect, FILE *ifile);
//global variables
uint32_t CurrOffset;
uint8_t one = 0;
uint16_t two = 0;
uint32_t four = 0;
uint64_t eight = 0;
char path[512]; 


typedef struct openfile{
	uint32_t sector;
	char fullpath[512];
	int rwmode;//1 for read, 2 for write, 3 for read/write
	
} openfile_t;
 
typedef struct Node{
	openfile_t *fi;
	struct Node *next;
} Node_t;

Node_t *head;

int main(int argc, char** argv) {

	//load fat32 file system from command line argument
	if (argc != 2){
		printf("usage is $project3 image.img\n");	
		exit(1);
	}

	// open image to read
	FILE *ifile = fopen(argv[1], "r+");
	if (ifile == NULL){
		printf("Error! image could not be opened\n");
		exit(1);
	}
	
	//parse bootsector and get information
	fseek(ifile, 0, SEEK_SET);
	fread(&bs, sizeof(bs), 1, ifile);
	
	uint32_t RootDirSec = ((bs.RootEntCnt * 32) + (bs.BytsPerSec - 1)) / bs.BytsPerSec; //0
	uint32_t FirstDataSec = bs.RsvdSecCnt + (bs.NumFATs * bs.FATSz32);
	uint32_t FirstClusSec = ((bs.RootClus - 2) * bs.SecPerClus) + FirstDataSec;
	uint32_t RootOffset = FirstClusSec * 512; //root offset in bytes
	fseek(ifile, RootOffset, SEEK_SET);
	CurrOffset = RootOffset;

	//initialize strcpy;
	strcpy(path, "/");	

	//printf("first data sector: %d\n", FirstDataSec);
	//printf("byte off set of root directory: %d\n", FirstClusSec*512);

	//begin parsing instructions

	//initialize head
	head = NULL;

	char* token = NULL;
	char* temp = NULL;
	
	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;

	while (1) {
		printf("%s $ ", path);
		// loop reads character sequences separated by whitespace
		do {
			//scans for next token and allocates token var to size of scanned token
			scanf("%ms", &token);
			addToken(&instr, token);
			//free and reset variables
			free(token);
			free(temp);
			token = NULL;
			temp = NULL;
		} while ('\n' != getchar());    //until end of line is reached
			
		addNull(&instr);
		//printTokens(&instr);

//OUR CODE BEGINS HERE//
			//all cases for commands
			//check for exit to begine exit procedure
			if (strcmp(instr.tokens[0], "exit") == 0)
				break;			
			else if (strcmp(instr.tokens[0], "info") == 0){
				//print bootsector info
				info();
			}else if (strcmp(instr.tokens[0], "size") == 0){
				//find size of a file
				if (instr.numTokens != 3){
					printf("size usage: size FILENAME\n");
				}else{
					size(instr.tokens[1], ifile, CurrOffset);
					fseek(ifile, CurrOffset, SEEK_SET);
				}

			}else if(strcmp(instr.tokens[0], "ls") == 0){
				if (instr.numTokens > 3){
					printf("ls usage: ls [DIRNAME]\n");
				}else if(instr.numTokens == 2){
					char *a = NULL;
					ls(a, ifile, CurrOffset);	
					fseek(ifile, CurrOffset, SEEK_SET);
				}else if(instr.numTokens == 3){
					ls(instr.tokens[1], ifile, CurrOffset);
					fseek(ifile, CurrOffset, SEEK_SET);
				}
			}else if (strcmp(instr.tokens[0], "cd") == 0){
				if (instr.numTokens != 3){
					printf("cd usage: cd DIRNAME\n");
				}else{
					uint32_t temp = cd(instr.tokens[1], 
							ifile, CurrOffset);
					//printf("strlen path: %d\n", strlen(path));
					if (temp != 0){
				if(strcmp(instr.tokens[1], "..") == 0){
				
				int i = strlen(path)-1;
				path[i] = 0;
				for (i; path[i] != '/'; i--){
					path[i] = 0;
				}
	
				}else if (strcmp(instr.tokens[1], ".") != 0){
					strcat(path, instr.tokens[1]);
					strcat(path, "/");
				}
						printf("new offset: %d\n", temp);
						CurrOffset = temp;
						fseek(ifile, temp, SEEK_SET);
					}else{
						fseek(ifile, CurrOffset, SEEK_SET);
					}
				}
			}else if(strcmp(instr.tokens[0], "creat") == 0){
				if (instr.numTokens != 3){
					printf("creat usage: creat FILENAME\n");
				}else{
					int cr = creat(instr.tokens[1],
						ifile, CurrOffset, 0);
					fseek(ifile, CurrOffset, SEEK_SET);
					if (cr == 1){
					printf("file created successfully\n");
					}
					fseek(ifile, CurrOffset, SEEK_SET);
				}
				
			}else if (strcmp(instr.tokens[0], "mkdir") == 0){
				if (instr.numTokens != 3){
					printf("mkdir usage: mkdir DIRNHAME\n");
				}else{
					int cr = mkdir(instr.tokens[1],
							ifile, CurrOffset);
					fseek(ifile, CurrOffset, SEEK_SET);
					if (cr == 1){
						printf("dir created succesfully\n");
					}
				}
			}else if (strcmp(instr.tokens[0], "mv") == 0){
				if (instr.numTokens != 4){
					printf("mv usage: mv SRC DEST\n");
				}else{
					int t = mv(instr.tokens[1], instr.tokens[2],
						ifile, CurrOffset);
					fseek(ifile, CurrOffset, SEEK_SET);
					if (t == 1)
						printf("moved successfully\n");
				}
			}else if (strcmp(instr.tokens[0], "open") == 0){
				if (instr.numTokens != 4){
					printf("open usage: open FILENAME MODE\n");
				}else{
					int t =open(instr.tokens[1],instr.tokens[2],
							ifile, CurrOffset);
					fseek(ifile, CurrOffset, SEEK_SET);
					if (t == 1){
						
				//		printList();
						printf("File opened successfully\n");
					}
				}
			}else if(strcmp(instr.tokens[0], "close") == 0){
				if (instr.numTokens != 3){
					printf("close usage: close FILENAME\n");
				}else{
					int t = close(instr.tokens[1],
							ifile, CurrOffset);
					fseek(ifile, CurrOffset, SEEK_SET);
					
					if (t == 1){
						
					//	printList();
						printf("File closed successfully\n");
					}
				}
			}else if(strcmp(instr.tokens[0], "read") == 0){
				if(instr.numTokens != 5){
					printf("read usage: read FILENAME OFFSET SIZE\n");
				}
				else{
					//printf("read offset passed in as: %d\n", atoi(instr.tokens[2]));
					//printf("read size passed in as: %d\n", atoi(instr.tokens[3]));
					int t = read(instr.tokens[1],
							ifile,atoi(instr.tokens[2]),
							atoi(instr.tokens[3]), CurrOffset);
					fseek(ifile, CurrOffset, SEEK_SET);
				}
			}else if(strcmp(instr.tokens[0], "write") == 0){
				if(instr.numTokens < 6){
					printf("write usage: write FILENAME OFFSET SIZE STRING\n");
				}
				else{
					int error = 0;
					//printf("write offset passed in as: %d\n", atoi(instr.tokens[2]));
					//printf("write size passed in as: %d\n", atoi(instr.tokens[3]));
					if (instr.tokens[4][0] != '"'){
						printf("String must begin with a quotation!\n");
						error = 1;
					}else if (instr.tokens[instr.numTokens-2][strlen(instr.tokens[instr.numTokens-2])-1] != '"'){
						printf("String must end with a quotation!\n");
						error = 1;
					}
					if (error == 0){
						int i = 4;
						int length = 0;		
						for(i; i < instr.numTokens-1; i++){
							length += strlen(instr.tokens[i]);
							length++;//for spaces
						} 
						length -=3;//dont need quotations and one extra space	
						//if (instr.numTokens == 6)
						//	length--;
						char *string = (char*)malloc(sizeof(char)*length+1);
						string = strcpy(string, "");
						for (i = 4; i < instr.numTokens-1; i++){
							if (i == instr.numTokens-2 && i == 4){//first and last
								strncat(string, instr.tokens[i]+1, strlen(instr.tokens[i])-2);
								 
							}else if (i == 4){//first
								strncat(string, instr.tokens[i]+1, strlen(instr.tokens[i])-1); 
							}else if (i == instr.numTokens-2){//last
								strcat(string, " ");
								strncat(string, instr.tokens[i], strlen(instr.tokens[i])-1); 
							}else{//none
								strcat(string, " ");
								strncat(string, instr.tokens[i], strlen(instr.tokens[i])); 
							}
						}
						int t = write(instr.tokens[1],
								ifile,atoi(instr.tokens[2]),
								atoi(instr.tokens[3]), string, CurrOffset);
						fseek(ifile, CurrOffset, SEEK_SET);
					}
				}
			}else if(strcmp(instr.tokens[0], "rm") == 0) {
				if (instr.numTokens != 3){
					printf("rm usage: rm FILENAME\n");
				}else{
					int t = rm(instr.tokens[1], ifile, CurrOffset);
					fseek(ifile, CurrOffset, SEEK_SET);
				}
			}else if(strcmp(instr.tokens[0], "cp") == 0){
				if(instr.numTokens != 4)
					printf("cp usage: cp FILENAME TO\n");
				else{
					int t = cp(instr.tokens[1], ifile, instr.tokens[2], CurrOffset);
					fseek(ifile, CurrOffset, SEEK_SET);
				}
			}else{
				printf("Invalid Command\n");
			}

			//clear instruction and get another command
			clearInstruction(&instr);
		}
		//exit procedure
		//close open files, clear linked list
		clearInstruction(&instr);
	return 0;

}

//gives sector number of first sector, in sectors not in bytes
uint32_t FS(){
	
	 return (bs.RootClus-2) * bs.SecPerClus
		+ (bs.RsvdSecCnt + (bs.NumFATs * bs.FATSz32));
}
//given a byte offset will return the sector number you are in
//gives sector number not including reserved and fat, corresponds to n in functions
uint32_t CS(uint32_t offset){
	uint32_t cs = ((offset/512)+2) - FS();
}
//given a sector, returns the byte offset of the fat table entry
uint32_t FATOffset(uint32_t currSector){
	uint32_t fatval = currSector * 4;
	uint32_t ThisFatSecNum = bs.RsvdSecCnt + (fatval / bs.BytsPerSec);
	uint32_t FATOffset = fatval % bs.BytsPerSec;
	uint32_t byteOffset = (ThisFatSecNum*512) + FATOffset;
	return byteOffset;
}

//get byte offset actual data corresponding to sector number 
uint32_t getByteOffset(uint32_t n){
	if (n == 0)
		return 1049600;
	return (n-2 + FS())*512;
}

//write for a name within directory info, fills last digits with spaces
void writeChar(char* name, FILE *ifile){
	int len = strlen(name);
	int ns = 11 - len;
	int i;
	for (i = 0; i < len; i++){
		uint8_t ch = (uint8_t)name[i];
		fwrite(&ch, sizeof(ch), 1, ifile);
	}

	for (i = ns; i > 0; i--){
		uint8_t ch = 32;
		fwrite(&ch, sizeof(ch), 1, ifile);
	}
}


void seekTo(char* name, FILE* ifile, uint32_t currOffset){
	//seeks to beginning of dir entry of name, assumes it exists
	getInfo(name, ifile, currOffset);
	fseek(ifile, -32, SEEK_CUR);
}

//print list of open files
void printList(){
	Node_t *node = head;
	if (head == NULL){
		printf("List is empty!\n");
		return;
	}
	while(1){
		printf("path: %s\n", node->fi->fullpath);
		if (node->next == NULL)
			break;
		node = node->next;
	}
}
//copies a file to a directory 
int cp(char*name, FILE *ifile, char* dest, uint32_t currOffset){
	uint32_t offset = currOffset;
	int check = getInfo(name, ifile, currOffset);
	uint32_t fsize = dir.FileSize;
	fseek(ifile, currOffset, SEEK_SET);
	if(check  == 0){
		printf("file not found\n");
		return 0;
	}else if(dir.Attr ==16){
		printf("Cannot use cp on a directory\n");
		return 0;
	}
	uint32_t fatval = (dir.FstClusHI << 16) + dir.FstClusLO;
	uint32_t byteOffset = getByteOffset(fatval);
	int count = dir.FileSize/512;
	count++;
	check = getInfo(dest, ifile, offset);
	fseek(ifile, currOffset, SEEK_SET);
	int direc = 0;
	if(dir.Attr == 16){
		direc = 1;
		offset = cd(dest, ifile, currOffset);
		fseek(ifile, offset, SEEK_SET);
	}
	if (direc == 0)
		creat(dest, ifile, offset, 0);
	else 
		creat(name, ifile, offset, 0);
	
	fseek(ifile, offset, SEEK_SET);
	if (direc == 0)
		check = getInfo(dest, ifile, offset);
	else
		check = getInfo(name, ifile, offset);	

	fseek(ifile, -4, SEEK_CUR);
	fwrite(&fsize, sizeof(fsize), 1, ifile);
	uint32_t newf = (dir.FstClusHI << 16) + dir.FstClusLO;
	uint32_t newbyte = getByteOffset(newf);
	int i = 0;
	for (i; i < count; i++){
		uint8_t sectorData[512];
		fseek(ifile, byteOffset, SEEK_SET);
		int j = 0;
		for (j; j < 512; j++){
			fread(&sectorData[j], sizeof(uint8_t), 1, ifile);
		}
	
		fseek(ifile, newbyte, SEEK_SET);
		fwrite(sectorData, sizeof(sectorData), 1, ifile);
		fatval = getNextFat(fatval, ifile);
		if (fatval != 0){
			byteOffset = getByteOffset(fatval);
			uint32_t oldf = FATOffset(newf);
			uint32_t brandnew = EmptyFAT(ifile);
			fseek(ifile, oldf, SEEK_SET);
			fwrite(&brandnew, sizeof(brandnew), 1, ifile);
			newf = brandnew;
			newbyte = getByteOffset(newf); 			
		}
	}
	return 1;
}

int rm(char*name, FILE *ifile, uint32_t currOffset){
	uint32_t offset = currOffset;

	uint8_t attr = 0;
	int i;
	uint8_t emptySector[512];
	for (i = 0; i < 512; i++)
		emptySector[i] = 0; 	

	int check = getInfo(name, ifile, currOffset);
	fseek(ifile, offset, SEEK_SET);
	if (check == 0){
		printf("file not found\n");
		return 0;
	}else if(dir.Attr == 16){
		printf("cannot remove directory\n");
		return 0;
	}	
	
	uint32_t fatval = (dir.FstClusHI << 16) + dir.FstClusLO;
	seekTo(name, ifile, currOffset);
	fseek(ifile, 11, SEEK_CUR);
	fwrite(&attr, sizeof(attr), 1, ifile);
	uint32_t byteOffset = getByteOffset(fatval);
	fseek(ifile, byteOffset, SEEK_SET);
	fwrite(&emptySector, sizeof(emptySector), 1, ifile);
 	fatval = getNextFat(fatval, ifile);
	while(fatval != 0){	
		byteOffset = getByteOffset(fatval);
		fseek(ifile, byteOffset, SEEK_SET);
		fwrite(&emptySector, sizeof(emptySector), 1, ifile);
 		fatval = getNextFat(fatval, ifile);
	}
	return 1;		
}

int write(char*name, FILE *ifile, int of, int size, char *str, uint32_t currOffset){
	uint32_t offset = currOffset;
	int writesize = size;
	int check = getInfo(name, ifile, offset);
	uint32_t WriteValue = strtol("FFFFFFF", NULL, 16);	//store 0x0FFFFFFF
	if(check == 0){
		printf("File not found\n");
		return 0;
	}
	if(dir.Attr == 16){
		printf("Cannot write to directories\n");
		return 0;
	}else if(dir.Attr != 10 && dir.Attr != 11){
		printf("file not open for writing\n");
		return 0;
	}
		
	uint32_t fatval = (dir.FstClusHI << 16) + dir.FstClusLO;
	uint32_t filesize = dir.FileSize;

	uint8_t* string = (uint8_t*)malloc(sizeof(uint8_t)*strlen(str));
	int p;
	for (p = 0; p < strlen(str); p++){
		string[p] = (uint8_t)(str[p]);
	}	
	
	if((of + size) > filesize){
		dir.FileSize += (of+size) - dir.FileSize;
		uint32_t newfsize = (uint32_t)(of+size);
		//dir.FileSize = of+size;
		int count = (of+size)/512;
		count++;
		//check if new space needs to be allocated
		uint32_t newf = fatval;
		for (count; count > 0; count--){
			uint32_t newval = getNextFat(newf, ifile);
			if (newval == 0){
				//allocate new space
				uint32_t newfat = EmptyFAT(ifile);
				uint32_t byteof = FATOffset(newf);
				fseek(ifile, byteof, SEEK_SET);//write new fatvalue over empty char
				fwrite(&newfat, sizeof(newfat), 1, ifile);
				byteof = FATOffset(newfat);
				fseek(ifile, byteof, SEEK_SET);//write last clustter number at the newly allocated fat
				fwrite(&WriteValue, 4, 1, ifile);
				newf = newfat;
				
			}else{
				newf = newval;
			}
		}
		fseek(ifile, offset, SEEK_SET);
		seekTo(name, ifile, offset);
		fseek(ifile, 28, SEEK_CUR);
		fwrite(&newfsize, sizeof(newfsize), 1, ifile);
		
	}
	if(writesize > strlen(str)){
		//append string with 0's
		string = (uint8_t*)realloc(string, sizeof(uint8_t)*writesize);
		int j = strlen(str);
		for (j; j < writesize; j++){
			string[j] = 0;
		}
	}
	
	//begin writing procedure by navigating to correct offset
	int newof = of;
	int x = 0;
	while (newof > 511){
		x++;
		newof -=512;
	}
	int k = 0;
	for (k; k < x; k++){
		fatval = getNextFat(fatval, ifile);
	}	
	uint32_t byteOffset = getByteOffset(fatval);
	
	fseek(ifile, byteOffset + newof, SEEK_SET);
	//write up to next cluster
	int sofar = 0;///variable to keep track of how much has been written so far
	if (newof + writesize > 512){
		int toNext = 512 - newof;
		writesize -= toNext;
		sofar += toNext;
		int y = 0;
		fwrite(string, sizeof(uint8_t)*toNext, 1, ifile);
		fatval = getNextFat(fatval, ifile);
		byteOffset = getByteOffset(fatval);
		fseek(ifile, byteOffset, SEEK_SET);
	}

	while (writesize>0){
		if (writesize > 512){
			fwrite(string+sofar, sizeof(uint8_t)*512, 1, ifile);
			sofar+=512;
		}else{
			fwrite(string+sofar, sizeof(uint8_t)*writesize, 1, ifile);	
		}
		if (writesize>512){
			writesize-=512;
			fatval = getNextFat(fatval, ifile);
			byteOffset = getByteOffset(fatval);
			fseek(ifile, byteOffset, SEEK_SET);
		}else{
			writesize = 0;
		}
		
	}
	
	return 1;
}
int read (char*name, FILE *ifile, int of, int size, uint32_t currOffset){
	//check if file name is in open-file table and read capable
	uint32_t offset = currOffset;
	int readsize = size;
	int check = getInfo(name, ifile, offset);
	if(check == 0){
		printf("File not found\n");
		return 0;
	}
	if(dir.Attr == 16){
		printf("Cannot read directories\n");
		return 0;
	}else if(dir.Attr != 1 && dir.Attr != 11){
		printf("File not open for reading\n");
		return 0;
	}
	uint32_t fatval = (dir.FstClusHI << 16)+dir.FstClusLO;
	int count = 0;
	int newof = of;
	while (newof > 511){
		count++;
		newof -= 512;
	}
	int i;
	for (i = 0; i < count; i++){
		fatval = getNextFat(fatval, ifile);	
	}
	
	uint32_t byteOffset = getByteOffset(fatval);//store offset of data at fatval
	//byteOffset += newof;
	uint32_t filesize = dir.FileSize;
	
	if(of > filesize){
		printf("Error: offset greater than file size\n");
		return 0;
	}
	else if((of+size) > filesize){
		readsize = dir.FileSize - of;
	}
	
	fseek(ifile, byteOffset + newof, SEEK_SET);
	if (newof + readsize > 512){
		int toNext = 512 - newof;
		char* buffer = (char*)malloc(sizeof(char)*toNext+1);
		fread(buffer, sizeof(char)*toNext, 1, ifile);
		readsize -= toNext;
		fatval = getNextFat(fatval, ifile);
		uint32_t newOffset = getByteOffset(fatval);
		fseek(ifile, newOffset, SEEK_SET);	
		printf("%s", buffer);			
		free(buffer);
		buffer = NULL;
	}

	while(readsize > 0){
		//fseek(ifile, byteOffset, SEEK_SET);
		char * buffer = (char*)malloc(sizeof(char)*512+1);
		if (readsize > 512){
			fread(buffer, sizeof(char)*512, 1, ifile);
		}else{ 
			fread(buffer, sizeof(char)*(readsize), 1, ifile);
		}
		//fseek(ifile, of, SEEK_CUR);
		printf("%s",buffer);
		free(buffer);
		buffer = NULL;
		if (readsize > 512){
			readsize -= 512;	
			fatval = getNextFat(fatval, ifile);
			uint32_t newOffset = getByteOffset(fatval);
			fseek(ifile, newOffset, SEEK_SET);				
		}else
			readsize = 0;
	}
	printf("\n");
	
	return 1;
}

//this function seeks and does not reset 
//given a sector number, determine next cluster
//already wrote this once, forgot and wrote it again
uint32_t getNextFat(uint32_t sect, FILE *ifile){
	uint32_t next;
	uint32_t fatByteOffset = FATOffset(sect);		
	fseek(ifile, fatByteOffset, SEEK_SET);
	fread(&next, sizeof(next), 1, ifile);
	if (next >= 0x0FFFFFF8)
		return 0;
	return next;
}

int open (char* name, char* mode, FILE  *ifile, uint32_t currOffset){
	uint32_t offset = currOffset;
	uint8_t Attr = 32;
	char pathcheck[512];
	snprintf(pathcheck, 512, "%s%s", path, name);
	Node_t * node = head;
	while(1){
		if(node == NULL){
			//printf("Empty\n");
			break;
		}
		if(strcmp(pathcheck,node->fi->fullpath)==0){
			printf("file already open\n");
			return 0;
		}
		node = node->next;
	}
	free(node);
	node = NULL;
	int rw;
	if (strcmp(mode, "r") == 0){
		Attr = 1;
		rw = 1;
	}
	else if(strcmp(mode, "w")==0){
		Attr = 10;
		rw=2;
	}
	else if (strcmp(mode, "rw") == 0 || strcmp(mode, "wr") == 0){
		Attr = 11;
		rw = 3;
	}else{
		printf("Not an actual mode\n");	
		return 0;
	}


	char newpath[512];
	snprintf(newpath, 512, "%s%s", path, name);
//	strcpy(newpath, path);
//	newpath = strcat(newpath, "/");
//	newpath = strcat(newpath, name);

	int check = getInfo(name, ifile, currOffset);
	fseek(ifile, offset, SEEK_SET);
	if (check == 0){
		printf("could not find file\n");
		return 0;
	}
	if (dir.Attr == 16){
		printf("cannot open a directory\n");
		return 0;
	}

	seekTo(name, ifile, offset);
	fseek(ifile, 11, SEEK_CUR);
	fwrite(&Attr, sizeof(Attr), 1, ifile);

	uint32_t fatval = (dir.FstClusHI << 16)+dir.FstClusLO;

	openfile_t* newfile = (openfile_t*)malloc(sizeof(openfile_t));
	newfile->rwmode = rw;
	strcpy(newfile->fullpath, newpath);
	newfile->sector = fatval;
	//if node
	if (head == NULL){
		head = (Node_t*)malloc(sizeof(Node_t));
		head->next=NULL;
		head->fi = newfile;
	
	//if already having a node
	}else if(head != NULL){
		Node_t* prevNode = head;
		Node_t* nextNode = head->next;
		while (nextNode != NULL){
			nextNode = nextNode->next;
			prevNode = prevNode->next;
		}

		
		nextNode = (Node_t*)malloc(sizeof(Node_t));
		nextNode->next = NULL;
		nextNode->fi = newfile;	
		prevNode->next = nextNode;
		//printf("hi\n");
	}
	return 1;
}

int close (char*name, FILE *ifile, uint32_t currOffset){
	uint32_t offset = CurrOffset;
	uint8_t attr = 32;
	char newpath[512];
	snprintf(newpath, 512, "%s%s", path, name);
	if(head == NULL){
		printf("file not open\n");
		return 0;
	}
	Node_t *curNode = head;
	Node_t *prevNode = curNode;
	while(1){
		if (strcmp(curNode->fi->fullpath, newpath) == 0){
			
			seekTo(name, ifile, offset);
			fseek(ifile, 11, SEEK_CUR);
			fwrite(&attr, sizeof(attr), 1, ifile);	

			free(curNode->fi);

			if (prevNode == curNode){
				head = curNode->next;
			}else if (curNode->next == NULL){
				prevNode->next = NULL;
			}else{
				prevNode->next = curNode->next;
			}
			
			free(curNode);

				
			
			break;
		}else{
			if (curNode->next == NULL){
				printf("file not open\n");
				return 0;//not found

			}
			prevNode = curNode;
			curNode = curNode->next;
		}
	}
	return 1;
}

int mv (char* src, char* dest, FILE *ifile, uint32_t currOffset){
	uint32_t offset = currOffset;
	uint32_t currSector = CS(offset);
	uint8_t zero = 0;
	uint32_t srcd = 0;
	//uint32_t destd == 0;

	int x = getInfo(src, ifile, offset);
	char* oldname = (char*)malloc(strlen(dir.Name)+1);
	strcpy(oldname, dir.Name);
	oldname = rmSpace(oldname);
	uint8_t attr = dir.Attr;
	uint16_t hi = dir.FstClusHI;
	uint16_t lo = dir.FstClusLO;
	uint32_t size = dir.FileSize;
	
	if (x == 0){
		printf("src not found\n");
		return 0;	
	}
	if (dir.Attr == 16){
		printf("Cannot move directory, invalid argument\n");
	}
		
	fseek(ifile, offset, SEEK_SET);
	int y = getInfo(dest, ifile, offset);
	if (y == 0){
		//just rename
		fseek(ifile, offset, SEEK_SET);
		seekTo(src, ifile, offset);
		writeChar(dest, ifile);		
		return 1;
	}
	if (dir.Attr == 16){
		//move into directory
	
		uint32_t fatval = (dir.FstClusHI << 16)+dir.FstClusLO;
		uint32_t byteOffset = getByteOffset(fatval);
		fseek(ifile, offset, SEEK_SET);
		seekTo(src, ifile, offset);
		
		fseek(ifile, 11, SEEK_CUR);//set attr to 0
		fwrite(&zero, sizeof(zero), 1, ifile);
		fseek(ifile, byteOffset, SEEK_SET);
		creat(src, ifile, byteOffset, 0);
		fseek(ifile, byteOffset, SEEK_SET);
		seekTo(src, ifile, byteOffset);
		
		writeChar(oldname, ifile);
		//fseek(ifile, -1, SEEK_CUR);
		fwrite(&attr, sizeof(attr), 1, ifile);
		fwrite(&eight, sizeof(eight), 1, ifile);
		fwrite(&hi, sizeof(hi), 1, ifile);
		fwrite(&four, sizeof(four), 1, ifile);
		fwrite(&lo, sizeof(lo), 1, ifile);
		fwrite(&size, sizeof(size), 1, ifile);						
		return 1;
	}
	printf("dest exists and is not a directory\n");
	return 0;
	

}

int mkdir(char*name, FILE*ifile, uint32_t currOffset){
	uint32_t offset = currOffset;
	uint32_t currSector = CS(offset);
	int test = creat(name, ifile, currOffset, 1);
	if (test == 0){
		return 0;
	}
	fseek(ifile, offset, SEEK_SET);
	getInfo(name, ifile, currOffset);
	uint8_t Attr = 16;	
	char dot[] = ".";
	char dotdot[] = "..";
	uint32_t fatval = (dir.FstClusHI << 16)+dir.FstClusLO;
	offset = getByteOffset(fatval);
	fseek(ifile, offset, SEEK_SET);
	//write first entry for directory "."	
	writeChar(dot, ifile);
	fwrite(&Attr, sizeof(Attr), 1, ifile);
	fseek(ifile, 8, SEEK_CUR);
	uint16_t FstClusHI = (fatval >> 16);
	fwrite(&FstClusHI, sizeof(FstClusHI), 1, ifile);
	fseek(ifile, 4, SEEK_CUR);
	uint16_t FstClusLO = fatval & 65535;
	fwrite(&FstClusLO, sizeof(FstClusLO), 1, ifile);
	
	fseek(ifile, 4, SEEK_CUR);
	writeChar(dotdot, ifile);	
	fwrite(&Attr, sizeof(Attr), 1, ifile);
	fseek(ifile, 8, SEEK_CUR);
	FstClusHI = (currSector >> 16);
	fwrite(&FstClusHI, sizeof(FstClusHI), 1, ifile);
	fseek(ifile, 4, SEEK_CUR);
	FstClusLO = currSector & 65535;
	fwrite(&FstClusLO, sizeof(FstClusLO), 1, ifile);
	return 1;
		
}

int creat(char* name, FILE *ifile, uint32_t currOffset, int direc){
	uint32_t offset = currOffset;
	uint32_t currSector = CS(offset);

	if (strlen(name) > 8){
		printf("name too large, must be 8 char or less\n");
		return 0;
	}
	int check = getInfo(name, ifile, offset);
	offset = currOffset;
	if(check == 1){
		if (dir.Attr == 32){
			printf("File already exists\n");
		}else if (dir.Attr == 16){
			printf("Directory already exists\n");
		}else
			printf("Already exists\n");
		return 0;
	}
	int test = EmptyFAT(ifile);
	if(test == 0)
		return 0;
	uint32_t WriteFile = FATOffset(test);
	uint32_t WriteValue = strtol("FFFFFFF", NULL, 16);	//store 0x0FFFFFFF
	
	fseek(ifile,WriteFile,SEEK_SET);
	fwrite(&WriteValue,4,1,ifile);	
	fseek(ifile, offset, SEEK_SET);
	
	while (1){
		fread(&dir, sizeof(dir), 1, ifile);
		offset+=32;	
		if (dir.Attr == 0){
			//write the file
			offset-=32;
			fseek(ifile, offset, SEEK_SET);
			writeChar(name, ifile);
			uint8_t Attr = 32;
			if (direc == 1)
				Attr = 16;
			fwrite(&Attr, sizeof(Attr), 1, ifile);
			uint8_t NTRes = 0;
			fwrite(&NTRes, sizeof(NTRes), 1, ifile);
			uint8_t CrtTimeTenth = 0;
			fwrite(&CrtTimeTenth, sizeof(CrtTimeTenth), 1, ifile);
			uint16_t CrtTime = 0;
			fwrite(&CrtTime, sizeof(CrtTime), 1, ifile);
			fwrite(&four, sizeof(four), 1, ifile);
			uint32_t Index = (uint32_t)test;
			uint16_t FstClusHI = (Index >> 16);
			fwrite(&FstClusHI, sizeof(FstClusHI), 1, ifile);
			fwrite(&four, sizeof(four), 1, ifile);
			uint16_t FstClusLO = Index & 65535;
			fwrite(&FstClusLO, sizeof(FstClusLO), 1, ifile);
			fwrite(&four, sizeof(four), 1, ifile);
			return 1;
		}
		if ((offset%512) == 0){
			uint32_t nextsec = getNextSector(currSector, ifile);
			currSector = nextsec;
			offset= getByteOffset(nextsec);
			fseek(ifile, offset, SEEK_SET);
			if (nextsec == 0){
				printf("hi\n");
				//dir too small. allocate another sector
				uint32_t newval = EmptyFAT(ifile);
				uint32_t fatoffset = FATOffset(currSector);
				fseek(ifile, fatoffset, SEEK_SET);
				fwrite(&newval, 4, 1, ifile);
				fseek(ifile, FATOffset(newval), SEEK_SET);
				fwrite(&WriteValue, 4,1, ifile);	
				offset = getByteOffset(newval);
				fseek(ifile, offset, SEEK_SET);
				currSector = newval;
			}
		}
		//offset+=32;
		//fseek(ifile, 32, SEEK_CUR);
				
	}
	
	return 1;
}

int EmptyFAT(FILE *ifile){
	//get to beginning of FAT
	uint32_t Current = bs.RsvdSecCnt*512;
	//printf("Start Search at: %d\n",Current);
	int startofdata = 1049600;
	int index = 0;	//start from root which is 2
	int Empty = 0;
	uint32_t FATEntry;
	while(Current < startofdata){
		fseek(ifile, Current, SEEK_SET);
		fread(&FATEntry, sizeof(FATEntry), 1, ifile);
		//printf("FATEntry is : %d\n",FATEntry);
		if(FATEntry == 0){
			Empty = 1;	//entry is 0x00000000 so can be replaced
			return index;
		}
		Current += 4;
		index++;
	}
	if(Empty == 0){		//empty never changed means never found empty entry
		printf("Couldn't find available cluster in FAT\n");
		return 0;
	}
	return index;
}

uint32_t cd (char dirname[], FILE *ifile, uint32_t CurrOffset){
	uint32_t offset = CurrOffset;
	int check = getInfo(dirname, ifile, offset);
	if (check == 0){
		printf("Directory not found\n");
		return 0;
	}else{
		uint32_t val = (dir.FstClusHI << 16)+dir.FstClusLO;
		if (val == 0)
			val = 2;
		return getByteOffset(val);
	}	
}

char* rmSpace(char* name){
	char *str = (char*)malloc(strlen(dir.Name)+1);
	strcpy(str, dir.Name);
	str = strtok(str, " ");
	return str;
}

void ls(char dirname[], FILE *ifile, uint32_t CurrOffset){
	uint32_t offset = CurrOffset;
	uint32_t currSector = CS(offset);
	if (dirname != NULL){
		//seek to directory
		int check = getInfo(dirname, ifile, offset);
		//fseek(ifile, offset, SEEK_SET);
		if (check == 0){
			printf("Directory not found\n");
			return;
		}
		if (dir.Attr != 16){
			printf("Not a directory\n");
			return;
		}
		if(check == 1){
			//seek to directory
			uint32_t fatval = (dir.FstClusHI << 16)+dir.FstClusLO;
			uint32_t FirstSect = (bs.RootClus-2) * bs.SecPerClus
				+ (bs.RsvdSecCnt + (bs.NumFATs * bs.FATSz32));
			uint32_t FirstSecOfClus = (fatval-2) + FS();
			offset = FirstSecOfClus*512;
			currSector = CS(offset);
			fseek(ifile, FirstSecOfClus*512, SEEK_SET);
		}

	}
	
	int count = 0;	
	while (1){
		offset +=32;
		fread(&dir, sizeof(dir), 1, ifile);
		if ((int)dir.Name[0] == 0){
			break;
		}
		int flag = 0;
		if (dir.Attr == 15 || dir.Attr == 0){
			flag = 1;
		}
		if (flag == 0){
			printf("%s\t", rmSpace(dir.Name));
			//printf("%d\t", dir.Attr);
			count += 1;
		if (count == 8){
			printf("\n");
			count = 0;
		}
		}
		if ((offset%512) == 0){
			offset = getNextSector(currSector, ifile);
			if (offset == 0){
				break;
			}
			currSector = offset;
			//sector to byte offset
			offset = getByteOffset(offset);
			fseek(ifile, offset, SEEK_SET);
		}
	}
	printf("\n");
}

//returns sector number of next sector, or 0 if no next sector is allocated
uint32_t getNextSector(uint32_t n, FILE *ifile){
	uint32_t fat;
	uint32_t byteOffset = FATOffset(n);
	fseek(ifile, byteOffset, SEEK_SET);
	fread(&fat, sizeof(fat), 1, ifile);
	if (fat >= 0x0FFFFFF8)
		return 0;

	return fat;
}

int getInfo(char name[], FILE *ifile, uint32_t currOffset){
	uint32_t offset = currOffset;
	uint32_t currSector = CS(offset);
	//puts info into dir
	//returns 1 if found, 0 if not found
	//assumes at beginning of directory
	while(1){
		offset+=32;
		fread(&dir, sizeof(dir), 1, ifile);
		if ((int)dir.Name[0] == 0){
			return 0;
		}
		
		if (strcmp(name, rmSpace(dir.Name)) == 0){
			if (dir.Attr != 0)
				return 1;
		}
		if ((offset%512) == 0){
			uint32_t nextsec;
			nextsec = getNextSector(currSector, ifile);
			if (offset == 0){
				break;
			}
			currSector = nextsec;
			offset = getByteOffset(currSector);
			fseek(ifile, offset, SEEK_SET);
		}
	}
}

//find filename and then print size
void size(char file[], FILE* ifile, uint32_t currOffset){
	uint32_t offset = currOffset;
	int check = getInfo(file, ifile, offset);
	if (check == 1)
		printf("%d\n", dir.FileSize);
	else if(check == 0)
		printf("File not found\n");
	
}
//print info found in  boot sector
void info(){
	printf("bytes per sector: %d\nsectors per cluster: %d\n", bs.BytsPerSec, bs.SecPerClus);
	printf("reserved sector count: %d\nnumber of FATs: %d\n", bs.RsvdSecCnt, bs.NumFATs);
	printf("total sectors: %d\nFatsize: %d\nroot cluster: %d\n", bs.TotSec32, bs.FATSz32, bs.RootClus);
}
	
//reallocates instruction array to hold another token
//allocates for new token within instruction array
void addToken(instruction* instr_ptr, char* tok)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char**) malloc(sizeof(char*));
	else
		instr_ptr->tokens = (char**) realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));
		//allocate char array for new token in new slot
	instr_ptr->tokens[instr_ptr->numTokens] = (char *)malloc((strlen(tok)+1) * 
	sizeof(char));
	strcpy(instr_ptr->tokens[instr_ptr->numTokens], tok);
	instr_ptr->numTokens++;
}

void addNull(instruction* instr_ptr)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char**)malloc(sizeof(char*));
	else
		instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));
		instr_ptr->tokens[instr_ptr->numTokens] = (char*) NULL;
		instr_ptr->numTokens++;
}
void printTokens(instruction* instr_ptr)
{
	int i;
	printf("Tokens:\n");
	for (i = 0; i < instr_ptr->numTokens; i++) {
		if ((instr_ptr->tokens)[i] != NULL)
			printf("%s\n", (instr_ptr->tokens)[i]);
	}
}
void clearInstruction(instruction* instr_ptr)
{
	int i;
	for (i = 0; i < instr_ptr->numTokens; i++)
		free(instr_ptr->tokens[i]);
		free(instr_ptr->tokens);
		instr_ptr->tokens = NULL;
		instr_ptr->numTokens = 0;
}
