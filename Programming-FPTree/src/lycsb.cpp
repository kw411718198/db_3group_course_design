#include <leveldb/db.h>
#include <string>
#include <fstream>

#define KEY_LEN 8
#define VALUE_LEN 8
using namespace std;

const string workload = "../workloads";

const string load = workload + "/1w-rw-50-50-load.txt"; // TODO: the workload_load filename
const string run  = workload + "/1w-rw-50-50-run.txt"; // TODO: the workload_run filename

const string filePath = "";

const int READ_WRITE_NUM = 0; // TODO: how many operations

int main()
{        
    leveldb::DB* db;
    leveldb::Options options;
    leveldb::WriteOptions write_options;
    // TODO: open and initial the levelDB
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, "./testdb", &db);
    assert(status.ok());

    uint64_t inserted = 0, queried = 0, t = 0;
    string* key = new string[2200000]; // the key and value are same
    bool* ifInsert = new bool[2200000]; // the operation is insertion or not
	//FILE *ycsb_load, *ycsb_run; // the files that store the ycsb operations
	char *buf = NULL;
	size_t len = 0;
    struct timespec start, finish; // use to caculate the time
    double single_time; // single operation time

    printf("Load phase begins \n");
    // TODO: read the ycsb_load and store
    ifstream ycsb_load(load);
    if (! ycsb_load.is_open()) {
	printf("fail to open file\n");
        return -1;
    }
    
    string operation;

    while (!ycsb_load.eof()) {
        ycsb_load >> operation;
        if (operation == "INSERT")  
        {
            ifInsert[queried] = true;
            inserted ++;
        }
        else ifInsert[queried] = false;
        ycsb_load >> key[queried];
        queried ++;
    }

    
    clock_gettime(CLOCK_MONOTONIC, &start);

    // TODO: load the workload in LevelDB
    string k, v;
    for (int i = 0; i < queried; i ++) {
        status = db->Put(leveldb::WriteOptions(), k, k);
        assert(status.ok());
    }

    clock_gettime(CLOCK_MONOTONIC, &finish);
	single_time = (finish.tv_sec - start.tv_sec) * 1000000000.0 + (finish.tv_nsec - start.tv_nsec);

    printf("Load phase finishes: %I64u items are inserted \n", inserted);
    printf("Load phase used time: %fs\n", single_time / 1000000000.0);
    printf("Load phase single insert time: %fns\n", single_time / inserted);

	int operation_num = 0;
    inserted = 0;		

    string * operations = new string[2200000];
    // TODO:read the ycsb_run and store
    ifstream ycsb_run(run);
    if (! ycsb_run.is_open()) {
	printf("fail to open file\n");
        return -1;
    }
    
    while (! ycsb_run.eof()) {
        operations[operation_num] = operation;
        ycsb_run >> key[operation_num];
        operation_num ++;
    }

    clock_gettime(CLOCK_MONOTONIC, &start);

    // TODO: operate the levelDB
    string temValue;
    for (int i = 0; i < operation_num; i ++) {
        if (operations[i] == "READ") {
            status = db->Get(leveldb::ReadOptions(), key[i], &temValue);
        }
        else if (operations[i] == "UPDATE") {
            status = db->Delete(leveldb::WriteOptions(), key[i]);
            if (status.ok()) {
                db->Put(leveldb::WriteOptions(), key[i], key[i]);
            }
        }
    }

	clock_gettime(CLOCK_MONOTONIC, &finish);
	single_time = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Run phase finishes: %I64u/%I64u items are inserted/searched\n", operation_num - inserted, inserted);
    printf("Run phase throughput: %f operations per second \n", operation_num/single_time);	
    return 0;
}
