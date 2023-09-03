#include <cstddef>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <vector>

// B+树节点基类
template <typename KeyType>
class BPlusTreeNode {
 public:
  virtual bool isLeaf() const = 0;  // 纯虚函数
  virtual ~BPlusTreeNode() {}       // 析构函数
  std::vector<KeyType> keys;
};

// 分支节点
template <typename KeyType>
class InternalNode : public BPlusTreeNode<KeyType> {
 public:
  bool isLeafNode;
  std::vector<BPlusTreeNode<KeyType> *> children;  // vector存指针

  InternalNode() : isLeafNode(false) {}  // 非空构造函数
  bool isLeaf() const override { return isLeafNode; }
};

// 叶子节点
template <typename KeyType>
class LeafNode : public BPlusTreeNode<KeyType> {
 public:
  bool isLeafNode;
  std::vector<uint64_t> values;
  LeafNode<KeyType> *next;
  LeafNode<KeyType> *pre;

  LeafNode() : isLeafNode(true) {}

  bool isLeaf() const override { return isLeafNode; }
};

// B+树
template <typename KeyType>
class BPlusTree {
 private:
  BPlusTreeNode<KeyType> *root;

  int MAX_KEYS;

  int MIN_KEYS;

  /***
   * @description:
   * @return {*}
   */
  void doInsert(BPlusTreeNode<KeyType> *&root, const KeyType &key,
                uint64_t value, BPlusTreeNode<KeyType> *parent, int pos) {
    BPlusTreeNode<KeyType> *&temp = root;
    int size = temp->keys.size();  // 记录当前需要处理的元素个数
    for (decltype(temp->keys.size()) i = 0; i < size; i++) {
      if (!temp->isLeaf()) {  // 分支节点
        if (key <= temp->keys[i] ||
            i == (temp->keys.size() - 1)) {  // 第一个小或到最后
          if (key > temp->keys[i]) {  // 需要更新路径上的最大节点信息
            temp->keys[i] = key;
          }
          auto Node = static_cast<InternalNode<KeyType> *>(temp)->children[i];
          doInsert(Node, key, value, temp, i);  // 递归查找
          if (temp->keys.size() > MAX_KEYS) {  // 当前父节点是否需要分裂
            splitInternalNode(parent, pos, temp);
          }
          break;
        }
      } else {  // 叶子节点

        // 将元素按顺序插入
        LeafNode<KeyType> *leafNode = insertElement(key, value, temp);

        // 大于M个则开始分裂
        if (leafNode->keys.size() > MAX_KEYS) {
          // 创建右子树
          LeafNode<KeyType> *rightNode = createRightLeafNode(leafNode);

          // 重组
          if (parent == nullptr) {  // 非叶根节点至少有两个元素
            // 创建新父节点
            InternalNode<KeyType> *newRoot = new InternalNode<KeyType>();
            newRoot->keys.push_back(leafNode->keys[(MAX_KEYS + 1) / 2]);
            newRoot->children.push_back(leafNode);
            newRoot->keys.push_back(leafNode->keys[MAX_KEYS]);
            newRoot->children.push_back(rightNode);
            root = newRoot;

            // 删除左子树的多余的元素
            leafNode->keys.erase(
                leafNode->keys.begin() + (MAX_KEYS + 1) / 2 + 1,
                leafNode->keys.end());
            leafNode->values.erase(
                leafNode->values.begin() + (MAX_KEYS + 1) / 2 + 1,
                leafNode->values.end());
          } else {
            // 删除左子树多余元素
            leafNode->keys.erase(
                leafNode->keys.begin() + (MAX_KEYS + 1) / 2 + 1,
                leafNode->keys.end());
            leafNode->values.erase(
                leafNode->values.begin() + (MAX_KEYS + 1) / 2 + 1,
                leafNode->values.end());

            // 父节点更新节点元素
            static_cast<InternalNode<KeyType> *>(parent)->children.insert(
                static_cast<InternalNode<KeyType> *>(parent)->children.begin() +
                    pos + 1,
                rightNode);
            parent->keys.erase(parent->keys.begin() + pos);
            parent->keys.insert(parent->keys.begin() + pos,
                                leafNode->keys.back());
            parent->keys.insert(parent->keys.begin() + pos + 1,
                                rightNode->keys.back());
          }

          // 叶子节点连接起来
          if (leafNode->next != nullptr) {
            leafNode->next->pre = rightNode;
          }
          rightNode->next = leafNode->next;
          leafNode->next = rightNode;
          rightNode->pre = leafNode;

          // rightNode->next = leafNode->next;
          // leafNode->next = rightNode;
          // rightNode->pre = leafNode;

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

  InternalNode<KeyType> *createRightInternalNode(
      const InternalNode<KeyType> *parentNode) const {
    decltype(BPlusTreeNode<KeyType>::keys) rightKey(
        parentNode->keys.begin() + (MAX_KEYS + 1) / 2 + 1,
        parentNode->keys.end());
    decltype(InternalNode<KeyType>::children) rightChildren(
        parentNode->children.begin() + (MAX_KEYS + 1) / 2 + 1,
        parentNode->children.end());
    InternalNode<KeyType> *rightNode = new InternalNode<KeyType>();
    rightNode->keys = rightKey;
    rightNode->children = rightChildren;
    return rightNode;
  }

  LeafNode<KeyType> *createRightLeafNode(
      const LeafNode<KeyType> *leafNode) const {
    decltype(BPlusTreeNode<KeyType>::keys) rightKey(
        leafNode->keys.begin() + (MAX_KEYS + 1) / 2 + 1, leafNode->keys.end());
    decltype(LeafNode<KeyType>::values) rightValues(
        leafNode->values.begin() + (MAX_KEYS + 1) / 2 + 1,
        leafNode->values.end());
    LeafNode<KeyType> *rightNode = new LeafNode<KeyType>();
    rightNode->keys = rightKey;
    rightNode->values = rightValues;
    return rightNode;
  }

  /***
   * @description: 只插入一次
   * @return {*}
   */
  LeafNode<KeyType> *insertElement(const KeyType &key, uint64_t value,
                                   BPlusTreeNode<KeyType> *const &temp) const {
    LeafNode<KeyType> *leafNode = static_cast<LeafNode<KeyType> *>(temp);
    for (decltype(leafNode->keys.size()) j = 0; j < leafNode->keys.size();
         j++) {
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

  void splitInternalNode(BPlusTreeNode<KeyType> *parent, int pos,
                         BPlusTreeNode<KeyType> *&temp) {
    if (parent == nullptr) {
      auto parentNode = static_cast<InternalNode<KeyType> *>(temp);
      InternalNode<KeyType> *rightNode = createRightInternalNode(parentNode);

      InternalNode<KeyType> *newRoot = new InternalNode<KeyType>();
      newRoot->keys.push_back(parentNode->keys[(MAX_KEYS + 1) / 2]);
      newRoot->children.push_back(temp);
      newRoot->keys.push_back(parentNode->keys[MAX_KEYS]);  // 这里就是最后的
      newRoot->children.push_back(rightNode);
      temp = newRoot;
      // 删除之前左子树的后面几个元素
      parentNode->keys.erase(parentNode->keys.begin() + (MAX_KEYS + 1) / 2 + 1,
                             parentNode->keys.end());
      parentNode->children.erase(
          parentNode->children.begin() + (MAX_KEYS + 1) / 2 + 1,
          parentNode->children.end());
    } else {
      auto parentNode = static_cast<InternalNode<KeyType> *>(parent);
      InternalNode<KeyType> *rightNode =
          createRightInternalNode(static_cast<InternalNode<KeyType> *>(temp));

      // 删除之前左子树的后面几个元素
      temp->keys.erase(temp->keys.begin() + (MAX_KEYS + 1) / 2 + 1,
                       temp->keys.end());
      auto tempNode = static_cast<InternalNode<KeyType> *>(temp);
      tempNode->children.erase(
          tempNode->children.begin() + (MAX_KEYS + 1) / 2 + 1,
          tempNode->children.end());

      // 父节点插入新增元素
      parentNode->keys.erase(parentNode->keys.begin() + pos);
      parentNode->keys.insert(parentNode->keys.begin() + pos,
                              temp->keys.back());
      parentNode->keys.insert(parentNode->keys.begin() + pos + 1,
                              rightNode->keys.back());
      parentNode->children.insert(parentNode->children.begin() + pos + 1,
                                  rightNode);
    }
  }

 public:
  LeafNode<KeyType> *start;

  BPlusTree() { root = nullptr; }

  ~BPlusTree() {
    if (root != nullptr) {
      // 层次遍历删除内存
      std::queue<BPlusTreeNode<KeyType> *> queue;
      queue.push(root);
      while (!queue.empty()) {
        int size = queue.size();
        for (int i = 0; i < size; ++i) {
          auto temp = queue.front();
          queue.pop();
          if (!temp->isLeaf()) {
            auto interNode = static_cast<InternalNode<KeyType> *>(temp);
            for (int i = 0; i < interNode->keys.size(); ++i) {
              queue.push(interNode->children[i]);
            }
          }
          delete temp;
        }
      }
      delete start;
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
      start->next = tempRoot;
    } else {
      doInsert(root, key, value, nullptr, 0);
    }
  }

  /**
   * 【叶子节点】：元素删除完后，可能需要使用借或者合并操作 叶子节点
   *
   * @param root
   * @param parent
   * @param pos 在父节点中的位置
   * @param key
   */
  void borrowOrMergeLeafNode(BPlusTreeNode<KeyType> *&root,
                             InternalNode<KeyType> *parent, int pos,
                             KeyType key) {
    auto index = static_cast<LeafNode<KeyType> *>(root);
    if (parent == nullptr) {  // 叶根节点，不处理
      return;
    }

    // 当节点删除完时,更新父节点信息,删除该节点
    if (index->keys.size() == 0) {
      parent->keys.erase(parent->keys.begin() + pos);
      parent->children.erase(parent->children.begin() + pos);
      index->pre->next = index->next;
      if (index->next != nullptr) {
        index->next->pre = index->pre;
      }
      delete index;
      root = nullptr;
      index = nullptr;
      return;
    }

    if (pos - 1 >= 0) {
      auto leftBro =
          static_cast<LeafNode<KeyType> *>(parent->children[pos - 1]);
      // 1、能借先借
      // 1.1 先借左边
      int borrowNum = MIN_KEYS - index->keys.size();
      if (leftBro->keys.size() - borrowNum >= MIN_KEYS) {  // 可以借
        // 借左边的节点

        index->keys.insert(index->keys.begin(), leftBro->keys.end() - borrowNum,
                           leftBro->keys.end());
        index->values.insert(index->values.begin(),
                             leftBro->values.end() - borrowNum,
                             leftBro->values.end());

        // 左节点删除借走的元素
        leftBro->keys.erase(leftBro->keys.end() - borrowNum,
                            leftBro->keys.end());
        leftBro->values.erase(leftBro->values.end() - borrowNum,
                              leftBro->values.end());

        // 更改父节点中左边的节点元素最大值
        parent->keys[pos - 1] = leftBro->keys.back();
        return;
      }
    }

    if (pos + 1 < parent->keys.size()) {
      auto rightBro =
          static_cast<LeafNode<KeyType> *>(parent->children[pos + 1]);
      // 1.2 再借右边
      int borrowNum = MIN_KEYS - index->keys.size();
      if (rightBro->keys.size() - borrowNum >= MIN_KEYS) {  // 可以借
        // 借一个右边的节点
        index->keys.insert(index->keys.end(), rightBro->keys.begin(),
                           rightBro->keys.begin() + borrowNum);
        index->values.insert(index->values.end(), rightBro->values.begin(),
                             rightBro->values.begin() + borrowNum);

        // 更改父节点中该的节点元素最大值
        parent->keys[pos] = rightBro->keys.front();

        // 删除右节点中借走的元素
        rightBro->keys.erase(rightBro->keys.begin(),
                             rightBro->keys.begin() + borrowNum);
        rightBro->values.erase(rightBro->values.begin(),
                               rightBro->values.begin() + borrowNum);
        return;
      }
    }

    // 2.借不了，则开始合并，先左后右
    if (pos - 1 >= 0) {
      auto leftBro =
          static_cast<LeafNode<KeyType> *>(parent->children[pos - 1]);
      // 插入元素到左孩子的节点

      leftBro->keys.insert(leftBro->keys.end(), index->keys.begin(),
                           index->keys.end());
      leftBro->values.insert(leftBro->values.end(), index->values.begin(),
                             index->values.end());

      // 更新父节点信息，插入到左边相当于，合并左右两个子树，并且最大值为右边子树的最大值，故只用删除左边key
      // 父节点删除右节点的孩子信息和右节点本身
      parent->children.erase(parent->children.begin() + pos);
      parent->keys.erase(parent->keys.begin() + pos);

      // 连接叶子节点
      leftBro->next = index->next;
      if (index->next != nullptr) {
        index->next->pre = leftBro;
      }

      // 更新左节点最大key
      if (leftBro->keys.back() < *(parent->keys.begin() + pos - 1)) {
        parent->keys.erase(parent->keys.begin() + pos - 1);
        parent->keys.insert(parent->keys.begin() + pos - 1,
                            leftBro->keys.back());
      }
      // 删除旧节点
      index->pre->next = index->next;
      if (index->next != nullptr) {
        index->next->pre = index->pre;
      }
      delete index;
      root = nullptr;
      index = nullptr;

      return;
    }

    if (pos + 1 < parent->keys.size()) {
      auto rightBro =
          static_cast<LeafNode<KeyType> *>(parent->children[pos + 1]);

      // 插入元素到右孩子的节点
      rightBro->keys.insert(rightBro->keys.begin(), index->keys.begin(),
                            index->keys.end());
      rightBro->values.insert(rightBro->values.begin(), index->values.begin(),
                              index->values.end());

      // 更新父节点信息,只用删除不用添加
      // 父节点删除旧子节点和key信息
      parent->keys.erase(parent->keys.begin() + pos);
      parent->children.erase(parent->children.begin() + pos);

      // TODO,插入到右子树时，需要知道左边的节点信息，但是这里用单链表，只能从头遍历才能找到，这样不好
      // 更改为双向链表，不等于空指针时，
      if (rightBro->pre != nullptr) {
        index->pre->next = index->next;
        index->next->pre = index->pre;
      }
      delete index;
      root = nullptr;
      index = nullptr;
      return;
    }
  }

  void borrowOrMergeInternalNode(BPlusTreeNode<KeyType> *&internalNode,
                                 InternalNode<KeyType> *parent, int pos) {
    auto index = static_cast<InternalNode<KeyType> *>(internalNode);
    if (parent ==
        nullptr) {  // 不会出现当前节点是根叶节点进来 ;
                    // 这里只会有非叶根节点进来，故这里只需要保证个数大于2，如果小于，则
      if (index->keys.size() < 2) {  // 根节点不符合要求时
        BPlusTreeNode<KeyType> *temp =
            static_cast<InternalNode<KeyType> *>(index->children.front());
        if (temp->isLeaf()) {
          root = static_cast<LeafNode<KeyType> *>(temp);
        } else {
          root = static_cast<InternalNode<KeyType> *>(temp);
        }
        delete index;
        internalNode = nullptr;
        index = nullptr;
      }
      return;
    }

    if (pos - 1 >= 0) {
      auto leftBro =
          static_cast<InternalNode<KeyType> *>(parent->children[pos - 1]);
      // 1、能借先借
      // 1.1 先借左边
      int borrowNum = MIN_KEYS - index->keys.size();
      if (leftBro->keys.size() - borrowNum >= MIN_KEYS) {  // 可以借
        // 借一个左边的节点
        index->keys.insert(index->keys.begin(), leftBro->keys.end() - borrowNum,
                           leftBro->keys.end());
        index->children.insert(index->children.begin(),
                               leftBro->children.end() - borrowNum,
                               leftBro->children.end());

        // 左节点删除借走的元素
        leftBro->keys.erase(leftBro->keys.end() - borrowNum,
                            leftBro->keys.end());
        leftBro->children.erase(leftBro->children.end() - borrowNum,
                                leftBro->children.end());

        // 更改父节点中左边的节点元素最大值
        parent->keys[pos - 1] = leftBro->keys.back();
        return;
      }
    }

    if (pos + 1 < parent->keys.size()) {
      auto rightBro =
          static_cast<InternalNode<KeyType> *>(parent->children[pos + 1]);
      // 1.2 再借右边
      int borrowNum = MIN_KEYS - index->keys.size();
      if (rightBro->keys.size() - borrowNum >= MIN_KEYS) {  // 可以借
        // 借一个右边的节点
        index->keys.insert(index->keys.end(), rightBro->keys.begin(),
                           rightBro->keys.begin() + borrowNum);
        index->children.insert(index->children.end(),
                               rightBro->children.begin(),
                               rightBro->children.begin() + borrowNum);

        // 更改父节点中该的节点元素最大值
        parent->keys[pos] = rightBro->keys.front();

        // 删除右节点中借走的元素
        rightBro->keys.erase(rightBro->keys.begin(),
                             rightBro->keys.begin() + borrowNum);
        rightBro->children.erase(rightBro->children.begin(),
                                 rightBro->children.begin() + borrowNum);
        return;
      }
    }

    // 2.借不了，则开始合并，先左后右
    if (pos - 1 >= 0) {
      auto leftBro =
          static_cast<InternalNode<KeyType> *>(parent->children[pos - 1]);
      // 插入元素到左孩子的节点

      leftBro->keys.insert(leftBro->keys.end(), index->keys.begin(),
                           index->keys.end());
      leftBro->children.insert(leftBro->children.end(), index->children.begin(),
                               index->children.end());

      // 更新父节点信息，插入到左边相当于，合并左右两个子树，并且最大值为右边子树的最大值，故只用删除左边key
      // 父节点删除左key，删除右节点的孩子信息和旧节点本身

      // 删除旧节点本身信息
      parent->children.erase(parent->children.begin() + pos);
      // 删除左孩子的小key，留下右孩子的大key
      parent->keys.erase(parent->keys.begin() + pos - 1);
      // 但事实上可能右孩子的大key可能被删了，此时需要变
      if (index->keys.back() < *(parent->keys.begin() + pos - 1)) {
        parent->keys.erase(parent->keys.begin() + pos - 1);
        parent->keys.insert(parent->keys.begin() + pos - 1, index->keys.back());
      }
      // 删除旧节点
      delete index;
      internalNode = nullptr;
      index = nullptr;
      return;
    }

    if (pos + 1 < parent->keys.size()) {
      auto rightBro =
          static_cast<InternalNode<KeyType> *>(parent->children[pos + 1]);

      // 插入元素到右孩子的节点
      rightBro->keys.insert(rightBro->keys.begin(), index->keys.begin(),
                            index->keys.end());
      rightBro->children.insert(rightBro->children.begin(),
                                index->children.begin(), index->children.end());
      // 更新父节点信息,只用删除不用添加 todo
      // :有问题，如果是key对应的分支中最大的元素被删除，则需要更改key为当前最大的

      // 父节点删除旧子节点和key信息
      parent->keys.erase(parent->keys.begin() + pos);
      parent->children.erase(parent->children.begin() + pos);
      // 删除旧节点的内存
      delete index;
      internalNode = nullptr;
      index = nullptr;
      return;
    }
  }

  void doRemove(std::vector<uint64_t> &result, BPlusTreeNode<KeyType> *root,
                const KeyType &key, BPlusTreeNode<KeyType> *parent, int pos) {
    BPlusTreeNode<KeyType> *&temp = root;
    int size = temp->keys.size();  // 记录当前需要处理的元素个数
    for (decltype(temp->keys.size()) i = 0; i < size; i++) {
      if (!temp->isLeaf()) {                            // 分支节点
        if (key <= temp->keys[i] || i == (size - 1)) {  // 第一个小或到最后
          if (key > temp->keys[i]) {  // 大于B+树最大的元素，删除失败
            return;
          }
          auto Node = static_cast<InternalNode<KeyType> *>(temp)->children[i];
          doRemove(result, Node, key, temp, i);  // 递归查找

          // 可能子树的最大值已经变了，需要更改父节点中的key
          if (parent != nullptr && temp != nullptr &&
              parent->keys[pos] > temp->keys.back()) {
            parent->keys[pos] = temp->keys.back();
          }

          // 可能子树的最小值也已经变了，需要更改父节点中的key
          if (parent != nullptr && temp != nullptr &&
              parent->keys[pos] < temp->keys.front()) {
            parent->keys[pos] = temp->keys.front();
          }

          if (temp->keys.size() < (MAX_KEYS + 1) / 2) {
            // TODO 这里parent可能为空，root
            borrowOrMergeInternalNode(
                temp, static_cast<InternalNode<int> *>(parent), pos);
          }
          break;
        }
      } else {
        // 叶子节点
        // TODO
        // ,删除元素只用调用一次删除，但是实际比较麻烦，只能针对单个节点中的相同的元素进行删除
        auto leafNode = static_cast<LeafNode<KeyType> *>(temp);

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
          if (it != leafNode->keys.end() && *it > key) {
            break;
          }
        }

        // 是否需要合并或借
        if (leafNode->keys.size() < (MAX_KEYS + 1) / 2) {
          borrowOrMergeLeafNode(temp, static_cast<InternalNode<int> *>(parent),
                                pos, key);
        }

        // 删除完成，是否需要更改父节点中最大值和最小值
        // 可能子树的最大值已经变了，需要更改父节点中的key
        if (parent != nullptr && temp != nullptr &&
            parent->keys[pos] > temp->keys.back()) {
          parent->keys[pos] = temp->keys.back();
        }

        // 可能子树的最小值也已经变了，需要更改父节点中的key
        if (parent != nullptr && temp != nullptr &&
            parent->keys[pos] < temp->keys.front()) {
          parent->keys[pos] = temp->keys.front();
        }

        return;
      }
    }
  }

  void remove(std::vector<uint64_t> &result, KeyType key) {
    doRemove(result, root, key, nullptr, 0);
    if (result.empty()) {
      std::cout << "no element,cant delete." << std::endl;
    } else {
      auto size = result.size();
      do {
        size = result.size();
        doRemove(result, root, key, nullptr, 0);
      } while (result.size() != size);
      std::cout << "deleted values: ";
      for (const auto &item : result) {
        std::cout << item << " ";
      }
      std::cout << std::endl;
    }
  }

  void getTargetNode(std::vector<uint64_t> &result,
                     BPlusTreeNode<KeyType> *index, const KeyType &key) const {
    int size = index->keys.size();  // 记录当前需要处理的元素个数
    for (decltype(index->keys.size()) i = 0; i < size; i++) {
      if (!index->isLeaf()) {         // 分支节点
        if (key <= index->keys[i]) {  // 第一个小或到最后
          auto Node = static_cast<InternalNode<KeyType> *>(index)->children[i];
          getTargetNode(result, Node, key);  // 递归查找
          break;
        }
      } else {  // 叶子节点
        auto leafNode = static_cast<LeafNode<KeyType> *>(index);
        auto temp(leafNode);
        while (temp) {
          for (int j = 0; j < temp->keys.size(); ++j) {
            if (temp->keys[j] == key) {
              result.push_back(temp->values[j]);
            }
            if (temp->keys[j] > key) {
              return;
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
    int high = root->keys.size() - 1;

    while (low < high) {
      int mid = low + (high - low) / 2;
      if (root->keys[mid] >= key) {
        high = mid;
      } else {
        low = mid + 1;
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
  LeafNode<KeyType> *binarySearch2(BPlusTreeNode<KeyType> *root,
                                   const KeyType key) {
    if (root->isLeaf()) {  // 返回元素可能在的节点
      return static_cast<LeafNode<KeyType> *>(root);
    }
    auto temp = static_cast<InternalNode<KeyType> *>(root);
    int low = 0;
    int high = temp->keys.size() - 1;

    while (low < high) {
      int mid = low + (high - low) / 2;
      if (temp->keys[mid] >= key) {
        high = mid;
      } else {
        low = mid + 1;
      }
    }
    return binarySearch2(temp->children[low], key);
  }

  /**
   * 返回目标key所在节点，二分法
   *
   * @param index
   * @param key
   * @return
   */
  void optimizedSearch(const KeyType &key) {
    auto targetNode = binarySearch2(root, key);
    int targetPos = binarySearch(targetNode, key);
    if (targetNode->keys[targetPos] == key) {
      std::cout << "find " << key << " :";
      while (targetNode) {
        for (int j = 0; j < targetNode->keys.size(); ++j) {
          if (targetNode->keys[j] == key) {
            std::cout << targetNode->values[j] << " ";
          }
          if (targetNode->keys[j] > key) {
            std::cout << std::endl;
            return;
          }
        }
        targetNode = targetNode->next;
      }
    } else {
      std::cout << "cant find " << key << std::endl;
    }
  }

  /**
   * 返回目标key所在节点
   *
   * @param index
   * @param key
   * @return
   */
  LeafNode<KeyType> *doSearch(BPlusTreeNode<KeyType> *index,
                              const KeyType &key) const {
    int size = index->keys.size();  // 记录当前需要处理的元素个数
    for (decltype(index->keys.size()) i = 0; i < size; i++) {
      if (!index->isLeaf()) {         // 分支节点
        if (key <= index->keys[i]) {  // 第一个小或到最后
          auto Node = static_cast<InternalNode<KeyType> *>(index)->children[i];
          doSearch(Node, key);  // 递归查找
          break;
        }
      } else {  // 叶子节点
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

  std::vector<uint64_t> search(const KeyType &key) const {
    std::vector<uint64_t> result;
    getTargetNode(result, root, key);
    return result;  // Return value placeholder
  }

  void searchRange(const KeyType &start, const KeyType &end) const {
    // Range search implementation
    std::vector<uint64_t> result;
    auto startNode = doSearch(root, start);
    if (startNode == nullptr) {
      std::cout << "cant find the key :" << start << std::endl;
      return;  // Return result placeholder
    } else {
      while (startNode != nullptr) {
        for (int j = 0; j < startNode->keys.size(); ++j) {
          if (startNode->keys[j] >= start && startNode->keys[j] <= end) {
            result.push_back(startNode->keys[j]);  // startNode->values[j]);
          }
          if (startNode->keys[j] > end) {
            return;
          }
        }
        startNode = startNode->next;
      }
      for (auto item : result) {
        std::cout << item << " ";
      }
    }
  }

  /**
   * 层次遍历B+树，按层输出结果
   */
  void print() {
    std::cout << "now BplusTree :" << std::endl;
    std::queue<BPlusTreeNode<KeyType> *> queue;
    queue.push(root);
    while (!queue.empty()) {
      int size = queue.size();
      for (int i = 0; i < size; ++i) {
        auto temp = queue.front();
        queue.pop();
        if (temp->isLeaf()) {
          std::cout << "[";
          for (auto key : temp->keys) {
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

  void setDegree() {
    std::cout << "please enter a number that is greater than 2 : ";
    int num;
    std::cin >> num;
    std::cout << std::endl;
    while (num < 3) {
      std::cout << "please enter a number that is greater than 2: ";
      std::cin >> num;
      std::cout << std::endl;
    }
    this->MAX_KEYS = num;
    this->MIN_KEYS = (num + 1) / 2;
  }
};

void createTestInt1(BPlusTree<int> *intTree);
void createTestInt2(BPlusTree<int> *intTree);
void createTestString1(BPlusTree<std::string> *StringTree);
void createTestString2(BPlusTree<std::string> *StringTree);
bool checkNotCreate(BPlusTree<int> *root);
