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
        catalog = std::move(pmem_ptr<allocator_catalog>{catalogPath});

        ifstream freelist(freeListFile, ios::in|ios::binary);
        for (uint64_t i = 0; !freelist.eof();i ++) {
            PPointer p;
            freelist >> p;
            freeList.push_back(p);
        }
       // initFilePmemAddr();

    } else {
        // not exist, create catalog and free_list file, then open.
        // TODO
        ofstream allocatorCatalog(allocatorCatalogPath, ios:out|ios::binary);
        ofstream freeListFile(freeListPath, ios::out|ios::binary);
    }
    this->initFilePmemAddr();
}
// vector<PPointer>     freeList;  
PAllocator::~PAllocator() {
    // TODO
    fId2PmAddr.clear();
    PAllocator::pAllocator = nullptr;
}

// memory map all leaves to pmem address, storing them in the fId2PmAddr
void PAllocator::initFilePmemAddr() {
    // TODO
    char *pmemaddr;
    size_t mapped_len;
    int is_pmem;
   string freeListPath         = DATA_DIR + P_ALLOCATOR_FREE_LIST;
    bool flag = freeListPath.is_open();
    fId2PmAddr.clear();
    for(unint64_t i=0;i<freeList.size();i++){
        //cout<<freeList[i].fileId;
        char * addr = pmem_map_file(freeListPath, freeNum, flag, 0666, &mapped_len, &is_pmem);
        fId2PmAddr.insert(map<uint64_t, char*>::value_type(freeList[i].fileId),addr);
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
   if(getFreeNum() == 0){
        if(!newLeafGroup())return false;
   }
    uint64_t n = fId2PmAddr[p.fileId++];
    pmem_addr = n + p.offset;
    return true;
}

bool PAllocator::ifLeafUsed(PPointer p){
    bitmap[(p.offset - LEAF_GROUP_AMOUNT) / sizeof(leaf)];
    return ifLeafExist(p) && ((p.offset - LEAF_GROUP_AMOUNT) / sizeof(leaf) < usedNum?true:false);

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
        memset(fId2PmAddr[p.fileId],'\0',sizeof(p.offset))
         freeNum ++;
        return true;
    }
    return false;
}
// persist the catalog file in NVM/SSD
bool PAllocator::persistCatalog() {
    // TODO
    string freeListPath         = DATA_DIR + P_ALLOCATOR_FREE_LIST;
    ifstream freelist(freeListPath));
    if(freelist.is_open()){
        return false;
    }
    for(unit64_t i = 0;i < freeNum;i ++){
        freelist <<freeList[i];
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
    persistent_ptr<leaf_group> group(getMaxFileId(),0)
    if(!group)
        return false;
    for(size_t i = 0;i < LEAF_GROUP_AMOUNT;i++){
        freeList.push_back(new PPoint().offset);
    }

    fId2PmAddr.insert(getMaxFileId(), group);
    maxFileId++;
    return true;
}
//    PPointer getUsedLeaf(int idx);
//    PPointer getFreeLeaf(int idx);
