#include"fptree/fptree.h"

using namespace std;

// Initial the new InnerNode
InnerNode::InnerNode(const int& d, FPTree* const& t, bool _isRoot) {
    degree = 0;
    this->isRoot = _isRoot;
    //key = new Key[2*d+1];
    //childrens = new Node*[2*d+1];
    //this-tree = t;
    // TODO
}

// delete the InnerNode
InnerNode::~InnerNode() {
    // TODO
    tree = NULL;
    delete keys;
    delete []childrens;
    degree = 0;
}
/*
    bool   isRoot;     // judge whether the node is root
    int    nKeys;      // amount of keys
    int    nChild;     // amount of children
    Key*   keys;       // max (2 * d + 1) keys
    Node** childrens;  // max (2 * d + 2) node pointers
*/
//stage 3
int BinarySearch(Key a[],const Key& x,int n)
{
    int left = 0;
    int right = n - 1;
    while(left <= right)
    {
        int middle = (left+right)/2;
        if(a[middle] == x)
            return middle;
        if(x >= a[middle])
            left = middle + 1;
        else
            right = middle - 1;
    }
    return -1;
}
//stage 3
// binary search the first key in the innernode larger than input key
int InnerNode::findIndex(const Key& k) {
    // TODO
    //use key(int k) get the index
    int  temp = BinarySearch(keys,k,nKeys);
    return temp;
}


// insert the node that is assumed not full
// insert format:
// ======================
// | key | node pointer |
// ======================
// WARNING: can not insert when it has no entry
void InnerNode::insertNonFull(const Key& k, Node* const& node) {
    // TODO
   int index = findIndex(k);
   nChild++;
   for(int i = nChild;i>index;i--){
       keys[i] = keys[i-1];
       childrens[keys[i]] = childrens[keys[i-1]] ;
   }
   keys[index] = k; 
   childrens[keys[index]] = node;
}

// insert func
// return value is not NULL if split, returning the new child and a key to insert
KeyNode* InnerNode::insert(const Key& k, const Value& v) {
    KeyNode* newChild = NULL;

    // 1.insertion to the first leaf(only one leaf)
    if (this->isRoot && this->nKeys == 0) {//from 0 to 1 leaf-----the first node that's root
        // TODO
        LeafNode* node = NULL;
        node->degree = 1;
        node->n = 1;
        node->kv[0].k = k;
        node->kv[0].v = v;
        node->tree = this->tree;
        newChild->key = k;
        newChild->node = node;        
        return newChild;
    }  
    // 2.recursive insertion
    // TODO
    if(this->ifLeaf())//if leafnodes
    {
        if(this->nChild > this->getKeyNum())
        //still have space
        {
            return insert(k,v);
        }
        else
        {
            if(this->degree <= this->getChildNum())
            //no space ,split to insert
            {
                return this->split();
            }
        }
        
    }
    //not the leaf node reursively
    else
    {
        /* code */
        int index = findIndex(k);
        Node* p = childrens[index];
        p->insert(v,k);
    }
    //InnerNode* root = this->tree->getRoot();
   // return newChild;
}

// ensure that the leaves inserted are ordered
// used by the bulkLoading func
// inserted data: | minKey of leaf | LeafNode* |
KeyNode* InnerNode::insertLeaf(const KeyNode& leaf) {
    KeyNode* newChild = NULL;
    // first and second leaf insertion into the tree
    if (this->isRoot && this->nKeys == 0) {
        // TODO
        LeafNode* node = NULL;
        node->degree = 1;
        node->n = 1;
        this->tree=leaf.node->getTree();    
        newChild->node = leaf.node;   
        return newChild;
    }
    
    // recursive insert
    // Tip: please judge whether this InnerNode is full
    if(this->nChild >= this->getDegree())
        {
            //is full ,spilt
            return this->split();
            //
        }
    // TODO
    for(int i = 0;i < this->nChild; i++)
    {
        if(this->childrens[i]->ifLeaf())//is leaf
        {
    // next level is leaf, insert to childrens array
           // this->childrens[i]->
           this->nChild++;
           this->nKeys++;
           this->childrens[1 + i] = leaf.node;
           this->keys[1 + i] = leaf.key;
           break;
        }
        else 
        {
    // next level is not leaf, just insertLeaf
       //get the child innnernode??????
            for (int i = 0; i < nChild; i ++) {
                if (insertLeaf(leaf)) break;   
            }
        }

    }

    // TODO

    return newChild;
}

KeyNode* InnerNode::split() {
    KeyNode* newChild = new KeyNode();
    // right half entries of old node to the new node, others to the old node. 
    // TODO
    int mid = this->nKeys/2;
    int midkey = this->keys[mid];
    InnerNode* right;
    InnerNode* left = this; 
    int i,j;
    for(i = mid,j = 0;i < this->nKeys;i++){
        if(i == mid)continue;
        right->keys[j] = this->keys[i];
        right->childrens[j] = this->childrens[i];
        right->nKeys++;
    }
    right->childrens[j] = this->childrens[i];
	
	//get the father node to return 
	InnerNode* root = this->tree->getRoot();
	int value = this->keys[nKeys-1];
	
	for(int i = 0;i< root->nKeys - 1;i++){
			int t = root->find(value);
			if(value == root->keys[nKeys-1]){
				newChild->node = root;
				newChild->key = keys[nKeys-1];
				break;
			}
			else//        InnerNode* n = (InnerNode*)node;
				root = (InnerNode*)root->childrens[t];
	}
    return newChild;
}

// remove the target entry
// return TRUE if the children node is deleted after removement.
// the InnerNode need to be redistributed or merged after deleting one of its children node.
bool InnerNode::remove(const Key& k, const int& index, InnerNode* const& parent, bool &ifDelete) {
    bool ifRemove = false;
    // only have one leaf
    // TODO
    int _index = findIndex(k);
    if(_index == 0)
    return false;
    
    ifRemove = childrens[_index-1]->remove(k,_index-1,this,ifDelete);
    if(ifDelete){
        if(_index)
        removeChild(_index-1,_index);
        else removeChild(_index,_index);
        
        if(isRoot && nChild == 1 &&(!childrens[0]->ifLeaf())){
            tree->changeRoot(dynamic_cast<InnerNode*>(childrens[0]));
            tree->getRoot()->isRoot = true;
            ifDelete = true;
            return ifRemove;
        }  


    if(nChild <degree +1 && !isRoot){
        InnerNode *leftBro = nullptr,*rightBro = nullptr;
        getBrother(index,parent,leftBro,rightBro);

        if(parent->isRoot && parent->getChildNum() == 2){
            if(leftBro){
                mergeParentLeft(parent,leftBro);
                ifDelete = true;
            }
            else if (rightBro){
                mergeParentRight(parent,rightBro);
                ifDelete = true;
            }
            return ifRemove;
        }
        if(rightBro){
            mergeRight(rightBro,k);
            parent->keys[index+1] = rightBro->keys[0];
            ifDelete = true;
            return ifRemove;
        }
        if(leftBro){
            mergeLeft(leftBro,k);
            parent->keys[index-1] = rightBro->keys[0];
            ifDelete = true;
            return ifRemove;
        }
        if(rightBro && rightBro->nChild > degree +1){
            redistributeRight(index,rightBro,parent);
            ifRemove = false;
            return ifRemove;
        }
         if(leftBro && leftBro->nChild > degree + 1 ){
            redistributeLeft(index,leftBro,parent);
            ifRemove = false;
            return ifRemove;
        }
        return false;
        }
        ifDelete = false;
        return ifRemove;
    }
    // recursive remove
    // TODO
    keys[0] = (dynamic_cast<InnerNode*>(childrens[0]))->keys[0]; 
    return ifRemove;
}

// If the leftBro and rightBro exist, the rightBro is prior to be used
void InnerNode::getBrother(const int& index, InnerNode* const& parent, InnerNode* &leftBro, InnerNode* &rightBro) {
    // TODO
    if(parent){
        if(index>0){
            leftBro = dynamic_cast<InnerNode*>(parent->childrens[index-1]);
        }
        if(index+1<(int)parent->nChild){
            rightBro = dynamic_cast<InnerNode*>(parent->childrens[index+1]);
        }
    }

}

// merge this node, its parent and left brother(parent is root)
void InnerNode::mergeParentLeft(InnerNode* const& parent, InnerNode* const& leftBro) {
    // TODO
    memmove(leftBro->childrens + leftBro->nChild,childrens,sizeof(Node*) * nChild);
    memmove(leftBro->keys +leftBro->nChild,keys,sizeof(Key) * nChild);
    leftBro->nChild += nChild;
    tree->changeRoot(leftBro);
    leftBro->isRoot = true;
}

// merge this node, its parent and right brother(parent is root)
void InnerNode::mergeParentRight(InnerNode* const& parent, InnerNode* const& rightBro) {
    // TODO
    memmove(rightBro->childrens+this->nChild,rightBro->childrens,sizeof(Node*)*rightBro->nChild);
    memmove(rightBro->keys +this->nChild,rightBro->keys,sizeof(Key)*rightBro->nChild);
    memmove(rightBro->childrens,childrens,sizeof(Node*) * nChild);
    memmove(rightBro->keys,keys,sizeof(Key) * nChild);
    rightBro->nChild += nChild;
    tree->changeRoot(rightBro);
    rightBro->isRoot = true;
}

// this node and its left brother redistribute
// the left has more entries
void InnerNode::redistributeLeft(const int& index, InnerNode* const& leftBro, InnerNode* const& parent) {
    // TODO
    memmove(childrens+1,childrens,sizeof(Node*) * nChild);
    memmove(keys+1,keys,sizeof(Key)*nChild);
    leftBro->nChild-=1;
    keys[0] = leftBro->keys[leftBro->nChild];
    childrens[0] = leftBro->childrens[leftBro->nChild];
    nChild++;

    parent->keys[index] = this->keys[0];
}

// this node and its right brother redistribute
// the right has more entries
void InnerNode::redistributeRight(const int& index, InnerNode* const& rightBro, InnerNode* const& parent) {
    // TODO
    memmove(rightBro->childrens,rightBro->childrens+1,sizeof(Node*) * rightBro->nChild);
    memmove(rightBro->keys,rightBro->keys+1,sizeof(Key)*rightBro->nChild);
    rightBro->nChild-=1;
    keys[nChild] = rightBro->keys[0];
    childrens[nChild] = rightBro->childrens[0];
    nChild++;

    parent->keys[index+1] = this->keys[0];
}

// merge all entries to its left bro, delete this node after merging.
void InnerNode::mergeLeft(InnerNode* const& leftBro, const Key& k) {
    // TODO
    memmove(leftBro->childrens +leftBro->nChild,childrens,sizeof(Node*) * nChild);
    memmove(leftBro->keys + leftBro->nChild,keys,sizeof(Key)* nChild);
    leftBro->nChild*=2;
    return;
}

// merge all entries to its right bro, delete this node after merging.
void InnerNode::mergeRight(InnerNode* const& rightBro, const Key& k) {
    // TODO
    memmove(rightBro->childrens + nChild,rightBro->childrens,sizeof(Node*)*rightBro->nChild);
    memmove(rightBro->keys+nChild,rightBro->keys,sizeof(Key)*rightBro->nChild);
    memmove(rightBro->childrens,childrens,sizeof(Node*)*nChild);
    memmove(rightBro->keys,keys,sizeof(Key*)*nChild);
    rightBro->nChild*=2;
    return;
}

// remove a children from the current node, used by remove func
void InnerNode::removeChild(const int& keyIdx, const int& childIdx) {
    // TODO
    delete this->childrens[childIdx];
    nChild--;
    memmove(keys+childIdx,keys+childIdx+1,sizeof(Key*)*(nChild-childIdx));
    memmove(childrens+childIdx,childrens+childIdx+1,sizeof(Node*)*(nChild-childIdx));
    return;
}
//stage 3
// update the target entry, return true if the update succeed.
bool InnerNode::update(const Key& k, const Value& v) {
    // TODO
    return recursive_update(this->tree->getRoot(),k,v);
 //   return false;
}
bool InnerNode::recursive_update(Node* node ,const Key& k,const Value& v){
    if(node == NULL){
        return false;
    }
    else{
        InnerNode* n =  (InnerNode*)node;
        int keyIndex =  n->findIndex(k);//get the branch index by key k
        if(n->childrens[keyIndex]->ifLeaf())//is a leaf node
        {
            return n->childrens[keyIndex]->update(k,v);
        }
        else
        {
            return recursive_update(n->childrens[keyIndex],k,v);//recursive to the children node to find the k
        }
    }
    return false;
}
//stage 3
// find the target value with the search key, return MAX_VALUE if it fails.
Value InnerNode::find(const Key& k) {
    // TODO
    return recursive_search(this->tree->getRoot(),k);

//    return MAX_VALUE;
}
Value InnerNode::recursive_search(Node* node ,const Key& k){
    if(node == NULL){
        return false;
    }
    else{
        InnerNode* n = (InnerNode*)node;
        int keyIndex = n->findIndex(k);//get the branch index by key k
        if(n->childrens[keyIndex]->ifLeaf())//is a leaf node
        {
            return n->childrens[keyIndex]->find(k);
        }
        else
        {
            return recursive_search(n->childrens[keyIndex],k);//recursive to the children node to find the k
        }
        
    }
    return MAX_VALUE;
}
//if the leaf is the child of the innerNode return  true
bool     InnerNode::ifChild(LeafNode* node){
    int n = node->getNum();
    int max_key = node->getKey(n-1);
    int min_key = node->getKey(0);
    int findIndex(const Key& k);
    int max_index = this->findIndex(max_key);
    int min_index = this->findIndex(min_key);
    if(max_index || min_index)
        return true;//if the side_key can be found in the keys[],which means the leafnode is the child of the node 
    return false;
}

// get the children node of this InnerNode
Node* InnerNode::getChild(const int& idx) {
    // TODO
    return this->childrens[idx];
//    return NULL;
}

// get the key of this InnerNode
Key InnerNode::getKey(const int& idx) {
    if (idx < this->nKeys) {
        return this->keys[idx];
    } else {
        return MAX_KEY;
    }
}

// print the InnerNode
void InnerNode::printNode() {
    cout << "||#|";
    for (int i = 0; i < this->nKeys; i++) {
        cout << " " << this->keys[i] << " |#|";
    }
    cout << "|" << "    ";
}

// print the LeafNode
void LeafNode::printNode() {
    cout << "||";
    for (int i = 0; i < 2 * this->degree; i++) {
        if (this->getBit(i)) {
            cout << " " << this->kv[i].k << " : " << this->kv[i].v << " |";
        }
    }
    cout << "|" << " ====>> ";
}

// new a empty leaf and set the valuable of the LeafNode
LeafNode::LeafNode(FPTree* t) {
    // TODO
	n=0;//t->getDegree();
	//t->insertLeaf(this);
 }

// reload the leaf with the specific Persistent Pointer
// need to call the PAllocator
LeafNode::LeafNode(PPointer p, FPTree* t) {
    // TODO
	pPointer = p;
	n=0;
	//t->insertLeaf(this);
	/*next->prev = this->prev;
	prev->next = this->next;// the address of previous leafnode   	
	delete p;
	t->remove(this);*/
}

LeafNode::~LeafNode() {
    // TODO
//	delete PPointer;
}

// insert an entry into the leaf, need to split it if it is full
KeyNode* LeafNode::insert(const Key& k, const Value& v) {
    KeyNode* newChild = NULL;
    // TODO
    if(this->n >= this->degree){
        return split();
    }
else
{
    /* code */
    insertNonFull(k,v);
   // return NULL;
}

}
/*
    if (root != NULL) {
        root->insert(k, v);
    }
*/
// insert into the leaf node that is assumed not full
void LeafNode::insertNonFull(const Key& k, const Value& v) {
    // TODO 
    //KeyValue kv
    uint64_t index;

	for(uint64_t i = 0; i < bitmapSize; i ++){
		for(uint64_t j = 0; j < 8; j ++) {
			if(!((bitmap[i] >> j) & 1)) {
				index =  (j + i * 8);
			}
		}
	}
    index = -1;
    uint64_t _index = 1 << (index%8);
    bitmap[index/8] |= _index;
    fingerprints[index] = keyHash(k);
    kv[index] = {k,v};
    n++;
}
//ifChild(leafNode * node)
KeyNode* LeafNode::getFatherNode(Node* root,KeyValue k){
    //if(node)?????????????
    InnerNode* node =  (InnerNode*)root;
    if(root->ifLeaf())
        return NULL;
    else{
        //if(node->ifChild(this))//how to use current class pointor to call another class'function ifChild(leafnode *node) 
        //which means let node == current class pointor
    }
    return NULL;
}
// split the leaf node
KeyNode* LeafNode::split() {
    KeyNode* newChild = new KeyNode();

        LeafNode* newNode;
        this->n = degree / 2;
        newNode->n = n + 1;
        next = newNode;
        prev = this;
        int i;
        for(i = 0;i < degree + 1; i++){
            newNode->kv[i] = this->kv[i];
        }
        //how to get it's fatherNode?????
		
			//get the father node to return 
		InnerNode* root = this->tree->getRoot();
        LeafNode* node =  (LeafNode*)root;
        int min_key = this->kv[0].k;//left key
        int max_key = this->kv[n-1].k;//right key
//????????????????????????
    // TODO
    return newChild;
}
//stage 2
// use to find a mediant key and delete entries less then middle
// called by the split func to generate new leaf-node
// qsort first then find
Key LeafNode::findSplitKey() {
    Key midKey = 0;
    // TODO
//????????????????????

    return midKey;
}

// get the targte bit in bitmap
// TIPS: bit operation
int LeafNode::getBit(const int& idx) {
    // TODO
    return 0;
}

Key LeafNode::getKey(const int& idx) {
    return this->kv[idx].k;
}

Value LeafNode::getValue(const int& idx) {
    return this->kv[idx].v;
}

PPointer LeafNode::getPPointer() {
    return this->pPointer;
}

// remove an entry from the leaf
// if it has no entry after removement return TRUE to indicate outer func to delete this leaf.
// need to call PAllocator to set this leaf free and reuse it
bool LeafNode::remove(const Key& k, const int& index, InnerNode* const& parent, bool &ifDelete) {
    bool ifRemove = false;
    // TODO
    return ifRemove;
}
//stage 3
// update the target entry
// return TRUE if the update succeed
bool LeafNode::update(const Key& k, const Value& v) {
    bool ifUpdate = false;
    // TODO
    for(int i = 0;i < n; i++){
        if(this->kv[i].k = k){
            this->kv[i].v = v;
            ifUpdate = true;
        }
    }
    return ifUpdate;
}
//stage 3
// if the entry can not be found, return the max Value
Value LeafNode::find(const Key& k) {
    // TODO
 //    KeyValue*  kv;             // the keyValue pairs array
    for(int i = 0;i < n; i++){
        if(this->kv[i].k == k)
            return kv[i].v;
    }
    return MAX_VALUE;
}
//stage 3
// find the first empty slot
int LeafNode::findFirstZero() {
    // TODO
    if(this->degree >= n)
        return n;
    return -1;
}

// persist the entire leaf
// use PMDK
void LeafNode::persist() {
    // TODO
    pmem_msync(pmem_addr,calLeafSize());
}

/*
friend class FPTree;
  bool   isRoot;     // judge whether the node is root
    int    nKeys;      // amount of keys
    int    nChild;     // amount of children
    Key*   keys;       // max (2 * d + 1) keys
    Node** childrens; 
*/


// call by the ~FPTree(), delete the whole tree
void FPTree::recursiveDelete(Node* n) {
    if (n->isLeaf) {
        delete n;
    } else {
        for (int i = 0; i < ((InnerNode*)n)->nChild; i++) {
            recursiveDelete(((InnerNode*)n)->childrens[i]);
        }
        delete n;
    }
}

FPTree::FPTree(uint64_t t_degree) {
    FPTree* temp = this;
    this->root = new InnerNode(t_degree, temp, true);
    this->degree = t_degree;
    bulkLoading();
}

FPTree::~FPTree() {
    recursiveDelete(this->root);
}

// get the root node of the tree
InnerNode* FPTree::getRoot() {
    return this->root;
}

// change the root of the tree
void FPTree::changeRoot(InnerNode* newRoot) {
    this->root = newRoot;
}

void FPTree::insert(Key k, Value v) {
    if (root != NULL) {
        root->insert(k, v);
    }
}

bool FPTree::remove(Key k) {
    if (root != NULL) {
        bool ifDelete = false;
        InnerNode* temp = NULL;
        return root->remove(k, -1, temp, ifDelete);
    }
    return false;
}

bool FPTree::update(Key k, Value v) {
    if (root != NULL) {
        return root->update(k, v);
    }
    return false;
}

Value FPTree::find(Key k) {
    if (root != NULL) {
        return root->find(k);
    }
    return MAX_VALUE;
}

// call the InnerNode and LeafNode print func to print the whole tree
// TIPS: use Queue
void FPTree::printTree() {
    // TODO
    queue<Node*> print;
    print.push(root);
    while(!print.empty()){
        Node* toPrint = print.front();
        print.pop();
        toPrint->printNode();
        if(!toPrint->isLeaf){
            for(int i = 0; i < ((InnerNode*)toPrint)->nChild; i ++ ){
                print.push(((InnerNode*)toPrint)->childrens[i]);
            }
        }
        cout << endl;
    }
}

// bulkLoading the leaf files and reload the tree
// need to traverse leaves chain
// if no tree is reloaded, return FALSE
// need to call the PALlocator
bool FPTree::bulkLoading() {
    // TODO
    PPointer p = PAllocator::getAllocator()->getStartPointer();
    if (!p.fileId) {
        this->root = new InnerNode(degree, this, true);
        return false;
    }
    LeafNode* tempLeaf = new LeafNode(p, this);
    queue<Node*> q;
    size_t length1;
    for (length1 = 0; tempLeaf; tempLeaf = tempLeaf->next) {
        q.push(tempLeaf);
        length1 ++;
    }

    while (q.front()->ifLeaf()) {
        if (q.size() == 0 ||  q.size() == 1) break;
        InnerNode* tempNode = new InnerNode(degree, this);
        size_t size;
        if (length1 < 2 * degree + 1) 
            size = length1;
        else size = degree;
        vector<LeafNode*> leafNodes;
        for(uint64_t i = 0;i<leafNodes.size();i++){
            root->insertLeaf({leafNodes[i]->getKey(0),leafNodes[i]});
        }

        q.push(tempNode);
        size_t length2 = 1;
        if (!length1) {
            length1 = length2;
            length2 = 0;
        }
    }

    this->root = dynamic_cast<InnerNode*>(q.front());
    this->root->isRoot = true;
    return true;
}
