#include <iostream>
#include <queue>
#include <stack>
#include <vector>
#include <string>

// B+树节点基类
template<typename KeyType>
class BPlusTreeNode {
public:
    virtual bool isLeaf() const = 0; // 纯虚函数
    virtual ~BPlusTreeNode() {}   // 析构函数
    std::vector<KeyType> keys;
};

// 分支节点
template<typename KeyType>
class InternalNode : public BPlusTreeNode<KeyType> {
public:
    bool isLeafNode;
    std::vector<BPlusTreeNode<KeyType> *> children; // vector存指针

    InternalNode() : isLeafNode(false) {}  // 非空构造函数
    bool isLeaf() const override {
        return isLeafNode;
    }
};

// 叶子节点
template<typename KeyType>
class LeafNode : public BPlusTreeNode<KeyType> {
public:
    bool isLeafNode;
    std::vector<uint64_t> values;
    LeafNode<KeyType> *next;
    LeafNode<KeyType> *pre;

    LeafNode() : isLeafNode(true) {}

    bool isLeaf() const override {
        return isLeafNode;
    }
};

// B+树
template<typename KeyType>
class BPlusTree {
private:
    BPlusTreeNode<KeyType> *root;

    int MAX_KEYS;

    int MIN_KEYS;
    
    // 创建新的父节点
    void doInsert(BPlusTreeNode<KeyType> *&root, const KeyType &key, uint64_t value, BPlusTreeNode<KeyType> *parent,
                  int pos) {
        BPlusTreeNode<KeyType> *&temp = root;
        int size = temp->keys.size(); // 记录当前需要处理的元素个数
        for (decltype(temp->keys.size()) i = 0; i < size; i++) {
            if (!temp->isLeaf()) { // 分支节点
                if (key <= temp->keys[i] || i == (temp->keys.size() - 1)) { // 第一个小或到最后
                    if (key > temp->keys[i]) { // 需要更新路径上的最大节点信息
                        temp->keys[i] = key;
                    }
                    auto Node = static_cast<InternalNode<KeyType> *>(temp)->children[i];
                    doInsert(Node, key, value, temp, i); // 递归查找
                    if (temp->keys.size() > MAX_KEYS) { //当前父节点是否需要分裂
                        splitInternalNode(parent, pos, temp);
                    }
                    break;
                }
            } else { // 叶子节点

                // 将元素按顺序插入
                LeafNode<KeyType> *leafNode = insertElement(key, value, temp);

                // 大于M个则开始分裂
                if (leafNode->keys.size() > MAX_KEYS) {
                    // 创建右子树
                    LeafNode<KeyType> *rightNode = createRightLeafNode(leafNode);

                    // 重组
                    if (parent == nullptr) { // 非叶根节点至少有两个元素
                        // 创建新父节点
                        InternalNode<KeyType> *newRoot = new InternalNode<KeyType>();
                        newRoot->keys.push_back(leafNode->keys[MAX_KEYS / 2]);
                        newRoot->children.push_back(leafNode);
                        newRoot->keys.push_back(leafNode->keys[MAX_KEYS]);
                        newRoot->children.push_back(rightNode);
                        root = newRoot;

                        // 删除左子树的多余的元素
                        leafNode->keys.erase(leafNode->keys.begin() + (MAX_KEYS + 1) / 2, leafNode->keys.end());
                        leafNode->values.erase(leafNode->values.begin() + (MAX_KEYS + 1) / 2, leafNode->values.end());
                    } else {
                        // 删除左子树多余元素
                        leafNode->keys.erase(leafNode->keys.begin() + (MAX_KEYS + 1) / 2, leafNode->keys.end());
                        leafNode->values.erase(leafNode->values.begin() + (MAX_KEYS + 1) / 2, leafNode->values.end());

                        // 父节点更新节点元素
                        static_cast<InternalNode<KeyType> *>(parent)->children.insert(
                                static_cast<InternalNode<KeyType> *>(parent)->children.begin() + pos + 1, rightNode);
                        parent->keys.erase(parent->keys.begin() + pos);
                        parent->keys.insert(parent->keys.begin() + pos, leafNode->keys.back());
                        parent->keys.insert(parent->keys.begin() + pos + 1, rightNode->keys.back());
                    }

                    // 叶子节点连接起来
                    rightNode->next = leafNode->next;
                    leafNode->next = rightNode;
                    rightNode->pre = leafNode;
                } else {
                    // 新增的元素，大于分支节点中记录的元素，则更新
                    if (parent != nullptr && parent->keys[pos] < key) {
                        parent->keys[pos] = key;
                    }
                }
                return;
            }
        }
    }

    InternalNode<KeyType> *createRightInternalNode(const InternalNode<KeyType> *parentNode) const {
        decltype(BPlusTreeNode<KeyType>::keys) rightKey(parentNode->keys.begin() + (MAX_KEYS + 1) / 2,
                                                        parentNode->keys.end());
        decltype(InternalNode<KeyType>::children) rightChildren(parentNode->children.begin() + (MAX_KEYS + 1) / 2,
                                                                parentNode->children.end());
        InternalNode<KeyType> *rightNode = new InternalNode<KeyType>();
        rightNode->keys = rightKey;
        rightNode->children = rightChildren;
        return rightNode;
    }

    LeafNode<KeyType> *createRightLeafNode(const LeafNode<KeyType> *leafNode) const {
        decltype(BPlusTreeNode<KeyType>::keys) rightKey(leafNode->keys.begin() + (MAX_KEYS + 1) / 2,
                                                        leafNode->keys.end());
        decltype(LeafNode<KeyType>::values) rightValues(leafNode->values.begin() + (MAX_KEYS + 1) / 2,
                                                        leafNode->values.end());
        LeafNode<KeyType> *rightNode = new LeafNode<KeyType>();
        rightNode->keys = rightKey;
        rightNode->values = rightValues;
        return rightNode;
    }

    LeafNode<KeyType> *insertElement(const KeyType &key, uint64_t value, BPlusTreeNode<KeyType> *const &temp) const {
        LeafNode<KeyType> *leafNode = static_cast<LeafNode<KeyType> *>(temp);
        for (decltype(leafNode->keys.size()) j = 0; j < leafNode->keys.size(); j++) {
            if (key <= leafNode->keys[j]) {
                leafNode->keys.insert(leafNode->keys.begin() + j, key);
                leafNode->values.insert(leafNode->values.begin() + j, value);
                return leafNode;
            }
        }
        leafNode->keys.insert(leafNode->keys.end(), key);
        leafNode->values.insert(leafNode->values.end(), value);
        return leafNode;
    }

    void splitInternalNode(BPlusTreeNode<KeyType> *parent, int pos, BPlusTreeNode<KeyType> *&temp) {
        if (parent == nullptr) {
            auto parentNode = static_cast<InternalNode<KeyType> *>(temp);
            InternalNode<KeyType> *rightNode = createRightInternalNode(parentNode);

            InternalNode<KeyType> *newRoot = new InternalNode<KeyType>();
            newRoot->keys.push_back(parentNode->keys[MAX_KEYS / 2]);
            newRoot->children.push_back(temp);
            newRoot->keys.push_back(parentNode->keys[MAX_KEYS]);
            newRoot->children.push_back(rightNode);
            temp = newRoot;
            // 删除之前左子树的后面几个元素
            parentNode->keys.erase(parentNode->keys.begin() + (MAX_KEYS + 1) / 2, parentNode->keys.end());
            parentNode->children.erase(parentNode->children.begin() + (MAX_KEYS + 1) / 2, parentNode->children.end());
        } else {
            auto parentNode = static_cast<InternalNode<KeyType> *>(parent);
            InternalNode<KeyType> *rightNode = createRightInternalNode(static_cast<InternalNode<KeyType> *>(temp));

            // 删除之前左子树的后面几个元素
            temp->keys.erase(temp->keys.begin() + (MAX_KEYS + 1) / 2, temp->keys.end());
            auto tempNode = static_cast<InternalNode<KeyType> *>(temp);
            tempNode->children.erase(tempNode->children.begin() + (MAX_KEYS + 1) / 2, tempNode->children.end());

            // 父节点插入新增元素
            parentNode->keys.erase(parentNode->keys.begin() + pos);
            parentNode->keys.insert(parentNode->keys.begin() + pos, temp->keys.back());
            parentNode->keys.insert(parentNode->keys.begin() + pos + 1, rightNode->keys.back());
            parentNode->children.insert(parentNode->children.begin() + pos + 1, rightNode);
        }
    }


public:
    LeafNode<KeyType> *start;

    BPlusTree() {
        root = nullptr;
    }

    ~BPlusTree() {
        if(root!=nullptr){

        }
    }

    void insert(const KeyType &key, uint64_t value) {
        if (root == nullptr) {
            root = new LeafNode<KeyType>();
            start = new LeafNode<KeyType>();
            root->keys.push_back(key);
            auto tempRoot = static_cast<LeafNode<KeyType> *>(root);
            tempRoot->values.push_back(value);
            tempRoot->next = nullptr;
            tempRoot->pre = start;
            start->next=tempRoot;
        } else {
            // 找到插入的节点位置，第一个大于等于该元素的指针，则继续递归找，直到找到叶子节点
            doInsert(root, key, value, nullptr, 0);
            // 插入该位置
            // 判断是否需要分裂
            // 分裂完，需要分裂节点插入到未分裂之前节点的父节点的后面一个节点
            // 再向上判断父节点是否需要分裂，一直向上
        }
    }


    /**
     * 删除完后，可能需要使用借或者合并操作叶子节点
     *
     * @param root
     * @param parent
     * @param pos
     * @param key
     */
    void borrowOrMergeLeafNode(LeafNode<KeyType> *root, InternalNode<KeyType> *parent, int pos, KeyType key) {
        // 即只有一个根节点（叶子节点），其实不会出现这种情况，对于这里分支节点中来说；
        if (parent == nullptr) { // 叶根节点，直接不处理，
            return;
        }

        // 当节点删除完时,更新父节点信息
        if(root->keys.size()==0){
            parent->keys.erase(parent->keys.begin()+pos);
            parent->children.erase(parent->children.begin()+pos);
            return;
        }
        if (pos - 1 >= 0) {
            auto leftBro = static_cast<LeafNode<KeyType> *>(parent->children[pos - 1]);
            // 1、能借先借
            // 1.1 先借左边
            if (leftBro->keys.size() - 1 >= MIN_KEYS) { // 可以借
                // 借一个左边的节点
                root->keys.insert(root->keys.begin(), leftBro->keys.back());
                root->values.insert(root->values.begin(), leftBro->values.back());

                // 左节点删除借走的元素
                leftBro->keys.erase(leftBro->keys.end() - 1);
                leftBro->values.erase(leftBro->values.end() - 1);

                // 更改父节点中左边的节点元素最大值
                parent->keys[pos - 1] = leftBro->keys.back();

                // 右边节点的最大值可能也会变化，所以还要更新右边节点的最大值
                if(parent->keys[pos]>root->keys.back()){
                    parent->keys[pos]=root->keys.back();
                }
                return;
            }
        }

        if (pos + 1 < parent->keys.size()) {
            auto rightBro = static_cast<LeafNode<KeyType> *>(parent->children[pos + 1]);
            // 1.2 再借右边
            if (rightBro->keys.size() - 1 >= MIN_KEYS) { // 可以借
                // 借一个右边的节点
                root->keys.insert(root->keys.end(), rightBro->keys.front());
                root->values.insert(root->values.end(), rightBro->values.front());

                // 更改父节点中该的节点元素最大值
                parent->keys[pos] = rightBro->keys.front();

                // 删除右节点中借走的元素
                rightBro->keys.erase(rightBro->keys.begin() - 1);
                rightBro->values.erase(rightBro->values.begin() - 1);
                return;
            }
        }

        // 2.借不了，则开始合并，先左后右
        if (pos - 1 >= 0) {
            auto leftBro = static_cast<LeafNode<KeyType> *>(parent->children[pos - 1]);
            // 插入元素到左孩子的节点
            for (int i = 0; i < root->keys.size(); ++i) {
                    leftBro->keys.push_back(root->keys[i]);
                    leftBro->values.push_back(root->values[i]);
            }
            // 更新父节点信息，插入到左边相当于，合并左右两个子树，并且最大值为右边子树的最大值，故只用删除左边key
            // 父节点删除左key，删除右节点的孩子信息和旧节点本身
            parent->children.erase(parent->children.begin()+pos);
            parent->keys.erase(parent->keys.begin() + pos -1);
            // 连接叶子节点
            leftBro->next=root->next;
            if(root->next!= nullptr){
                root->next->pre=leftBro;
            }

            // 但事实上可能右孩子的大key可能被删了，此时需要变
            if(root->keys.back()<*(parent->keys.begin()+pos -1)){
                parent->keys.erase(parent->keys.begin() + pos -1);
                parent->keys.insert(parent->keys.begin()+pos - 1,root->keys.back());
            }
            // 删除旧节点
            delete (root);
            return;
        }

        if (pos + 1 < parent->keys.size()) {
            auto rightBro = static_cast<LeafNode<KeyType> *>(parent->children[pos + 1]);

            // 插入元素到右孩子的节点
            for (int i = 0; i < root->keys.size(); ++i) {
                    rightBro->keys.insert(rightBro->keys.begin(),root->keys[i]);
                    rightBro->values.insert(rightBro->values.begin(),root->values[i]);
            }
            // 更新父节点信息,只用删除不用添加
            // 父节点删除旧子节点和key信息
            parent->keys.erase(parent->keys.begin()+pos);
            parent->children.erase(parent->children.begin() + pos);

            // TODO,插入到右子树时，需要知道左边的节点信息，但是这里用单链表，只能从头遍历才能找到，这样不好
            // 更改为双向链表，不等于空指针时，
            if(rightBro->pre!= nullptr){
                root->pre->next=root->next;
                root->next->pre= root->pre;
            }

            // 删除旧节点的内存
            delete (root);
            return;
        }
    }

    void borrowOrMergeInternalNode(InternalNode<KeyType>*index, InternalNode<KeyType> *parent, int pos){
        if(parent== nullptr){ // 不会出现当前节点是根叶节点进来 ; 这里只会有非叶根节点进来，故这里只需要保证个数大于2，如果小于，则
            if(index->keys.size() < 2){ // 根节点不符合要求时
                BPlusTreeNode<KeyType>* temp = static_cast<InternalNode<KeyType> *>(index->children.front());
                if(temp->isLeaf()){
                    root = static_cast<LeafNode<KeyType>*>(temp);
                }else{
                    root = static_cast<InternalNode<KeyType>*>(temp);
                }
            }
            return;
        }

        if (pos - 1 >= 0) {
            auto leftBro = static_cast<InternalNode<KeyType> *>(parent->children[pos - 1]);
            // 1、能借先借
            // 1.1 先借左边
            if (leftBro->keys.size() - 1 >= MIN_KEYS) { // 可以借
                // 借一个左边的节点
                index->keys.insert(index->keys.begin(), leftBro->keys.back());
                index->children.insert(index->children.begin(), leftBro->children.back());

                // 左节点删除借走的元素
                leftBro->keys.erase(leftBro->keys.end() - 1);
                leftBro->children.erase(leftBro->children.end() - 1);

                // 更改父节点中左边的节点元素最大值
                parent->keys[pos - 1] = leftBro->keys.back();

                // 右边节点的最大值可能也会变化，所以还要更新右边节点的最大值
                if(parent->keys[pos]>index->keys.back()){
                    parent->keys[pos]=index->keys.back();
                }
                return;
            }
        }

        if (pos + 1 < parent->keys.size()) {
            auto rightBro = static_cast<InternalNode<KeyType> *>(parent->children[pos + 1]);
            // 1.2 再借右边
            if (rightBro->keys.size() - 1 >= MIN_KEYS) { // 可以借
                // 借一个右边的节点
                index->keys.insert(index->keys.end(), rightBro->keys.front());
                index->children.insert(index->children.end(), rightBro->children.front());

                // 更改父节点中该的节点元素最大值
                parent->keys[pos] = rightBro->keys.front();

                // 删除右节点中借走的元素
                rightBro->keys.erase(rightBro->keys.begin() - 1);
                rightBro->children.erase(rightBro->children.begin() - 1);
                return;
            }
        }

        // 2.借不了，则开始合并，先左后右
        if (pos - 1 >= 0) {
            auto leftBro = static_cast<InternalNode<KeyType> *>(parent->children[pos - 1]);
            // 插入元素到左孩子的节点
            for (int i = 0; i < index->keys.size(); ++i) {
                leftBro->keys.push_back(index->keys[i]);
                leftBro->children.push_back(index->children[i]);
            }
            // 更新父节点信息，插入到左边相当于，合并左右两个子树，并且最大值为右边子树的最大值，故只用删除左边key
            // 父节点删除左key，删除右节点的孩子信息和旧节点本身

            // 删除旧节点本身信息
            parent->children.erase(parent->children.begin()+pos);
            // 删除左孩子的小key，留下右孩子的大key
            parent->keys.erase(parent->keys.begin() + pos -1);
            // 但事实上可能右孩子的大key可能被删了，此时需要变
            if(index->keys.back()<*(parent->keys.begin()+pos -1)){
                parent->keys.erase(parent->keys.begin() + pos -1);
                parent->keys.insert(parent->keys.begin()+pos - 1,index->keys.back());
            }
            // 删除旧节点
         //   delete (root);
            return;
        }

        if (pos + 1 < parent->keys.size()) {
            auto rightBro = static_cast<InternalNode<KeyType> *>(parent->children[pos + 1]);

            // 插入元素到右孩子的节点
            for (int i = 0; i < index->keys.size(); ++i) {
                    rightBro->keys.insert(rightBro->keys.begin(),index->keys[i]);
                    rightBro->children.insert(rightBro->children.begin(),index->children[i]);
            }
            // 更新父节点信息,只用删除不用添加 todo :有问题，如果是key对应的分支中最大的元素被删除，则需要更改key为当前最大的

            // 父节点删除旧子节点和key信息
            parent->keys.erase(parent->keys.begin()+pos);
            parent->children.erase(parent->children.begin() + pos);
            // 删除旧节点的内存
           // delete (root);
            return;
        }
    }


    void doRemove(std::vector<uint64_t> &result, BPlusTreeNode<KeyType>*root, const KeyType &key, BPlusTreeNode<KeyType> *parent,
                      int pos) {
            BPlusTreeNode<KeyType> *&temp = root;
            int size = temp->keys.size(); // 记录当前需要处理的元素个数
            for (decltype(temp->keys.size()) i = 0; i < size; i++) {
                if (!temp->isLeaf()) { // 分支节点
                    if (key <= temp->keys[i] || i == (size - 1)) { // 第一个小或到最后
                        if (key > temp->keys[i]) { // 大于B+树最大的元素，删除失败
                            return;
                        }
                        auto Node = static_cast<InternalNode<KeyType> *>(temp)->children[i];
                        doRemove(result,Node, key,  temp, i); // 递归查找

                        // 可能子树的最大值已经变了，需要更改父节点中的key
                        if(parent!= nullptr&& parent->keys[pos] > temp->keys.back()){
                            parent->keys[pos] = temp->keys.back();
                        }

                        // 合并完成后叶节点也可能需要合并
                        if (temp->keys.size() < (MAX_KEYS+1)/2) {
                            // TODO 这里parent可能为空，root
                            borrowOrMergeInternalNode(static_cast<InternalNode<int> *>(temp),
                                                      static_cast<InternalNode<int> *>(parent), pos);
                        }
                        break;
                    }
                } else {
                    // 叶子节点
                    // TODO ,删除元素只用调用一次删除，但是实际比较麻烦，只能针对单个节点中的相同的元素进行删除
                    auto leafNode = static_cast<LeafNode<KeyType>*>(temp);

                    // 收集删除的元素
                    auto size = leafNode->keys.size();
                    for (int j = 0; j < size; ++j) {
                        if (leafNode->keys[j] == key) {
                            result.push_back(leafNode->values[j]);
                        }
                        if (leafNode->keys[j] > key) {
                            break;
                        }
                    }

                    // 删除元素
                    auto it2 = leafNode->values.begin();
                    for (auto it = leafNode->keys.begin();
                         it != leafNode->keys.end() && it2 != leafNode->values.end();) {
                        if (*it == key) {
                            it = leafNode->keys.erase(it);
                            it2 = leafNode->values.erase(leafNode->values.begin());
                        } else {
                            it++;
                            it2++;
                        }
                        if(it!= leafNode->keys.end()&&*it>key){
                            break;
                        }
                    }

                    // 是否需要合并或借
                    if(leafNode->keys.size()<(MAX_KEYS+1)/2){
                        borrowOrMergeLeafNode(static_cast<LeafNode<int> *>(temp),
                                              static_cast<InternalNode<int> *>(parent), pos, key);
                    }
                    return;
                }
            }
        }

    void remove(std::vector<uint64_t> &result,KeyType key) {
        doRemove(result,root,key, nullptr,0);
        if(result.empty()){
            std::cout<<"no element,cant delete."<<std::endl;
        } else{
            auto size = result.size();
            do{
                size = result.size();
                doRemove(result,root,key, nullptr,0);
            } while (result.size() != size);
            std::cout<<"deleted values:"<<std::endl;
            for (const auto &item: result){
                std::cout<< item <<" ";
            }
            std::cout<<std::endl;
        }
    }

   void getTargetNode(std::vector<uint64_t>& result,BPlusTreeNode<KeyType>* index,const KeyType &key) const{
        int size = index->keys.size(); // 记录当前需要处理的元素个数
        for (decltype(index->keys.size()) i = 0; i < size; i++) {
            if (!index->isLeaf()) { // 分支节点
                if (key <= index->keys[i]) { // 第一个小或到最后
                    auto Node = static_cast<InternalNode<KeyType> *>(index)->children[i];
                    getTargetNode(result, Node, key); // 递归查找
                    break;
                }
            } else { // 叶子节点
                auto leafNode = static_cast<LeafNode<KeyType>*>(index);
                auto temp(leafNode);
                while (temp){
                    for (int j = 0; j < temp->keys.size(); ++j) {
                        if(temp->keys[j]==key){
                            result.push_back(temp->values[j]);
                        }
                        if(temp->keys[j]>key){
                            return ;
                        }
                    }
                    temp = temp->next;
                }
            }
        }
    }

    /**
     * 二分法查找第一个大于等于该元素的位置,要么返回该元素的位置，要么返回第一个大于该元素的元素
     *
     * @param root
     * @param key
     * @return
     */
    int binarySearch(BPlusTreeNode<KeyType> *root, const KeyType key) {
        int low = 0;
        int high = root->keys.size()-1;

        while (low < high){
            int mid = low + (high - low) /2;
            if(root->keys[mid]>=key){
                high = mid ;
            }else {
                low = mid +1;
            }
        }
        return low;  // 返回大于等于该key的位置
    }

    /**
     * 找到元素该插入或者所待的位置，多个相同元素时位置不固定
     *
     * @param root
     * @param key
     * @return
     */
    LeafNode<KeyType>* binarySearch2(BPlusTreeNode<KeyType> *root, const KeyType key) {
        if(root->isLeaf()){ // 返回元素可能在的节点
            return static_cast<LeafNode<KeyType>*>(root);
        }
        auto temp = static_cast<InternalNode<KeyType>*>(root);
        int low = 0;
        int high = temp->keys.size()-1;

        while (low < high) {
            int mid = low + (high - low) / 2;
            if (temp->keys[mid] >=  key) {
                high = mid;
            } else  {
                low = mid + 1;
            }
        }
        return binarySearch2(temp->children[low],key);
    }

    /**
     * 返回目标key所在节点，二分法
     *
     * @param index
     * @param key
     * @return
     */
    void optimizedSearch(const KeyType &key){
        auto targetNode= binarySearch2(root,key);
        int targetPos = binarySearch(targetNode,key);
        if(targetNode->keys[targetPos] == key){
            std::cout<<"find"<<key<<":"<<targetNode->values[targetPos]<< std::endl;
        } else{
            std::cout<<"cant find "<< key<< std::endl;
        }
    }

    /**
     * 返回目标key所在节点
     *
     * @param index
     * @param key
     * @return
     */
    LeafNode<KeyType>* doSearch(BPlusTreeNode<KeyType>* index,const KeyType &key) const{
        int size = index->keys.size(); // 记录当前需要处理的元素个数
        for (decltype(index->keys.size()) i = 0; i < size; i++) {
            if (!index->isLeaf()) { // 分支节点
                if (key <= index->keys[i]) { // 第一个小或到最后
                    auto Node = static_cast<InternalNode<KeyType> *>(index)->children[i];
                    doSearch( Node, key); // 递归查找
                    break;
                }
            } else { // 叶子节点
                auto leafNode = static_cast<LeafNode<KeyType> *>(index);
                for (int j = 0; j < leafNode->keys.size(); ++j) {
                    if (leafNode->keys[j] == key) {
                        return leafNode;
                    }
                }
                return nullptr;
            }
        }
    }

    std::vector<uint64_t> search(const KeyType &key) const{
        std::vector<uint64_t> result;
        getTargetNode(result,root,key);
        return result; // Return value placeholder
    }

    std::vector<uint64_t> searchRange(const KeyType &start, const KeyType &end) const {
        // Range search implementation
        std::vector<uint64_t> result;
        auto startNode = doSearch(root,start);
        if(startNode == nullptr){
            std::cout<<"cant find the key :"<<start<<std::endl;
            return std::vector<uint64_t>(); // Return result placeholder
        }else{
            while (startNode){
                for (int j = 0; j < startNode->keys.size(); ++j) {
                    if(startNode->keys[j]>= start&&startNode->keys[j]<=end){
                        result.push_back(startNode->keys[j]);//startNode->values[j]);
                    }
                    if(startNode->keys[j]>end){
                        return result;
                    }
                }
                startNode = startNode->next;
            }
        }
    }

    /**
     * 层次遍历B+树，按层输出结果
     */
    void print() {
        std::queue<BPlusTreeNode<KeyType> *> queue;
        queue.push(root);
        while (!queue.empty()) {
            int size = queue.size();
            for (int i = 0; i < size; ++i) {
                auto temp = queue.front();
                queue.pop();
                if (temp->isLeaf()) {
                    std::cout << "[";
                    for (auto key: temp->keys) {
                        std::cout << key << " ";
                    }
                    std::cout << "]";
                } else {
                    auto interNode = static_cast<InternalNode<KeyType> *>(temp);
                    std::cout << "[";
                    for (int i = 0; i < interNode->keys.size(); ++i) {
                        std::cout << interNode->keys[i] << " ";
                        queue.push(interNode->children[i]);
                    }
                    std::cout << "]";
                }
            }
            std::cout << std::endl;
        }
    }

    void setDegree(){
        std::cout<<"please enter a number that is greater than 2"<<std::endl;
        int num;
        std::cin>>num;
        while (num<3){
            std::cout<<"please enter a number that is greater than 2"<<std::endl;
            std::cin>>num;
        }
        this->MAX_KEYS = num;
        this->MIN_KEYS = (num+1)/2;
    }
};

void createTestInt(BPlusTree<int>* intTree);
void createTestInt2(BPlusTree<int>* intTree);
void createTestString(BPlusTree<std::string>* StringTree);
void createTestString2(BPlusTree<std::string>* StringTree);
bool checkNotCreate(BPlusTree<int>* root);


















