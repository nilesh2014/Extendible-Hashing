#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#define (int)pow(2,directory->globle_depth) 


int bucket_size, inital_dir_size = 0;
typedef struct bucket
{
	int local_depth;
	int *bucket_list;
	int flag;
} bckt;

typedef struct directory_element
{
	bckt *bucket;
	int flag;	// mirro or not
}dir_elem;

typedef struct directory
{
	int globle_depth;
	dir_elem *dir_elem_list;
} dir;

// Globle direcory
dir *directory;

// printing the directroy id in binary
void print(int num){
	
	for (int i = directory->globle_depth-1; i >= 0; --i)
	{
		printf("%d", num >> i & 1);
	}
	//printf("%d%d%d%d\t",num>>3 & 1,num>>2 & 1,num>>1 & 1,num>>0 & 1);
	printf("\t");
}

// Printing whole structure
void print_dir(){

	int ndirs = (int)pow(2,directory->globle_depth);
	printf("Status:\n");
	for (int i = 0; i < ndirs; ++i)
	{	
		printf("Dir:");
		print(i);
		
		if (directory->dir_elem_list[i].flag == -1)
		{
			printf("\tLocal Depth:%d\t",(directory->dir_elem_list[i].bucket)->local_depth );
			for (int j = 0; j < bucket_size; ++j)
			{
				if ((directory->dir_elem_list[i].bucket)->bucket_list[j] != -1)
				{
					printf("%d ", (directory->dir_elem_list[i].bucket)->bucket_list[j]);
				}
				
			}
		}
		else{
			printf("\t\t\tPointing to:");
			print(directory->dir_elem_list[i].flag);
		}
		printf("\n");
	}

}

// Initialize a bucket
bckt *initbucket(){
	bckt *bucket = (bckt *)malloc(1*sizeof(bckt));
	bucket->bucket_list = (int *)malloc(bucket_size * sizeof(int ));
	for (int i = 0; i < bucket_size; ++i)
	{
		bucket->bucket_list[i] = -1;
	}
	bucket->local_depth = directory->globle_depth;
	bucket->flag = 0;

	return bucket;
}


// Initialize the directory
void initdir(){
	directory->globle_depth = inital_dir_size;
	int ndirs = (int)pow(2,directory->globle_depth);
	directory->dir_elem_list = (dir_elem *)malloc(ndirs * sizeof(dir_elem ));

	for (int i = 0; i < ndirs; ++i)
	{
		directory->dir_elem_list[i].bucket = initbucket();
		directory->dir_elem_list[i].flag = -1;
	}
}


int inserttobucket(bckt *bucket,int key){
	for (int i = 0; i < bucket_size; ++i)
	{
		if(bucket->bucket_list[i] == -1){
			bucket->bucket_list[i] = key;
			bucket->flag++;
			return 1;
		}
	}

	return 0;
}

// Insertion to mirror image of a bucket
bckt *inserttomirror(bckt *oldbucket,int key){

	bckt *bcktToBeAdded = initbucket();
	oldbucket->local_depth++;
	int tempdepth = bcktToBeAdded->local_depth = oldbucket->local_depth;
	int *tempkey;
	for (int i = 0; i < bucket_size; ++i)
	{
		tempkey = &(oldbucket->bucket_list[i]);
		if((*tempkey >>(tempdepth - 1) & 1)){
			//print_dir();
			inserttobucket(bcktToBeAdded,*tempkey);
			oldbucket->flag--;
			*tempkey = -1;
		}
	}
	if(key>>(tempdepth - 1) & 1){
			//print_dir();
			inserttobucket(bcktToBeAdded,key);
	}
	else{
		inserttobucket(oldbucket,key);
	}

	return bcktToBeAdded;
}

// insertion
void insert(int key){     
	int ndirs = (int)pow(2,directory->globle_depth);
 	int hvalue = key % ndirs;
 	while(directory->dir_elem_list[hvalue].flag != -1){
 		hvalue = directory->dir_elem_list[hvalue].flag;
 	}
	if (inserttobucket(directory->dir_elem_list[hvalue].bucket,key)){
		
		printf("Insert:Insertion Successfull for key %d into dirrectory %d.\n", key,hvalue);
	}
	else if((directory->dir_elem_list[hvalue].bucket)->local_depth == directory->globle_depth){
			
			/*printf("Before directory split.\n");
			print_dir();
			printf("After directory split.\n");*/
			directory->globle_depth++;
			
			int temp = (int)pow(2,directory->globle_depth);
			dir_elem *tempdir = (dir_elem *)malloc((temp)*sizeof(dir_elem ));
			for (int i = 0; i < temp/2; ++i)
			{
				tempdir[i] = directory->dir_elem_list[i];
			}
			for (int i = temp/2; i < temp; ++i)
			{
				//directory->dir_elem_list[i].mirrodir = &directory->dir_elem_list[i-temp/2];
				tempdir[i].flag = i-temp/2;
			}
			free(directory->dir_elem_list);
			directory->dir_elem_list = tempdir;

			directory->dir_elem_list[hvalue + temp/2].bucket = inserttomirror(directory->dir_elem_list[hvalue].bucket,key);
			directory->dir_elem_list[hvalue + temp/2].flag = -1;
			printf("Insert:Insertion (with doubling directory) Successfull for key %d into dirrectory %d.\n", key,hvalue + temp/2);
			//print_dir();
	}
	else{
			/*printf("Before bucket split.\n");
			print_dir();
			printf("After bucket split.\n");*/
			bckt *tempbucket = inserttomirror(directory->dir_elem_list[hvalue].bucket,key);
			int temp = (int)pow(2,directory->dir_elem_list[hvalue].bucket->local_depth);
			directory->dir_elem_list[hvalue + temp/2].bucket = tempbucket;
			directory->dir_elem_list[hvalue + temp/2].flag = -1;
			printf("Insert:Insertion (to mirror image) Successfull for key %d into dirrectory %d.\n", key,hvalue + temp/2);
			//print_dir();
	}
	

}

void search(int key){
	int ndirs = (int)pow(2,directory->globle_depth);
 	int hvalue = key % ndirs;
 	while(directory->dir_elem_list[hvalue].flag != -1){
 		hvalue = directory->dir_elem_list[hvalue].flag;
 	}
 	for (int i = 0; i < bucket_size; ++i)
 	{
 		int *temp = &directory->dir_elem_list[hvalue].bucket->bucket_list[i];
 		if (*temp == key)
 		{

 			printf("Search:Key %d was found in %d \n", key,hvalue);
 			return;
 		}
 	}
 	printf("Search:Key %d does not exists\n",key);
 	return;
}


void deletekey(int key){
	int ndirs = (int)pow(2,directory->globle_depth);
 	int hvalue = key % ndirs;
 	while(directory->dir_elem_list[hvalue].flag != -1){
		hvalue = directory->dir_elem_list[hvalue].flag;
	}
 	for (int i = 0; i < bucket_size; ++i)
 	{
 		int *temp = &directory->dir_elem_list[hvalue].bucket->bucket_list[i];
 		if (*temp == key)
 		{
 			*temp = -1;
 			directory->dir_elem_list[hvalue].bucket->flag--;
 			printf("Delete(0):Key %d was removed from %d lazyly\n", key,hvalue);
 			//print_dir();
 			return;
 		}
 	}
 	printf("Delete(0):Key %d does not exists\n",key);
 	//print_dir();
 	return;
}


void zombiebucket(){

	int ndirs = (int )pow(2,directory->globle_depth);
	for (int i = 0; i < ndirs; ++i)
	{
		if(directory->dir_elem_list[i].flag == -1 && directory->dir_elem_list[i].bucket->flag == 0 ){
				int tempnbuckets = (int)pow(2,directory->dir_elem_list[i].bucket->local_depth);
	 			int mergebucketdepth = directory->dir_elem_list[i].bucket->local_depth;

	 			if (i >= tempnbuckets/2 && mergebucketdepth == directory->dir_elem_list[i - tempnbuckets/2].bucket->local_depth)
	 			{
	 			
	 				free(directory->dir_elem_list[i].bucket);
	 				
	 				directory->dir_elem_list[i].bucket = directory->dir_elem_list[i - tempnbuckets/2].bucket;
	 				directory->dir_elem_list[i].flag = i - tempnbuckets/2;
	 				//directory->dir_elem_list[hvalue - ndirs/2].flag = -1;
	 				//print_dir();
	 				directory->dir_elem_list[i].bucket->local_depth--;
	 				//printf("Bucket %d (zombie) was removed completely\n",i);
	 				//return;
	 			}
	 			else if (i < tempnbuckets/2 && mergebucketdepth == directory->dir_elem_list[i + tempnbuckets/2].bucket->local_depth){
	 				free(directory->dir_elem_list[i].bucket);
	 				directory->dir_elem_list[i].bucket = directory->dir_elem_list[i + tempnbuckets/2].bucket;
	 				directory->dir_elem_list[i + tempnbuckets/2].flag = i;
	 				//directory->dir_elem_list[hvalue].flag = -1;
	 				//print_dir();	
 					directory->dir_elem_list[i].bucket->local_depth--;
 					//printf("Bucket %d (zombie) was removed completely\n",i);
 					//return;
	 				
	 			}

		}
	}
	return;
}


void deletebucket(int key){
	int ndirs = (int)pow(2,directory->globle_depth);
 	int hvalue = key % ndirs;
 	while(directory->dir_elem_list[hvalue].flag != -1){
		hvalue = directory->dir_elem_list[hvalue].flag;
	}
 	bckt *temp = directory->dir_elem_list[hvalue].bucket;
 	if (temp->flag == 1)
 	{
 		for (int i = 0; i < bucket_size; ++i)
	 	{
	 		if (temp->bucket_list[i] == key)
	 		{
	 			temp->bucket_list[i] = -1;
	 			temp->flag--;
	 			ndirs = (int)pow(2,temp->local_depth);
	 			int mergebucketdepth = temp->local_depth;

	 			if (hvalue >= ndirs/2 && mergebucketdepth == directory->dir_elem_list[hvalue - ndirs/2].bucket->local_depth)
	 			{
	 			
	 				free(directory->dir_elem_list[hvalue].bucket);
	 				
	 				directory->dir_elem_list[hvalue].bucket = directory->dir_elem_list[hvalue - ndirs/2].bucket;
	 				directory->dir_elem_list[hvalue].flag = hvalue - ndirs/2;
	 				//directory->dir_elem_list[hvalue - ndirs/2].flag = -1;
	 				//print_dir();
	 				directory->dir_elem_list[hvalue].bucket->local_depth--;
	 				printf("Delete(1):Key %d was removed from %d by deleting complete bucket\n", key,hvalue);
	 				zombiebucket();
	 				return;
	 			}
	 			else if (hvalue < ndirs/2 && mergebucketdepth == directory->dir_elem_list[hvalue + ndirs/2].bucket->local_depth){
	 				free(directory->dir_elem_list[hvalue].bucket);
	 				directory->dir_elem_list[hvalue].bucket = directory->dir_elem_list[hvalue + ndirs/2].bucket;
	 				directory->dir_elem_list[hvalue + ndirs/2].flag = hvalue;
	 				//directory->dir_elem_list[hvalue].flag = -1;
	 				//print_dir();	
 					directory->dir_elem_list[hvalue].bucket->local_depth--;
 					printf("Delete(1):Key %d was removed from %d by deleting complete bucket\n", key,hvalue);
 					zombiebucket();
 					return;
	 				
	 			}
	 			printf("Delete(1):Key %d was removed from %d by deleting lazyly bucket\n", key,hvalue);
	 			//print_dir();
	 			return;
	 		}
	 	}
 	}
 	else{
 		deletekey(key);
 		return;
 	}
}

int isdirreducable(){
	int ndirs = (int )pow(2,directory->globle_depth);
	for (int i = 0; i < ndirs; ++i)
	{
		if (directory->dir_elem_list[i].flag == -1)
		{
			if (directory->dir_elem_list[i].bucket->local_depth == directory->globle_depth)
			{
				return 0 ;
			}
		}

	}
	return 1;
}

void deletedir(int key){
	deletebucket(key);
	int count = 0;
	int ndirs = (int )pow(2,directory->globle_depth);
	while(isdirreducable()){
		dir_elem *tempdir = (dir_elem *)malloc((ndirs/2)*sizeof(dir_elem ));
		for (int i = 0; i < ndirs/2; ++i)
		{
			tempdir[i] = directory->dir_elem_list[i];
		}
		free(directory->dir_elem_list);
		directory->dir_elem_list = tempdir;
		directory->globle_depth--;
		count++;
	}
	if(count){
		printf("Directory is reduced by %d times.\n", count);
	}
	else{
		printf("Directory is not reducable.\n");
	}
	return;
}


int main(int argc, char const *argv[])
{
	
	//int bucket_size, iniital_dir_size;
	//scanf("%d %d",&inital_dir_size,&bucket_size);
	inital_dir_size = atoi(argv[1]);
	bucket_size = atoi(argv[2]);
	directory = (dir *)malloc(1 * sizeof(dir));
	initdir();

	int deleteopt = atoi(argv[3]);

	FILE *input = fopen(argv[4],"r");
	char buf[20];
	while(fgets(buf,20,input)){
		char *token = strtok(buf," \t\n");
		int key;
		while(token != NULL){
			if (strcmp(token,"insert") == 0)
			{
				token = strtok(NULL," \t\n");
				if (key = atoi(token))
				{
					insert(key);
					printf("\n");
				}
				else{
					printf("Invalid charcter\n");
					return 0;
				}
				
			}
			else if (strcmp(token,"delete") == 0)
			{
				token = strtok(NULL," \t\n");
				if (key = atoi(token))
				{
					switch(deleteopt){
						case 0:
							deletekey(key);
							printf("\n");
							break;
						case 1:
							deletebucket(key);
							printf("\n");
							break;
						case 2:
							deletedir(key);
							printf("\n");
							break;
						default:
							printf("Invalid option for deletion.\n");
							return 0;

					}
				}
				else{
					printf("Invalid charcter\n");
					return 0;
				}
			}
			else if (strcmp(token,"search") == 0)
			{
				token = strtok(NULL," \t\n");
				if (key = atoi(token))
				{
					search(key);
					printf("\n");
				}
				else{
					printf("Invalid charcter\n");
					return 0;
				}
			}
			else if (strcmp(token,"status") == 0)
			{
				print_dir();
				printf("\n");
			}
			else{
				printf("Invalid charcter\n");
			}
			token = strtok(NULL," \t\t\n");
		}
	}

	/*insert(1);
	insert(2);
	insert(3);
	insert(4);
	insert(6);
	insert(10);
	insert(26);
	//insert(14);
	//insert(22);
	deletedir(6);
	print_dir();
	deletedir(2);
	print_dir();
	//deletebucket(4);*/
		
	/*
	insert(6);
	insert(1);
	insert(2);
	insert(3);
	insert(4);
	insert(5);
	insert(7);
	insert(8);
	insert(9);
	insert(10);
	insert(11);
	insert(12);
	insert(14);
	insert(16);
	insert(17);
	insert(18);
	insert(40);
	insert(58);
	insert(62);
	print_dir();

		
	//insert(42);
	deletedir(62);
	deletedir(14);
	deletedir(6);
	print_dir();
	
	deletedir(18);
	print_dir();
	deletedir(2);
	print_dir();
	deletedir(10);
	print_dir();
	deletedir(58);
	print_dir();
	deletedir(4);
	print_dir();
	deletedir(12);
	print_dir();
	deletedir(3);
	print_dir();
	deletedir(7);
	print_dir();
	deletedir(11);
	print_dir();
	deletedir(40);
	print_dir();
	deletedir(8);
	print_dir();
	deletedir(16);
	print_dir();
insert(10);
	insert(11);
	insert(12);
	insert(14);
	insert(16);
	insert(17);
	insert(18);
	insert(40);
	insert(58);
	insert(62);
	print_dir();*/
	/*deletedir(12);
	print_dir();
	deletedir(4);
	print_dir();*/

	return 0;
}