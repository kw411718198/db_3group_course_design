#include "fptree/fptree.h"
#include <leveldb/db.h>
#include <string>

#define KEY_LEN 8
#define VALUE_LEN 8
using namespace std;

const string workload = "../workloads"; // TODO: the workload folder filepath

const string load = workload + "/220w-rw-50-50-load.txt"; // TODO: the workload_load filename
const string run  = workload + "/220w-rw-50-50-run.txt"; // TODO: the workload_run filename

const int READ_WRITE_NUM = 0; // TODO: amount of operations

int main()
{        
    FPTree fptree(1028);
    uint64_t inserted = 0, queried = 0, t = 0;
    uint64_t* key = new uint64_t[2200000];
    bool* ifInsert = new bool[2200000];
	FILE *ycsb, *ycsb_read;
	char *buf = NULL;
	size_t len = 0;
    struct timespec start, finish;
    double single_time;

    printf("===================FPtreeDB===================\n");
    printf("Load phase begins \n");

    // TODO: read the ycsb_load
	FILE* fp= fopen(load.c_str(),"r");
	char op[8];
	unit64_t key;
	for(int i = 0;i < 2200000; ++i){
		if(fscanf(fp,"%s %lu",op,&key) == EOF)
			break;
		keys[i] = key;
		ifInsert[i] = *op == 'I';
	}
	
    clock_gettime(CLOCK_MONOTONIC, &start);

    // TODO: load the workload in the fptree
	Key kkey;
	Value value;
	for(int i = ;i < 2200000; ++i){
		kkey = keys[i];
		if(ifInsert[i]){
			++inserted;
			fp->insert(kkey,value);
		}else{
			++queried;
			value = fp->find(kkey);
		}
	}
    clock_gettime(CLOCK_MONOTONIC, &finish);
	single_time = (finish.tv_sec - start.tv_sec) * 1000000000.0 + (finish.tv_nsec - start.tv_nsec);
    printf("Load phase fnishes: %d items are inserted \n", inserted);
    printf("Load phase used time: %fs\n", single_time / 1000000000.0);
    printf("Load phase single insert time: %fns\n", single_time / inserted);

	printf("Run phase begins\n");

	int operation_num = 0;
    inserted = 0;		
    // TODO: read the ycsb_run
	FILE* fp= fopen(run.c_str(),"r");
	char op[8];
	unit64_t key;
	for(int i = 0;i < READ_WRITE_NUM; ++i){
		if(fscanf(fp,"%s %lu",op,&key) == EOF)
			break;
		keys[i] = key;
		ifInsert[i] = *op == 'I';
	}
    clock_gettime(CLOCK_MONOTONIC, &start);

    // TODO: operate the fptree
	Key kkey;
	Value value;
	for(int i = ;i < READ_WRITE_NUM; ++i){
		kkey = keys[i];
		if(ifInsert[i]){
			++inserted;
			fp->insert(kkey,value);
		}else{
			++queried;
			value = fp->find(kkey);
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &finish);
	single_time = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Run phase finishes: %d/%d items are inserted/searched\n", inserted, operation_num - inserted);
    printf("Run phase throughput: %f operations per second \n", READ_WRITE_NUM/single_time);	
    
    // LevelDB
    printf("===================LevelDB====================\n");
    const string filePath = "/home/kong/share"; // data storing folder(NVM)

    memset(key, 0, 2200000);
    memset(ifInsert, 0, 2200000);

    leveldb::DB* db;
    leveldb::Options options;
    leveldb::WriteOptions write_options;
    // TODO: initial the levelDB
    leveldb::ReadOptions  read_options;
    leveldb::Status status    = leveldb::DB::Open(options, filePath, &db);
    options.create_if_missing = true;
    assert(status.ok());
	
    inserted = 0;
    printf("Load phase begins \n");
    // TODO: read the ycsb_read
	FILE* fp= fopen(load.c_str(),"r");
	char op[8];
	unit64_t key;
	for(int i = 0;i < 2200000; ++i){
		if(fscanf(fp,"%s %lu",op,&key) == EOF)
			break;
		keys[i] = key;
		ifInsert[i] = *op == 'I';
	}
    clock_gettime(CLOCK_MONOTONIC, &start);
    // TODO: load the levelDB
		char kkey[9] = {0};
	string value;
	for(int i = 0;i < 2200000;i ++){
		memcpy(kkey,key + i,8);
		if(ifInsert(i)){
			++inserted;
			db->Put(write_options,kkey,kkey);
		}else{
			++queried;
			db->Get(read_options,kkey,&value);
		}
	}
    clock_gettime(CLOCK_MONOTONIC, &finish);
	single_time = (finish.tv_sec - start.tv_sec) * 1000000000.0 + (finish.tv_nsec - start.tv_nsec);
    printf("Load phase finishes: %d items are inserted \n", inserted);
    printf("Load phase used time: %fs\n", single_time / 1000000000.0);
    printf("Load phase single insert time: %fns\n", single_time / inserted);

	printf("Run phase begin\n");
	operation_num = 0;
    inserted = 0;		
    // TODO: read the ycsb_run
	FILE* fp= fopen(run.c_str(),"r");
	char op[8];
	unit64_t key;
	for(int i = 0;i < READ_WRITE_NUM; ++i){
		if(fscanf(fp,"%s %lu",op,&key) == EOF)
			break;
		keys[i] = key;
		ifInsert[i] = *op == 'I';
	}
    clock_gettime(CLOCK_MONOTONIC, &start);

    // TODO: run the workload_run in levelDB
	char kkey[9] = {0};
	string value;
	for(int i = 0;i < READ_WRITE_NUM;i ++){
		memcpy(kkey,key + i,8);
		if(ifInsert(i)){
			++inserted;
			db->Put(write_options,kkey,kkey);
		}else{
			++queried;
			db->Get(read_options,kkey,&value);
		}
	}
	
	clock_gettime(CLOCK_MONOTONIC, &finish);
    fclose(ycsb_read);
	single_time = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Run phase finishes: %d/%d items are inserted/searched\n", inserted, operation_num - inserted);
    printf("Run phase throughput: %f operations per second \n", READ_WRITE_NUM/single_time);	
    return 0;
}
