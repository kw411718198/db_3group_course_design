#include"utility/p_allocator.h"
#define LAYOUT_NAME "my_layout"
#include<iostream>
using namespace std;

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
        maxFileId=0;
        pAllocator=nullptr;
       // initFilePmemAddr();

    } else {
        // not exist, create catalog and free_list file, then open.
        // TODO
        ifstream allocatorCatalog(allocatorCatalogPath, ios::in|ios::binary);
        ifstream freeListFile(freeListPath, ios::in|ios::binary);
         maxFileId=0;
        pAllocator=nullptr;
    }
    this->initFilePmemAddr();
}
// vector<PPointer>     freeList;  
PAllocator::~PAllocator() {
    // TODO
    freeList.clear();
    fId2PmAddr.clear();
}

// memory map all leaves to pmem address, storing them in the fId2PmAddr
void PAllocator::initFilePmemAddr() {
    // TODO
    PPointer p = startLeaf;
    char *pmemaddr;
    size_t mapped_len;
    int is_pmem;

    for(size_t i=0;i<freeList.size();i++){
        //cout<<freeList[i].fileId;
        char * addr = pmem_map_file(const char *path, size_t len, int flags, mode_t mode, size_t *mapped_lenp, int *is_pmemp);
        fId2PmAddr.insert(map<uint64_t, char*>::value_type(freeList[i].fileId,addr));
    }

}

// get the pmem address of the target PPointer from the map fId2PmAddr
char* PAllocator::getLeafPmemAddr(PPointer p) {
    // TODO
    return fId2PmAddr.find(p.fileId)->second;
//    return NULL;
}

// get and use a leaf
//the fptree leaf allocation
// return 
bool PAllocator::getLeaf(PPointer &p, char* &pmem_addr) {
    // TODO
    return false;
}

bool     PAllocator::ifLeafUsed(PPointer p){
    for(auto it=freeList.begin();it!=freeList.end();it++){
        if(*it== p)
            return false;
    }
    return true;

}
bool PAllocator::ifLeafFree(PPointer p) {
    // TODO
    //vector<PPointer>     freeList;  
    for(auto it=freeList.begin();it!=freeList.end();it++){
        if(*it== p)
            return true;
    }
    return false;
}

// judge whether the leaf with specific PPointer exists. 
bool PAllocator::ifLeafExist(PPointer p) {//if p.fid is in the catalog
    // TODO
    if(ifLeafFree(p) || ifLeafUsed(p))
    return true;
}

// free and reuse a leaf
bool PAllocator::freeLeaf(PPointer p) {
    // TODO

    return false;
}
// persist the catalog file in NVM/SSD
bool PAllocator::persistCatalog() {
    // TODO
    pmem_persist(const void *addr, size_t len);
    return false;
}

/*
  Leaf group structure: (uncompressed)
  | usedNum(8b) | bitmap(n * byte) | leaf1 |...| leafn |
*/
// create a new leafgroup, one file per leafgroup
bool PAllocator::newLeafGroup() {
    // TODO

    return false;
}
//    PPointer getUsedLeaf(int idx);
//    PPointer getFreeLeaf(int idx);
