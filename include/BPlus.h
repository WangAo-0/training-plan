#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <queue>
#include <stack>
#include <string>
#include <vector>

#include "BPlusTree.pb.h"

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
   * @description: 插入操作
   *
   * @param {BPlusTreeNode<KeyType>} *&root 当前节点
   * @param {KeyType} &key 待插入元素key
   * @param {uint64_t} value 待插入元素value
   * @param {BPlusTreeNode<KeyType>} *parent 当前节点的父节点
   * @param {int} pos 当前节点在父节点中的位置
   * @return void
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
        int deteleNum = 0;
        if ((MAX_KEYS + 1) % 2 == 0) {
          deteleNum = (MAX_KEYS + 1) / 2;
        } else {
          deteleNum = (MAX_KEYS + 1) / 2 + 1;
        }

        // 大于M个则开始分裂
        if (leafNode->keys.size() > MAX_KEYS) {
          // 创建右子树
          LeafNode<KeyType> *rightNode = createRightLeafNode(leafNode);

          // 重组
          if (parent == nullptr) {  // 非叶根节点至少有两个元素
            // 创建新父节点
            InternalNode<KeyType> *newRoot = new InternalNode<KeyType>();
            newRoot->keys.push_back(leafNode->keys[deteleNum - 1]);
            newRoot->children.push_back(leafNode);
            newRoot->keys.push_back(leafNode->keys.back());
            newRoot->children.push_back(rightNode);
            root = newRoot;

            // 删除左子树的多余的元素
            leafNode->keys.erase(leafNode->keys.begin() + deteleNum,
                                 leafNode->keys.end());
            leafNode->values.erase(leafNode->values.begin() + deteleNum,
                                   leafNode->values.end());
          } else {
            // 删除左子树多余元素
            leafNode->keys.erase(leafNode->keys.begin() + deteleNum,
                                 leafNode->keys.end());
            leafNode->values.erase(leafNode->values.begin() + deteleNum,
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

  /***
   * @description: 创建右分支节点
   *
   * @param {InternalNode<KeyType>} *parentNode 父节点
   * @return InternalNode<KeyType>* 返回右分支节点
   */
  InternalNode<KeyType> *createRightInternalNode(
      const InternalNode<KeyType> *parentNode) const {
    int deteleNum = 0;
    if ((MAX_KEYS + 1) % 2 == 0) {
      deteleNum = (MAX_KEYS + 1) / 2;
    } else {
      deteleNum = (MAX_KEYS + 1) / 2 + 1;
    }
    decltype(BPlusTreeNode<KeyType>::keys) rightKey(
        parentNode->keys.begin() + deteleNum, parentNode->keys.end());
    decltype(InternalNode<KeyType>::children) rightChildren(
        parentNode->children.begin() + deteleNum, parentNode->children.end());
    InternalNode<KeyType> *rightNode = new InternalNode<KeyType>();
    rightNode->keys = rightKey;
    rightNode->children = rightChildren;
    return rightNode;
  }

  /***
   * @description: 创建右叶子节点
   *
   * @param {LeafNode<KeyType>} *leafNode 叶子节点
   * @return LeafNode<KeyType> * 返回右叶子节点
   */
  LeafNode<KeyType> *createRightLeafNode(
      const LeafNode<KeyType> *leafNode) const {
    int deteleNum = 0;
    if ((MAX_KEYS + 1) % 2 == 0) {
      deteleNum = (MAX_KEYS + 1) / 2;
    } else {
      deteleNum = (MAX_KEYS + 1) / 2 + 1;
    }
    decltype(BPlusTreeNode<KeyType>::keys) rightKey(
        leafNode->keys.begin() + deteleNum, leafNode->keys.end());
    decltype(LeafNode<KeyType>::values) rightValues(
        leafNode->values.begin() + deteleNum, leafNode->values.end());
    LeafNode<KeyType> *rightNode = new LeafNode<KeyType>();
    rightNode->keys = rightKey;
    rightNode->values = rightValues;
    return rightNode;
  }

  /***
   * @description: 插入元素到叶节点中
   *
   * @param {KeyType} &key 待插入key
   * @param {uint64_t} value 待插入value
   * @param {BPlusTreeNode<KeyType>} * 当前叶节点
   * @return LeafNode<KeyType> * 返回元素插入后的叶节点
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

  /***
   * @description: 插入后超过元素个数超过最大元素个数，分支节点的分裂
   *
   * @param {BPlusTreeNode<KeyType>} *parent 父节点
   * @param {int} pos 当前节点在父节点中的位置
   * @param {BPlusTreeNode<KeyType>} * 当前的节点
   * @return void
   */
  void splitInternalNode(BPlusTreeNode<KeyType> *parent, int pos,
                         BPlusTreeNode<KeyType> *&temp) {
    int deteleNum = 0;
    if ((MAX_KEYS + 1) % 2 == 0) {
      deteleNum = (MAX_KEYS + 1) / 2;
    } else {
      deteleNum = (MAX_KEYS + 1) / 2 + 1;
    }
    if (parent == nullptr) {
      auto parentNode = static_cast<InternalNode<KeyType> *>(temp);
      InternalNode<KeyType> *rightNode = createRightInternalNode(parentNode);

      InternalNode<KeyType> *newRoot = new InternalNode<KeyType>();
      newRoot->keys.push_back(parentNode->keys[deteleNum - 1]);
      newRoot->children.push_back(temp);
      newRoot->keys.push_back(parentNode->keys.back());  // 这里就是最后的
      newRoot->children.push_back(rightNode);
      temp = newRoot;

      // 删除之前左子树的后面几个元素
      parentNode->keys.erase(parentNode->keys.begin() + deteleNum,
                             parentNode->keys.end());
      parentNode->children.erase(parentNode->children.begin() + deteleNum,
                                 parentNode->children.end());
    } else {
      auto parentNode = static_cast<InternalNode<KeyType> *>(parent);
      InternalNode<KeyType> *rightNode =
          createRightInternalNode(static_cast<InternalNode<KeyType> *>(temp));

      // 删除之前左子树的后面几个元素
      temp->keys.erase(temp->keys.begin() + deteleNum, temp->keys.end());
      auto tempNode = static_cast<InternalNode<KeyType> *>(temp);
      tempNode->children.erase(tempNode->children.begin() + deteleNum,
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

  /***
   * @description: 叶子节点借或合并
   *
   * @param root 当前节点
   * @param *parent 父节点
   * @param pos 当前节点在父节点的位置
   * @param key 待插入元素的key
   * @return void
   */
  void borrowOrMergeLeafNode(BPlusTreeNode<KeyType> *&root,
                             InternalNode<KeyType> *parent, int pos,
                             KeyType key) {
    auto index = static_cast<LeafNode<KeyType> *>(root);
    if (parent == nullptr) {  // 叶根节点，不处理
      if (index->keys.size() == 0) {
        index->pre->next = nullptr;
        index->pre = nullptr;

        delete index;
        root = nullptr;
        index = nullptr;
        return;
      }
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

  /***
   * @description: 分支节点借或合并
   *
   * @param root 当前节点
   * @param *parent 父节点
   * @param pos 当前节点在父节点的位置
   * @return void
   */
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

  /***
   * @description: 删除操作
   *
   * @param &result 删除的元素
   * @param *root 当前节点
   * @param &key 待删除元素的key
   * @param *parent 父节点
   * @param pos 当前节点在父节点中的位置
   * @return void
   */
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

  /***
   * @description: 单key查找，O(n)版
   *
   * @param &result 查找到的结果values
   * @param *index 当前节点
   * @param &key 待查找的key
   * @return void
   */
  void doSearch(std::vector<uint64_t> &result, BPlusTreeNode<KeyType> *index,
                const KeyType &key) const {
    int size = index->keys.size();  // 记录当前需要处理的元素个数
    for (decltype(index->keys.size()) i = 0; i < size; i++) {
      if (!index->isLeaf()) {         // 分支节点
        if (key <= index->keys[i]) {  // 第一个小或到最后
          auto Node = static_cast<InternalNode<KeyType> *>(index)->children[i];
          doSearch(result, Node, key);  // 递归查找
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

  /***
   * @description: 查找key所在第一个节点或待插入的位置，O(n)版
   *
   * @param *index 当前节点
   * @param &key 待查找元素的key
   * @return LeafNode<KeyType> * 元素所在的第一个节点
   */
  LeafNode<KeyType> *getTargetNode(BPlusTreeNode<KeyType> *index,
                                   const KeyType &key) const {
    int size = index->keys.size();  // 记录当前需要处理的元素个数
    for (decltype(index->keys.size()) i = 0; i < size; i++) {
      if (!index->isLeaf()) {         // 分支节点
        if (key <= index->keys[i]) {  // 第一个小或到最后
          auto Node = static_cast<InternalNode<KeyType> *>(index)->children[i];
          getTargetNode(Node, key);  // 递归查找
          break;
        }
      } else {  // 叶子节点
        auto leafNode = static_cast<LeafNode<KeyType> *>(index);
        for (int j = 0; j < leafNode->keys.size(); ++j) {
          if (leafNode->keys[j] == key) {
            return leafNode;
          }
        }
        return leafNode;  //这里不是目标节点，只是找到了插入的位置
      }
    }
  }

  /***
   * @description:  查找叶子节点中>=该key的首个位置，O(logn)
   *
   * @param *root 叶节点
   * @param key 待查找的key
   * @return 返回位置
   */
  int binarySearchLeafNode(BPlusTreeNode<KeyType> *root, const KeyType key) {
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

  /***
   * @description: 查找元素所处的首个叶节点，O(logn)
   *
   * @param *root 当节点
   * @param key 待查找的key
   * @return 返回叶节点
   */
  LeafNode<KeyType> *binarySearchInternalNode(BPlusTreeNode<KeyType> *root,
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
    return binarySearchInternalNode(temp->children[low], key);
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
          } else {
            auto leafNode = static_cast<LeafNode<KeyType> *>(temp);
            leafNode->pre->next = nullptr;
            leafNode->next = nullptr;
          }
          delete temp;
          temp = nullptr;
        }
      }
      delete start;
      start = nullptr;
    }
  }

  /***
   * @description: 插入
   *
   * @param &key 待插入key
   * @param value 待插入value
   * @return void
   */
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

  /***
   * @description:删除
   *
   * @param &result 删除元素的集合
   * @param key 待删除的key
   * @return void
   */
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
    }
  }

  /***
   * @description: 单key查找，O(logn)
   *
   * @param &key
   * @return
   */
  void optimizedSearch(const KeyType &key, std::vector<uint64_t> &res) {
    auto targetNode = binarySearchInternalNode(root, key);
    while (targetNode) {
      for (int j = 0 /*targetPos*/; j < targetNode->keys.size(); ++j) {
        if (targetNode->keys[j] == key) {
          res.push_back(targetNode->values[j]);
        }
        if (targetNode->keys[j] > key) {
          return;
        }
      }
      targetNode = targetNode->next;
    }
  }

  /***
   * @description: 查找操作
   *
   * @param &key
   * @return void
   */
  void search(const KeyType &key, std::vector<uint64_t> &res) const {
    doSearch(res, root, key);
  }

  /***
   * @description: 范围查询
   *
   * @param &
   * @param &start
   * @param &end
   * @return
   */
  void searchRange(std::map<KeyType, std::vector<uint64_t>> &res,
                   const KeyType &start, const KeyType &end) const {
    auto startNode = getTargetNode(root, start);
    while (startNode != nullptr) {
      for (int j = 0; j < startNode->keys.size(); ++j) {
        if (startNode->keys[j] >= start && startNode->keys[j] <= end) {
          std::vector<uint64_t> &vec = res[startNode->keys[j]];
          vec.push_back(startNode->values[j]);
        }
        if (startNode->keys[j] > end) {
          return;
        }
      }
      startNode = startNode->next;
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

  void setDegree(int num) {
    this->MAX_KEYS = num;
    this->MIN_KEYS = (num + 1) / 2;
  }

  void serialiaztionTree() {
    std::queue<BPlusTreeNode<KeyType> *> queue;
    std::queue<BPlusTreeNodeS *> queueS;
    queue.push(root);
    BPlusTreeS bPlusTreeS;
    bPlusTreeS.set_max_keys(MAX_KEYS);
    bPlusTreeS.set_min_keys(MIN_KEYS);
    BPlusTreeNodeS *rootS = new BPlusTreeNodeS();
    LeafNodeS *startS = new LeafNodeS();
    startS->set_allocated_next(nullptr);

    bPlusTreeS.set_allocated_root(rootS);
    bPlusTreeS.set_allocated_start(startS);

    queueS.push(rootS);

    LeafNodeS *tempStart = startS;

    while (!queue.empty()) {
      int size = queue.size();
      for (int i = 0; i < size; ++i) {
        auto temp = queue.front();
        queue.pop();

        auto tempS = queueS.front();
        queueS.pop();
        if (temp->isLeaf()) {
          auto leafNode = static_cast<LeafNode<KeyType> *>(temp);
          for (int i = 0; i < temp->keys.size(); i++) {
            tempS->mutable_leaf_node()->add_keys(temp->keys[i]);
            tempS->mutable_leaf_node()->add_values(leafNode->values[i]);
            std::cout << "keys[" << i
                      << "] :" << tempS->mutable_leaf_node()->keys(i);
          }
          tempStart->set_allocated_next(tempS->mutable_leaf_node());
          tempStart = tempS->mutable_leaf_node();
          tempStart->set_allocated_next(nullptr);
          std::cout << std::endl;
        } else {
          auto interNode = static_cast<InternalNode<KeyType> *>(temp);
          for (int i = 0; i < interNode->keys.size(); ++i) {
            queue.push(interNode->children[i]);

            tempS->mutable_internal_node()->add_keys(interNode->keys[i]);
            auto children = tempS->mutable_internal_node()->add_children();

            // 增加孩子节点，会自动增加一个到children数组中
            queueS.push(children);
            if (tempS->has_internal_node()) {
              std::cout << "keys[" << i
                        << "] :" << tempS->mutable_internal_node()->keys(i);
            }
          }
        }
        std::cout << std::endl;
      }
    }

    std::string output;
    bPlusTreeS.SerializeToString(&output);
    std::ofstream outputFile("/home/oliver/Developer/bPlusProject/src/outfile");

    if (!outputFile.is_open()) {
      std::cerr << "无法打开文件" << std::endl;
      return;
    }
    outputFile << output << std::endl;
    outputFile.close();
    if (outputFile.fail()) {
      std::cerr << "无法关闭文件" << std::endl;
      return;
    }
  }

  void deserializationTree() {
    BPlusTreeS bp;
    // 打开文件
    std::ifstream input_file("/home/oliver/Developer/bPlusProject/src/outfile",
                             std::ios::binary);

    // 检查文件是否成功打开
    if (!input_file.is_open()) {
      std::cerr << "Failed to open the file." << std::endl;
      return;
    }

    // 读取文件内容到字符串
    std::string file_contents;
    std::string line;
    bool flag = false;
    while (std::getline(input_file, line)) {
      if (flag) {
        file_contents += '\n';  // 可选，如果你想保留文件中的换行符
      }
      file_contents += line;
      flag = true;
    }

    // 关闭文件
    input_file.close();
    if (!bp.ParseFromString(file_contents)) {
      std::cout << "反序列化失败" << std::endl;
    };
    std::cout << bp.max_keys() << std::endl;
    std::cout << "反序列化结果： " << bp.root().internal_node().keys(0);

    rebuildTree(bp);
  }

  void rebuildTree(BPlusTreeS bPlusTreeS) {
    // if (root != nullptr) {
    //   std::cout << "当前树非空" << std::endl;
    //   return;
    // }

    std::queue<BPlusTreeNode<KeyType> *> queue;
    if (bPlusTreeS.root().has_internal_node()) {
      root = new InternalNode<KeyType>();
    } else {
      root = new LeafNode<KeyType>();
    }
    start = new LeafNode<KeyType>();
    start->next = nullptr;
    queue.push(root);
    setDegree(bPlusTreeS.max_keys());

    std::queue<BPlusTreeNodeS *> queueS;
    queueS.push(bPlusTreeS.mutable_root());

    LeafNode<KeyType> *tempStart = start;

    while (!queueS.empty()) {
      int size = queueS.size();
      for (int i = 0; i < size; ++i) {
        auto tempS = queueS.front();
        queueS.pop();

        auto temp = queue.front();
        queue.pop();
        if (tempS->has_leaf_node()) {
          auto leafNodeS = tempS->mutable_leaf_node();
          auto leafNode = static_cast<LeafNode<KeyType> *>(temp);
          for (int i = 0; i < leafNodeS->keys_size(); i++) {
            leafNode->keys.push_back(leafNodeS->keys(i));
            leafNode->values.push_back(leafNodeS->values(i));
          }
          tempStart->next = leafNode;
          leafNode->pre = tempStart;
          tempStart = leafNode;
          tempStart->next = nullptr;
          std::cout << std::endl;
        } else {
          auto interNodeS = tempS->mutable_internal_node();
          auto interNode = static_cast<InternalNode<KeyType> *>(temp);
          for (int i = 0; i < interNodeS->keys_size(); ++i) {
            queueS.push(interNodeS->mutable_children(i));

            interNode->keys.push_back(interNodeS->keys(i));

            if (interNodeS->mutable_children(i)->has_internal_node()) {
              auto children = new InternalNode<KeyType>();
              interNode->children.push_back(children);
              queue.push(children);
            } else {
              auto children = new LeafNode<KeyType>();
              interNode->children.push_back(children);
              queue.push(children);
            }
          }
        }
        std::cout << std::endl;
      }
    }
  }
};