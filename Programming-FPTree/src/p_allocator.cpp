#include"utility/p_allocator.h"
#define LAYOUT_NAME "my_layout"
#include<iostream>
#include<cstring>
#include<stdio.h>
#include<stdlib.h>
using namespace std;
#define LEAF_SIZE calLeafSize()

// the file that store the information of allocator
const string P_ALLOCATOR_CATALOG_NAME = "p_allocator_catalog";
// a list storing the free leaves
const string P_ALLOCATOR_FREE_LIST    = "free_list";

PAllocator* PAllocator::pAllocator = new PAllocator();

PAllocator* PAllocator::getAllocator() {
    if (PAllocator::pAllocator == NULL) {
        PAllocator::pAllocator = new PAllocator();
    }
    return PAllocator::pAllocator;
}

/* data storing structure of allocator
   In the catalog file, the data structure is listed below
   | maxFileId(8 bytes) | freeNum = m | treeStartLeaf(the PPointer) |
   In freeList file:
   | freeList{(fId, offset)1,...(fId, offset)m} |
*/
PAllocator::PAllocator() {
    string allocatorCatalogPath = DATA_DIR + P_ALLOCATOR_CATALOG_NAME;
    string freeListPath         = DATA_DIR + P_ALLOCATOR_FREE_LIST;
    ifstream allocatorCatalog(allocatorCatalogPath, ios::in|ios::binary);
    ifstream freeListFile(freeListPath, ios::in|ios::binary);
    // judge if the catalog exists
    if (allocatorCatalog.is_open() && freeListFile.is_open()) {
        // exist
        // TODO
        //read
        allocatorCatalog.read((char*)&(maxFileId),sizeof(uint64_t));
        allocatorCatalog.read((char*)&(freeNum),sizeof(uint64_t));
        allocatorCatalog.read((char*)&(startLeaf),sizeof(uint64_t));
        for (uint64_t i = 0; i<freeNum;i ++) {
            PPointer p;
            freeListFile.read((char*)&(p), sizeof(PPointer));
            freeList.push_back(p);
        }
       // initFilePmemAddr();
        allocatorCatalog.close();
        freeListFile.close();

    } else {
        // not exist, create catalog and free_list file, then open.
        // TODO
        ofstream allocatorCatalog(allocatorCatalogPath, ios::out|ios::binary);
        ofstream freeListFile(freeListPath, ios::out|ios::binary);
    }
    this->initFilePmemAddr();
}
// vector<PPointer>     freeList;  
PAllocator::~PAllocator() {
    // TODO
    persistCatalog();
    fId2PmAddr.clear();
    PAllocator::pAllocator = nullptr;
}
int PAllocator::getLeafGroupSize(){
    return LEAF_GROUP_HEAD + LEAF_SIZE * LEAF_GROUP_AMOUNT;
}
// memory map all leaves to pmem address, storing them in the fId2PmAddr
void PAllocator::initFilePmemAddr() {
    // TODO
    char *pmemaddr;
    size_t mapped_len;
    int is_pmem;
   string freeListPath         = DATA_DIR + P_ALLOCATOR_FREE_LIST;
    fId2PmAddr.clear();
    for(uint64_t i = 1; i <getMaxFileId();i ++){
        string filePath = DATA_DIR + to_string(i);
        fId2PmAddr.emplace(i, (char *)pmem_map_file(filePath.c_str(),calLeafSize(), PMEM_FILE_CREATE, 0666, NULL, NULL));
    }
}

// get the pmem address of the target PPointer from the map fId2PmAddr
char* PAllocator::getLeafPmemAddr(PPointer p) {
    // TODO
    return fId2PmAddr.find(p.fileId)->second+p.offset;
//    return NULL;
}

// get and use a leaf
//the fptree leaf allocation
// return 
bool PAllocator::getLeaf(PPointer &p, char* &pmem_addr) {
    if (getFreeNum() == 0) {
        if (!newLeafGroup()) return false;
    }
	p = freeList.back();
	freeList.pop_back();
	freeNum --;

	uint64_t usedNum_temp;
	Byte bitmap_temp = 1;
	memcpy(&usedNum_temp, fId2PmAddr[p.fileId], sizeof(uint64_t));
	usedNum_temp ++;
	memcpy(fId2PmAddr[p.fileId], &usedNum_temp, sizeof(uint64_t));
	memcpy(fId2PmAddr[p.fileId] + sizeof(uint64_t) + (p.offset - LEAF_GROUP_HEAD) / LEAF_SIZE, &bitmap_temp, sizeof(Byte));

	pmem_addr = fId2PmAddr[p.fileId] + p.offset;

	if(!startLeaf.fileId){
		startLeaf = p;
	}
    return true;
}

bool PAllocator::ifLeafUsed(PPointer p){
    if(ifLeafExist(p)){
		Byte ifUsed;
		memcpy(&ifUsed, fId2PmAddr[p.fileId] + sizeof(uint64_t) + (p.offset - LEAF_GROUP_HEAD)/LEAF_SIZE, sizeof(Byte));
		return (ifUsed == 1 ? true : false);
	}
    else return false;
}
bool PAllocator::ifLeafFree(PPointer p) {
    // TODO
    return !ifLeafUsed(p); 
}

// judge whether the leaf with specific PPointer exists. 
bool PAllocator::ifLeafExist(PPointer p) {//if p.fid is in the catalog
    // TODO
    if(fId2PmAddr.count(p.fileId) && fId2PmAddr.count(p.fileId) != '\0'){
        return true;
    }
    return false;
    
}

// free and reuse a leaf
bool PAllocator::freeLeaf(PPointer p) {
    // TODO
    if(ifLeafExist(p)){
        memset(fId2PmAddr[p.fileId],'\0',sizeof(p.offset));
         freeNum ++;
        return true;
    }
    return false;
}
// persist the catalog file in NVM/SSD
bool PAllocator::persistCatalog() {
    // TODO
    string freeListPath         = DATA_DIR + P_ALLOCATOR_FREE_LIST;
    ifstream freelist(freeListPath);
    if(freelist.is_open()){
        return false;
    }
    ofstream freeListFile(freeListPath, ios::ate|ios::out|ios::binary);
    for(uint64_t i = 0;i < freeNum;i ++){
        freeListFile.write((char*)&(freeList[i]), sizeof(PPointer));
    }
    return true;
}

/*
  Leaf group structure: (uncompressed)
  | usedNum(8b) | bitmap(n * byte) | leaf1 |...| leafn |
*/
// create a new leafgroup, one file per leafgroup
bool PAllocator::newLeafGroup() {
    // TODO
	string path;
	path = DATA_DIR + to_string(this->maxFileId);
	char *fId2PmAddr_temp = (char *)pmem_map_file(path.c_str(), getLeafGroupSize(), PMEM_FILE_CREATE, 0666, NULL, NULL);
	if(!fId2PmAddr_temp){
    	return false;
	}

	uint64_t usedNum_temp = 0;
	memset(fId2PmAddr_temp, 0, getLeafGroupSize());
	memcpy(fId2PmAddr_temp, &usedNum_temp, sizeof(uint64_t));

	this->fId2PmAddr.emplace(this->maxFileId, fId2PmAddr_temp);

	
	for(int i = 0; i < LEAF_GROUP_AMOUNT; i ++){
		PPointer freeLeaf_temp{this->maxFileId, LEAF_GROUP_HEAD + LEAF_SIZE * i};
		this->freeList.emplace_back(freeLeaf_temp);
	}

	this->maxFileId++;
	this->freeNum += LEAF_GROUP_AMOUNT;
	
	return true;
}
//    PPointer getUsedLeaf(int idx);
//    PPointer getFreeLeaf(int idx);
