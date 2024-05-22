#ifndef TICKETSYSTEM_BPTREE_HPP
#define TICKETSYSTEM_BPTREE_HPP
#include"HashMap.hpp"
#include"FileSystem.hpp"
#include"vector.hpp"
#include"utility.hpp"

#define ll long long
template<class Key, class Value, class Function,int max_size, int info_len,int CacheSize>
class BPT{
    typedef Yuki::pair<Key, Value> T;
private:
    // 在外存中不适用指针，用pos代替
    //T block[2000];// 用于存储+写入，储存一个数据页
    int root_num = 0;// 记录root的pos
    class node{
        friend class BPT<Key, Value, Function, max_size, info_len, CacheSize>;
    private:
        bool isLeaf;
        T key[max_size];// 如果是leaf则为max_size,否则是max_size - 1,并且索引块的值之前是指向的位置
        int sonPos[max_size] = {0};// 记录索引页的位置，但如果是leaf则不需要
        int index_num;// 储存这是创建的第几个块
        int len; // 记录当前填充长度，用于裂块和并块，是key的长度
        int next_index = -1; // 有next就赋值，只给叶子结点next_index
    public:
        node():isLeaf(true), len(0), index_num(0){}
        explicit node(bool is_leaf, int num):isLeaf(is_leaf), index_num(num), len(0){}
        node(const node& other) {
            len = other.len;
            isLeaf = other.isLeaf;
            index_num = other.index_num;
            next_index = other.next_index;
            for (int i = 0; i < max_size; i++) {
                key[i] = other.key[i];
                sonPos[i] = other.sonPos[i];
            }
        }

        node& operator=(const node& other) {
            if (this == &other) return *this;
            len = other.len;
            isLeaf = other.isLeaf;
            index_num = other.index_num;
            next_index = other.next_index;
            if (!isLeaf) {
                for (int i = 0; i < len + 1; i++) {
                    key[i] = other.key[i];
                    sonPos[i] = other.sonPos[i];
                }
            } else {
                for (int i = 0; i < len; i++) {
                    key[i] = other.key[i];
                }
            }
            return *this;
        }

        bool operator==(const node& other) const {
            return index_num == other.index_num;
        }

    };
    struct spiltNode{
        node objNode;
        bool need_spilt = false;
        //bool exist = false;
        spiltNode& operator=(const spiltNode &other) {
            if (this == &other) return *this;
            objNode = other.objNode;
            need_spilt = other.need_spilt;
            return *this;
        }

        bool operator==(const spiltNode& other) const {
            return objNode == other.objNode;
        }
    };
    class EraseType{
        friend class BPT<Key, Value, Function, max_size, info_len, CacheSize>;
    private:
        T updateData;
        bool need_merge = false;
        bool need_update = false;
    public:
        explicit EraseType(const T& value, bool flag1, bool flag2) : updateData(value), need_merge(flag1), need_update(flag2){}
    };
    // T需要为pair
    struct KeyEqual{
        bool operator() (const Key obj1, const Key obj2) const {
            if (obj1 < obj2) return false;
            else return true;
        }
    };
    FileSystem<node, info_len> file;// 2个info,暂时先不管
    node root;
    FileSystem<int, 1> spaceFile;
    // 用于查找，减少对页面数据的读取
    // 每次写入文件的时候都插入，否则node可能会更新
    //Yuki::HashMap<int, node, Function, CacheSize, 5> Cache;
    // Reuse the deleted place, in Merging Operation
    // we can also use it in Insert Operation
    Yuki::vector<int> Reuse;
    KeyEqual equal;
    int total = 0; // 总的块数（最多，因为有空间回收）

    ll changeToPos(int index) {
        return info_len * sizeof(int) + (index + 1) * sizeof(node);
    }

    // 没必要每次都写入
    void writeAndCache(node &obj) {
        /*if (!Cache.insert(obj.index_num, obj)) {
            int pos = -1;
            node tmp = Cache.pop(pos);
            file.write(tmp, changeToPos(pos));
        }*/
        file.write(obj, changeToPos(obj.index_num));
    }

    void readAndCache(node &obj, int index) {
        file.read(obj, changeToPos(index));
        //Cache.insert(index, obj);
    }

    // 返回的是新节点的node，或者原来节点+false
    // 更新：返回重复插入的标记
    spiltNode insert(const T&obj, node &objNode) {
        int pos = binarySearch(objNode.key, objNode.len, obj);
        node nextNode;
        spiltNode spilt_node;
        int obj_index;
        if (objNode.key[pos] == obj && pos < objNode.len) {
            obj_index = objNode.sonPos[++pos];
            //spilt_node.exist = true;
            //return spilt_node;
        } else {
            obj_index = objNode.sonPos[pos];
        }
        //if (!Cache.find(obj_index, nextNode)) readAndCache(nextNode, obj_index);
        readAndCache(nextNode, obj_index);
        if (nextNode.isLeaf) {
            spilt_node = insertData(obj, nextNode);
        } else {
            spilt_node = insert(obj, nextNode);
        }
        //if (spilt_node.exist) return spilt_node;
        if (!spilt_node.need_spilt) return spilt_node;
        if (nextNode.isLeaf) {
            // 添加data结点
            return addData(spilt_node.objNode, objNode);
        } else {
            // 添加index结点
            return addIndex(spilt_node.objNode, objNode);
        }
    }

    spiltNode insertData(const T&obj, node &dataNode) {
        int pos = binarySearch(dataNode.key, dataNode.len, obj);
        spiltNode spilt_node;
        /*if (dataNode.key[pos] == obj && pos < dataNode.len) {
            spilt_node.need_spilt = false;
            spilt_node.objNode = dataNode;
            spilt_node.exist = true;
            return spilt_node;
        }*/
        if (dataNode.len < max_size) {
            // 注意不会出现需要更改索引值的情况，因为如果小于第一个值就不会在这个块中进行插入！！！
            for (int i = dataNode.len; i > pos; i--) {
                dataNode.key[i] = dataNode.key[i - 1];
            }
            dataNode.key[pos] = obj;
            dataNode.len++;
            writeAndCache(dataNode);
            spilt_node.objNode = dataNode;
            spilt_node.need_spilt = false;
            return spilt_node;
        }
        // 否则裂块
        int max = max_size / 2;
        node newNode;
        if (Reuse.empty()) {
            newNode.index_num = total;
            total++;
        } else {
            newNode.index_num = Reuse.back();
            Reuse.pop_back();
        }
        newNode.len = 0;
        newNode.isLeaf = true;
        newNode.next_index = dataNode.next_index;
        dataNode.next_index = newNode.index_num;
        // pos <= max 加入前块
        if (pos <= max) {
            for (int i = max; i < max_size; i++) {
                newNode.key[newNode.len] = dataNode.key[i];
                newNode.len++;
            }
            for (int i = max; i > pos; i--) {
                dataNode.key[i] = dataNode.key[i - 1];
            }
            dataNode.key[pos] = obj;
            dataNode.len = max + 1;
        } else {
            for (int i = max; i < max_size; i++) {
                if (i == pos) {
                    newNode.key[newNode.len] = obj;
                    newNode.len++;
                    newNode.key[newNode.len] = dataNode.key[i];
                    newNode.len++;
                    continue;
                }
                newNode.key[newNode.len] = dataNode.key[i];
                newNode.len++;
            }
            if (pos == max_size) {
                newNode.key[newNode.len] = obj;
                newNode.len++;
            }
            dataNode.len = max;
        }
        spilt_node.objNode = newNode;
        spilt_node.need_spilt = true;
        writeAndCache(newNode);
        writeAndCache(dataNode);
        return spilt_node;
    }


    spiltNode addData(node &cur, node &father) {
        int pos = binarySearch(father.key, father.len, cur.key[0]);
        spiltNode spilt_node;
        if (father.len < max_size - 1) {
            for (int i = father.len; i > pos; i--) {
                // 同时改变索引值和键值
                father.key[i] = father.key[i - 1];
                father.sonPos[i + 1] = father.sonPos[i];
            }
            father.key[pos] = cur.key[0];
            father.sonPos[pos + 1] = cur.index_num;
            father.len++;
            writeAndCache(father);
            spilt_node.objNode = father;
            spilt_node.need_spilt = false;
            return spilt_node;
        }
        node newNode;
        newNode.isLeaf = false;
        if (Reuse.empty()) {
            newNode.index_num = total;
            total++;
        } else {
            newNode.index_num = Reuse.back();
            Reuse.pop_back();
        }
        newNode.len = 0;
        int max = (max_size - 1) / 2;// .0.1.   .3.|.4. max = 2 max_size = 6 pos = 4
        if (pos <= max) {
            father.len = max + 1;
            newNode.sonPos[newNode.len] = father.sonPos[max + 1];
            for (int i = max + 1; i < max_size - 1; i++) {
                newNode.key[newNode.len] = father.key[i];
                newNode.len++;
                newNode.sonPos[newNode.len] = father.sonPos[i + 1];
            }
            for (int i = max; i > pos; i--) {
                father.key[i] = father.key[i - 1];
                father.sonPos[i + 1] = father.sonPos[i];
            }
            father.key[pos] = cur.key[0];
            father.sonPos[pos + 1] = cur.index_num;
        } else {
            father.len = max;
            newNode.sonPos[newNode.len] = father.sonPos[max + 1];
            for (int i = max + 1; i < max_size - 1; i++) {
                if (pos == i) {
                    newNode.key[newNode.len] = cur.key[0];
                    newNode.sonPos[newNode.len + 1] = cur.index_num;
                    newNode.len++;
                    newNode.key[newNode.len] = father.key[i];
                    newNode.sonPos[newNode.len + 1] = father.sonPos[i + 1];
                    newNode.len++;
                    continue;
                }
                newNode.key[newNode.len] = father.key[i];
                newNode.sonPos[newNode.len + 1] = father.sonPos[i + 1];
                newNode.len++;
            }
            if (pos == max_size - 1) {
                newNode.key[newNode.len] = cur.key[0];
                newNode.sonPos[newNode.len + 1] = cur.index_num;
                newNode.len++;
            }
        }
        spilt_node.objNode = newNode;
        spilt_node.need_spilt = true;
        writeAndCache(newNode);
        writeAndCache(father);
        return spilt_node;
    }

    spiltNode addIndex(node cur, node &father) {
        T min_index = findMin(cur);
        spiltNode spilt_node;
        int pos = binarySearch(father.key, father.len, min_index);
        if (father.len < max_size - 1) {
            for (int i = father.len; i > pos; i--) {
                father.key[i] = father.key[i - 1];
                father.sonPos[i + 1] = father.sonPos[i];
            }
            father.key[pos] = min_index;
            father.sonPos[pos + 1] = cur.index_num;
            father.len++;
            writeAndCache(father);
            spilt_node.objNode = father;
            spilt_node.need_spilt = false;
            return spilt_node;
        }
        node newNode;
        newNode.isLeaf = false;
        if (Reuse.empty()) {
            newNode.index_num = total;
            total++;
        } else {
            newNode.index_num = Reuse.back();
            Reuse.pop_back();
        }
        newNode.len = 0;
        int max = (max_size - 1) / 2;// .0.1.   .3.|.4. max = 2 max_size = 6 pos = 4
        if (pos <= max) {
            father.len = max + 1;
            newNode.sonPos[newNode.len] = father.sonPos[max + 1];
            for (int i = max + 1; i < max_size - 1; i++) {
                newNode.key[newNode.len] = father.key[i];
                newNode.len++;
                newNode.sonPos[newNode.len] = father.sonPos[i + 1];
            }
            for (int i = max; i > pos; i--) {
                father.key[i] = father.key[i - 1];
                father.sonPos[i + 1] = father.sonPos[i];
            }
            father.key[pos] = min_index;
            father.sonPos[pos + 1] = cur.index_num;
        } else {
            father.len = max;
            newNode.sonPos[newNode.len] = father.sonPos[max + 1];
            for (int i = max + 1; i < max_size - 1; i++) {
                if (pos == i) {
                    newNode.key[newNode.len] = min_index;
                    newNode.sonPos[newNode.len + 1] = cur.index_num;
                    newNode.len++;
                    newNode.key[newNode.len] = father.key[i];
                    newNode.sonPos[newNode.len + 1] = father.sonPos[i + 1];
                    newNode.len++;
                    continue;
                }
                newNode.key[newNode.len] = father.key[i];
                newNode.sonPos[newNode.len + 1] = father.sonPos[i + 1];
                newNode.len++;
            }
            if (pos == max_size - 1) {
                newNode.key[newNode.len] = min_index;
                newNode.sonPos[newNode.len + 1] = cur.index_num;
                newNode.len++;
            }
        }
        spilt_node.objNode = newNode;
        spilt_node.need_spilt = true;
        writeAndCache(newNode);
        writeAndCache(father);
        return spilt_node;

    }

    T findMin(node tmp) {
        if (tmp.isLeaf) return tmp.key[0];
        int index = tmp.sonPos[0];
        node newNode;
        //if (!Cache.find(index, newNode)) readAndCache(newNode, index);
        readAndCache(newNode, index);
        return findMin(newNode);
    }

    // 二分查找函数,查找第一个不大于obj的位置，如果位置为max_size-1，则是最后一个son块（key比son要少一个）
    int binarySearch(T array[], int len, const T& obj) {
        int l = 0, r = len;
        while (l < r) {
            int mid = (l + r) / 2;
            if (array[mid] >= obj) r = mid;
            else l = mid + 1;
        }
        return l;
    }

    int keyBinarySearch(T array[], int len, const Key obj) {
        int l = 0, r = len;
        while (l < r) {
            int mid = (l + r) / 2;
            if (equal(array[mid].first, obj)) r = mid;
            else l = mid + 1;
        }
        return l;
    }

    // 删除条目可能会不存在
    // true--Merge / Borrow  false--no operation
    EraseType erase(const T& obj, node &objNode) {
        int pos = binarySearch(objNode.key, objNode.len, obj);
        if (!objNode.isLeaf) {
            int index;
            if (objNode.key[pos] == obj && pos < objNode.len) {
                index = objNode.sonPos[++pos];
            } else index = objNode.sonPos[pos];
            node nextNode;
            //if (!Cache.find(index, nextNode)) readAndCache(nextNode, index);
            readAndCache(nextNode, index);
            if (nextNode.isLeaf) {
                EraseType dataType = dataErase(obj, nextNode);
                if (!dataType.need_merge && !dataType.need_update) return dataType;
                if (!dataType.need_merge && dataType.need_update) {
                    if (pos != 0) {
                        objNode.key[pos - 1] = dataType.updateData;
                        writeAndCache(objNode);
                        dataType.need_update = false;
                        return dataType;
                    } else {
                        return dataType;
                    }
                }
                if (dataType.need_merge && !dataType.need_update) {
                    // 区分是否是objNode的最后一个块
                    return MergeData(pos, objNode, nextNode, dataType);
                }
                if (dataType.need_merge && dataType.need_update) {
                    dataType = MergeData(pos, objNode, nextNode, dataType);
                    if (pos != 0) {
                        objNode.key[pos - 1] = dataType.updateData;
                        dataType.need_update = false;
                        writeAndCache(objNode);
                        return dataType;
                    }
                    return dataType;
                }
            } else {
                EraseType indexType = erase(obj, nextNode);
                if (!indexType.need_merge && !indexType.need_update) return indexType;
                if (!indexType.need_merge && indexType.need_update) {
                    if (pos != 0) {
                        objNode.key[pos - 1] = indexType.updateData;
                        writeAndCache(objNode);
                        indexType.need_update = false;
                        return indexType;
                    } else return indexType;
                }
                if (indexType.need_merge && !indexType.need_update) {
                    return MergeIndex(pos, objNode, nextNode, indexType);
                }
                if (indexType.need_merge && indexType.need_update) {
                    indexType = MergeIndex(pos, objNode, nextNode, indexType);
                    if (pos != 0) {
                        objNode.key[pos - 1] = indexType.updateData;
                        indexType.need_update = false;
                        writeAndCache(objNode);
                        return indexType;
                    } else return indexType;
                }
            }
        } else {
            EraseType dataType = dataErase(obj, objNode);
            writeAndCache(objNode);
            return dataType;
        }
    }

    EraseType MergeData(int pos, node &objNode, node &nextNode, EraseType &dataType) {
        int min_size = max_size / 2;
        if (pos != objNode.len) {
            int brother_index = objNode.sonPos[pos + 1];
            node brother;// brother是data块
            //if (!Cache.find(brother_index, brother)) readAndCache(brother, brother_index);
            readAndCache(brother, brother_index);
            // 借块或元素
            if (brother.len > min_size) {
                nextNode.key[nextNode.len] = brother.key[0];
                for (int i = 0; i < brother.len - 1; i++) brother.key[i] = brother.key[i + 1];
                nextNode.len++;
                brother.len--;
                objNode.key[pos] = brother.key[0];
                writeAndCache(objNode);
                writeAndCache(brother);
                writeAndCache(nextNode);
                dataType.need_merge = false;
                return dataType;
            } else {
                for (int i = nextNode.len, j = 0; i < nextNode.len + brother.len; i++, j++) {
                    nextNode.key[i] = brother.key[j];
                }
                nextNode.len += brother.len;
                nextNode.next_index = brother.next_index;
                brother.next_index = -1;
                // 更新objNode brother-->pos
                for (int i = pos; i < objNode.len - 1; i++) {
                    objNode.key[i] = objNode.key[i + 1];
                    objNode.sonPos[i + 1] = objNode.sonPos[i + 2];
                }
                objNode.len--;
                writeAndCache(nextNode);
                writeAndCache(objNode);
                //Cache.erase(brother.index_num);
                Reuse.push_back(brother.index_num);
                dataType.updateData = nextNode.key[0];
                if (objNode.len >= min_size) {
                    dataType.need_merge = false;
                    return dataType;
                } else return dataType;
            }
        } else {
            int brother_index = objNode.sonPos[pos - 1];
            node brother;// brother是data块
            //if (!Cache.find(brother_index, brother)) readAndCache(brother, brother_index);
            readAndCache(brother, brother_index);
            if (brother.len > min_size) {
                // 借brother的最后一个元素
                for (int i = nextNode.len; i > 0; i--) {
                    nextNode.key[i] = nextNode.key[i - 1];
                }
                nextNode.len++;
                nextNode.key[0] = brother.key[brother.len - 1];
                brother.len--;
                objNode.key[pos - 1] = nextNode.key[0];
                dataType.updateData = nextNode.key[0];
                writeAndCache(nextNode);
                writeAndCache(brother);
                writeAndCache(objNode);
                dataType.need_merge = false;
                return dataType;
            } else {
                // 向前并块
                for (int i = brother.len, j = 0; i < brother.len + nextNode.len; i++, j++) {
                    brother.key[i] = nextNode.key[j];
                }
                brother.len += nextNode.len;
                brother.next_index = nextNode.next_index;
                nextNode.next_index = -1;
                objNode.len--;
                writeAndCache(brother);
                writeAndCache(objNode);
                //Cache.erase(nextNode.index_num);
                Reuse.push_back(nextNode.index_num);
                if (objNode.len >= min_size) {
                    dataType.need_merge = false;
                    return dataType;
                } else return dataType;
            }
        }
    }

    EraseType MergeIndex(int pos, node &objNode, node &nextNode, EraseType &indexType){
        int min_size = (max_size - 1) / 2;
        if (pos != objNode.len ) {
            int brother_index = objNode.sonPos[pos + 1];
            node brother;
            //if (!Cache.find(brother_index, brother)) readAndCache(brother, brother_index);
            readAndCache(brother, brother_index);
            if (brother.len > min_size) {
                // 将brother 的第一个块借给nextNode
                T min_index = findMin(brother);
                nextNode.key[nextNode.len] = min_index;
                nextNode.sonPos[nextNode.len + 1] = brother.sonPos[0];
                nextNode.len++;
                for (int i = 0; i < brother.len - 1; i++) {
                    brother.sonPos[i] = brother.sonPos[i + 1];
                    brother.key[i] = brother.key[i + 1];
                }
                brother.sonPos[brother.len - 1] = brother.sonPos[brother.len];
                brother.len--;
                objNode.key[pos] = findMin(brother);
                writeAndCache(brother);
                writeAndCache(objNode);
                writeAndCache(nextNode);
                indexType.need_merge = false;
                return indexType;
            } else {
                // 将brother Merge 到nextNode上
                nextNode.key[nextNode.len] = findMin(brother);
                nextNode.sonPos[nextNode.len + 1] = brother.sonPos[0];
                for (int i = nextNode.len + 1, j = 0; i < nextNode.len + brother.len + 1; i++, j++) {
                    nextNode.key[i] = brother.key[j];
                    nextNode.sonPos[i + 1] = brother.sonPos[j + 1];
                }
                nextNode.len += (brother.len + 1);
                for (int i = pos; i < objNode.len - 1; i++) {
                    objNode.key[i] = objNode.key[i + 1];
                    objNode.sonPos[i + 1] = objNode.sonPos[i + 2];
                }
                objNode.len--;
                Reuse.push_back(brother.index_num);
                //Cache.erase(brother.index_num);
                writeAndCache(objNode);
                writeAndCache(nextNode);
                if (objNode.len >= min_size) {
                    indexType.need_merge = false;
                    return indexType;
                } else return indexType;
            }
        } else {
            int brother_index = objNode.sonPos[pos - 1];
            node brother;
            //if (!Cache.find(brother_index, brother)) readAndCache(brother, brother_index);
            readAndCache(brother, brother_index);
            if (brother.len > min_size) {
                T min_num = findMin(nextNode);
                for (int i = nextNode.len; i > 0; i--) {
                    nextNode.key[i] = nextNode.key[i - 1];
                    nextNode.sonPos[i + 1] = nextNode.sonPos[i];
                }
                nextNode.sonPos[1] = nextNode.sonPos[0];
                nextNode.key[0] = min_num;
                nextNode.sonPos[0] = brother.sonPos[brother.len];
                objNode.key[pos - 1] = findMin(nextNode);
                indexType.updateData = objNode.key[pos - 1];
                brother.len--;
                nextNode.len++;
                writeAndCache(nextNode);
                writeAndCache(brother);
                writeAndCache(objNode);
                indexType.need_merge = false;
                return indexType;
            } else {
                // 向前并块
                T min_num = findMin(nextNode);
                brother.key[brother.len] = min_num;
                brother.sonPos[brother.len + 1] = nextNode.sonPos[0];
                for (int i = brother.len + 1,j = 0; i < brother.len + nextNode.len + 1; i++, j++) {
                    brother.key[i] = nextNode.key[j];
                    brother.sonPos[i + 1] = nextNode.sonPos[j + 1];
                }
                brother.len += (nextNode.len + 1);
                objNode.len--;
                //Cache.erase(nextNode.index_num);
                Reuse.push_back(nextNode.index_num);
                writeAndCache(brother);
                writeAndCache(objNode);
                if (objNode.len >= min_size) {
                    indexType.need_merge = false;
                    return indexType;
                } else return indexType;
            }
        }
    }

    EraseType dataErase(const T& obj, node &objNode) {
        int pos = binarySearch(objNode.key, objNode.len, obj);
        if (objNode.key[pos] != obj) {
            EraseType type(objNode.key[0], false, false);
            return type;
        }
        int min_size = max_size / 2;
        // pos是objNode.key[]数组中key的位置
        for (int i = pos; i < objNode.len - 1; i++) {
            objNode.key[i] = objNode.key[i + 1];
        }
        objNode.len--;
        if (objNode.len >= min_size) {
            writeAndCache(objNode);
            if (pos != 0) {
                EraseType type(objNode.key[0], false, false);
                return type;
            } else {
                EraseType type(objNode.key[0], false, true);
                return type;
            }
        }
        if (pos != 0) {
            EraseType type(objNode.key[0], true, false);
            return type;
        } else {
            EraseType type(objNode.key[0], true, true);
            return type;
        }
    }


public:
    explicit BPT(const string &filename, const string &space) {
        file.initialise(filename); // 创建文件
        file.clear();
        spaceFile.initialise(space);
        spaceFile.clear();
        int all;
        spaceFile.get_info(all, 1);
        if (all != 0) {
            for (int i = 0; i < all; i++) {
                int x;
                spaceFile.read(x, (i + 1) * sizeof(int));
                Reuse.push_back(x);
            }
        }
        file.get_info(total, 1);
        if (total != 0) file.read(root, sizeof(int) * info_len);
    }
    ~BPT() {
        // 把缓存中的元素写入文件当中
        // 先缓存，在弹出的时候再写入文件
        //Cache.clear_(file, info_len);
        spaceFile.write_info(Reuse.size(), 1);
        for (int i = 0; i < Reuse.size(); i++) {
            spaceFile.write(Reuse[i], (i + 1) * sizeof(int));
        }
        file.write_info(total, 1);
        file.write(root, info_len * sizeof(int));
        file.clear();
    }

    // 便于查找父节点，我们进行递归插入
    // true 代表成功插入
    bool insert(const T& x) {
        if (total == 0) {
            // 说明是开头
            if (Reuse.empty()) {
                root_num = total;
                total++;
            } else {
                root_num = Reuse[Reuse.size() - 1];
                Reuse.pop_back();
            }
            node root_(false, root_num);
            int leaf_num;
            if (Reuse.empty()) {
                leaf_num = total;
                total++;
            } else {
                leaf_num = Reuse[Reuse.size() - 1];
                Reuse.pop_back();
            }
            root_.sonPos[root_.len] = leaf_num;
            root = root_;
            node newLeaf(true, leaf_num);
            newLeaf.key[newLeaf.len] = x;
            newLeaf.len++;
            newLeaf.index_num = leaf_num;
            writeAndCache(root);
            writeAndCache(newLeaf);
            return true;
        }
        spiltNode spilt_node = insert(x, root);
        //if (spilt_node.exist) return false;
        if (spilt_node.need_spilt) {
            node newRoot;
            newRoot.isLeaf = false;
            if (Reuse.empty()) {
                newRoot.index_num = total;
                total++;
            } else {
                newRoot.index_num = Reuse.back();
                Reuse.pop_back();
            }
            newRoot.len = 1;
            newRoot.sonPos[0] = root.index_num;
            newRoot.sonPos[1] = spilt_node.objNode.index_num;
            T min_index = findMin(spilt_node.objNode);
            newRoot.key[0] = min_index;
            root = newRoot;
            writeAndCache(newRoot);
        }
        return true;
    }

    // 如果是最后一个就向前借块，否则向后借块，减少对索引值的更改
    void erase(const T& x) {
        int pos = binarySearch(root.key, root.len, x);
        int index;
        if (root.key[pos] == x && pos < root.len) {
            index = root.sonPos[++pos];
        } else index = root.sonPos[pos];
        node nextNode;
        //if (!Cache.find(index, nextNode)) readAndCache(nextNode, index);
        readAndCache(nextNode, index);
        EraseType type = erase(x, nextNode);
        if (!type.need_update && !type.need_merge) return;
        if (!type.need_merge && type.need_update) {
            if (pos != 0) root.key[pos - 1] = type.updateData;
            return;
        }
        if (type.need_merge && !type.need_update) {
            if (root.len != 0) {
                if (!nextNode.isLeaf) MergeIndex(pos, root, nextNode, type);
                else MergeData(pos, root, nextNode, type);
                return;
            } else {
                int son_index = root.sonPos[0];
                node newRoot;
                //if (!Cache.find(son_index, newRoot)) readAndCache(newRoot, son_index);
                readAndCache(newRoot, son_index);
                if (!newRoot.isLeaf) {
                    //Cache.erase(root.index_num);
                    Reuse.push_back(root.index_num);
                    root = newRoot;
                    return;
                } else return;
            }
        }
        if (type.need_update && type.need_merge) {
            if (pos != 0) root.key[pos - 1] = type.updateData;
            if (root.len != 0) {
                if (!nextNode.isLeaf) MergeIndex(pos, root, nextNode, type);
                else MergeData(pos, root, nextNode, type);
                return;
            } else {
                int son_index = root.sonPos[0];
                node newRoot;
                //if (!Cache.find(son_index, newRoot)) readAndCache(newRoot, son_index);
                readAndCache(newRoot, son_index);
                if (newRoot.isLeaf) return;
                //Cache.erase(root.index_num);
                Reuse.push_back(root.index_num);
                root = newRoot;
                return;
            }
        }
    }

    Yuki::vector<Value> find(const Key key) {
        Yuki::vector<Value> all;
        int pos = keyBinarySearch(root.key, root.len, key);
        node nextNode;
        int index = root.sonPos[pos];
        //if (!Cache.find(index, nextNode)) readAndCache(nextNode, index);
        readAndCache(nextNode, index);
        while (!nextNode.isLeaf) {
            int pos_ = keyBinarySearch(nextNode.key, nextNode.len, key);
            int index_ = nextNode.sonPos[pos_];
            //if (!Cache.find(index_, nextNode)) readAndCache(nextNode, index_);
            readAndCache(nextNode, index_);
        }
        int pos_ = keyBinarySearch(nextNode.key, nextNode.len, key);
        if (pos_ == nextNode.len) {
            if (nextNode.next_index == -1) return all;
            //if (!Cache.find(nextNode.next_index, nextNode)) readAndCache(nextNode, nextNode.next_index);
            readAndCache(nextNode, nextNode.next_index);
            pos_ = keyBinarySearch(nextNode.key, nextNode.len, key);
            if (nextNode.key[pos_].first != key) return all;
        } else if (nextNode.key[pos_].first != key) return all;
        for (int i = pos_; i < nextNode.len; i++) {
            if (nextNode.key[i].first == key) all.push_back(nextNode.key[i].second);
            else return all;
        }
        while (true) {
            if (nextNode.next_index == -1) return all;
            //if (!Cache.find(nextNode.next_index, nextNode)) readAndCache(nextNode, nextNode.next_index);
            readAndCache(nextNode, nextNode.next_index);
            for (int i = 0; i < nextNode.len; i++) {
                if (nextNode.key[i].first == key) all.push_back(nextNode.key[i].second);
                else return all;
            }
        }
    }

    bool empty() {
        if (total == 0) return true;
        else return false;
    }

    bool findKV(const Key key, Value &obj) {
        int pos = keyBinarySearch(root.key, root.len, key);
        node nextNode;
        if (root.key[pos].first == key) pos++;
        int index = root.sonPos[pos];
        //if (!Cache.find(index, nextNode)) readAndCache(nextNode, index);
        readAndCache(nextNode, index);
        while (!nextNode.isLeaf) {
            int pos_ = keyBinarySearch(nextNode.key, nextNode.len, key);
            if (nextNode.key[pos_].first == key) pos_++;
            int index_ = nextNode.sonPos[pos_];
            //if (!Cache.find(index_, nextNode)) readAndCache(nextNode, index_);
            readAndCache(nextNode, index_);
        }
        int pos_ = keyBinarySearch(nextNode.key, nextNode.len, key);
        if (nextNode.key[pos_].first != key) return false;
        else {
            obj = nextNode.key[pos_].second;
            return true;
        }
    }

    void update(const T &obj) {
        int pos = binarySearch(root.key, root.len, obj);
        node nextNode;
        if (root.key[pos] == obj) pos++;
        int index = root.sonPos[pos];
        //if (!Cache.find(index, nextNode)) readAndCache(nextNode, index);
        readAndCache(nextNode, index);
        while (!nextNode.isLeaf) {
            int pos_ = binarySearch(nextNode.key, nextNode.len, obj);
            if (nextNode.key[pos_] == obj) pos_++;
            int index_ = nextNode.sonPos[pos_];
            //if (!Cache.find(index_, nextNode)) readAndCache(nextNode, index_);
            readAndCache(nextNode, index_);
        }
        int pos_ = binarySearch(nextNode.key, nextNode.len, obj);
        nextNode.key[pos_] = obj;
        writeAndCache(nextNode);
    }
};

template<class Value, class Function,int max_size, int info_len,int CacheSize>
class BPT<char, Value, Function, max_size, info_len, CacheSize>{
    friend class UserManagement;
    typedef Yuki::pair<char, Value> T;
private:
    int root_num = 0;// 记录root的pos
    class node{
        friend class BPT<char, Value, Function, max_size, info_len, CacheSize>;
        friend class UserManagement;
    private:
        bool isLeaf;
        T key[max_size];// 如果是leaf则为max_size,否则是max_size - 1,并且索引块的值之前是指向的位置
        int sonPos[max_size] = {0};// 记录索引页的位置，但如果是leaf则不需要
        int index_num;// 储存这是创建的第几个块
        int len; // 记录当前填充长度，用于裂块和并块，是key的长度
        int next_index = -1; // 有next就赋值，只给叶子结点next_index
    public:
        node():isLeaf(true), len(0), index_num(0){}
        explicit node(bool is_leaf, int num):isLeaf(is_leaf), index_num(num), len(0){}
        node(const node& other) {
            len = other.len;
            isLeaf = other.isLeaf;
            index_num = other.index_num;
            next_index = other.next_index;
            for (int i = 0; i < max_size; i++) {
                key[i] = other.key[i];
                sonPos[i] = other.sonPos[i];
            }
        }

        node& operator=(const node& other) {
            if (this == &other) return *this;
            len = other.len;
            isLeaf = other.isLeaf;
            index_num = other.index_num;
            next_index = other.next_index;
            if (!isLeaf) {
                for (int i = 0; i < len + 1; i++) {
                    key[i] = other.key[i];
                    sonPos[i] = other.sonPos[i];
                }
            } else {
                for (int i = 0; i < len; i++) {
                    key[i] = other.key[i];
                }
            }
            return *this;
        }

        bool operator==(const node& other) const {
            return index_num == other.index_num;
        }

    };
    struct spiltNode{
        node objNode;
        bool need_spilt = false;
        //bool exist = false;
        spiltNode& operator=(const spiltNode &other) {
            if (this == &other) return *this;
            objNode = other.objNode;
            need_spilt = other.need_spilt;
            //exist = other.exist;
            return *this;
        }

        bool operator==(const spiltNode& other) const {
            return objNode == other.objNode;
        }
    };
    class EraseType{
        friend class BPT<char, Value, Function, max_size, info_len, CacheSize>;
    private:
        T updateData;
        bool need_merge = false;
        bool need_update = false;
    public:
        explicit EraseType(const T& value, bool flag1, bool flag2) : updateData(value), need_merge(flag1), need_update(flag2){}
    };
    // T需要为pair
    struct KeyEqual{
        bool operator() (const char obj1[], const char obj2[]) const {
            if (strcmp(obj1, obj2) < 0) return false;
            else return true;
        }
    };
    FileSystem<node, info_len> file;// 2个info,暂时先不管
    node root;
    FileSystem<int, 1> spaceFile;
    // Yuki::HashMap<int, node, Function, CacheSize, 1019> Cache;
    // Reuse the deleted place, in Merging Operation
    // we can also use it in Insert Operation
    Yuki::vector<int> Reuse;
    KeyEqual equal;
    int total = 0; // 总的块数（最多，因为有空间回收）

    ll changeToPos(int index) {
        return info_len * sizeof(int) + (index + 1) * sizeof(node);
    }

    // 没必要每次都写入
    void writeAndCache(node &obj) {
        /*if (!Cache.insert(obj.index_num, obj)) {
            int pos = -1;
            node tmp = Cache.pop(pos);
            file.write(tmp, changeToPos(pos));
        }*/
        file.write(obj, changeToPos(obj.index_num));
    }

    void readAndCache(node &obj, int index) {
        file.read(obj, changeToPos(index));
        //Cache.insert(index, obj);
    }

    // 返回的是新节点的node，或者原来节点+false
    // 更新：返回重复插入的标记
    spiltNode insert(const T&obj, node &objNode) {
        int pos = binarySearch(objNode.key, objNode.len, obj);
        node nextNode;
        spiltNode spilt_node;
        int obj_index;
        if (objNode.key[pos] == obj && pos < objNode.len) {
            obj_index = objNode.sonPos[++pos];
            //spilt_node.exist = true;
            //return spilt_node;
        } else {
            obj_index = objNode.sonPos[pos];
        }
        //if (!Cache.find(obj_index, nextNode)) readAndCache(nextNode, obj_index);
        readAndCache(nextNode, obj_index);
        if (nextNode.isLeaf) {
            spilt_node = insertData(obj, nextNode);
        } else {
            spilt_node = insert(obj, nextNode);
        }
        //if (spilt_node.exist) return spilt_node;
        if (!spilt_node.need_spilt) return spilt_node;
        if (nextNode.isLeaf) {
            // 添加data结点
            return addData(spilt_node.objNode, objNode);
        } else {
            // 添加index结点
            return addIndex(spilt_node.objNode, objNode);
        }
    }

    spiltNode insertData(const T &obj, node &dataNode) {
        int pos = binarySearch(dataNode.key, dataNode.len, obj);
        spiltNode spilt_node;
        /*if (dataNode.key[pos] == obj && pos < dataNode.len) {
            spilt_node.need_spilt = false;
            spilt_node.objNode = dataNode;
            spilt_node.exist = true;
            return spilt_node;
        }*/
        if (dataNode.len < max_size) {
            // 注意不会出现需要更改索引值的情况，因为如果小于第一个值就不会在这个块中进行插入！！！
            for (int i = dataNode.len; i > pos; i--) {
                dataNode.key[i] = dataNode.key[i - 1];
            }
            dataNode.key[pos] = obj;
            dataNode.len++;
            writeAndCache(dataNode);
            spilt_node.objNode = dataNode;
            spilt_node.need_spilt = false;
            return spilt_node;
        }
        // 否则裂块
        int max = max_size / 2;
        node newNode;
        if (Reuse.empty()) {
            newNode.index_num = total;
            total++;
        } else {
            newNode.index_num = Reuse.back();
            Reuse.pop_back();
        }
        newNode.len = 0;
        newNode.isLeaf = true;
        newNode.next_index = dataNode.next_index;
        dataNode.next_index = newNode.index_num;
        // pos <= max 加入前块
        if (pos <= max) {
            for (int i = max; i < max_size; i++) {
                newNode.key[newNode.len] = dataNode.key[i];
                newNode.len++;
            }
            for (int i = max; i > pos; i--) {
                dataNode.key[i] = dataNode.key[i - 1];
            }
            dataNode.key[pos] = obj;
            dataNode.len = max + 1;
        } else {
            for (int i = max; i < max_size; i++) {
                if (i == pos) {
                    newNode.key[newNode.len] = obj;
                    newNode.len++;
                    newNode.key[newNode.len] = dataNode.key[i];
                    newNode.len++;
                    continue;
                }
                newNode.key[newNode.len] = dataNode.key[i];
                newNode.len++;
            }
            if (pos == max_size) {
                newNode.key[newNode.len] = obj;
                newNode.len++;
            }
            dataNode.len = max;
        }
        spilt_node.objNode = newNode;
        spilt_node.need_spilt = true;
        writeAndCache(newNode);
        writeAndCache(dataNode);
        return spilt_node;
    }

    spiltNode addData(node &cur, node &father) {
        int pos = binarySearch(father.key, father.len, cur.key[0]);
        spiltNode spilt_node;
        if (father.len < max_size - 1) {
            for (int i = father.len; i > pos; i--) {
                // 同时改变索引值和键值
                father.key[i] = father.key[i - 1];
                father.sonPos[i + 1] = father.sonPos[i];
            }
            father.key[pos] = cur.key[0];
            father.sonPos[pos + 1] = cur.index_num;
            father.len++;
            writeAndCache(father);
            spilt_node.objNode = father;
            spilt_node.need_spilt = false;
            return spilt_node;
        }
        node newNode;
        newNode.isLeaf = false;
        if (Reuse.empty()) {
            newNode.index_num = total;
            total++;
        } else {
            newNode.index_num = Reuse.back();
            Reuse.pop_back();
        }
        newNode.len = 0;
        int max = (max_size - 1) / 2;// .0.1.   .3.|.4. max = 2 max_size = 6 pos = 4
        if (pos <= max) {
            father.len = max + 1;
            newNode.sonPos[newNode.len] = father.sonPos[max + 1];
            for (int i = max + 1; i < max_size - 1; i++) {
                newNode.key[newNode.len] = father.key[i];
                newNode.len++;
                newNode.sonPos[newNode.len] = father.sonPos[i + 1];
            }
            for (int i = max; i > pos; i--) {
                father.key[i] = father.key[i - 1];
                father.sonPos[i + 1] = father.sonPos[i];
            }
            father.key[pos] = cur.key[0];
            father.sonPos[pos + 1] = cur.index_num;
        } else {
            father.len = max;
            newNode.sonPos[newNode.len] = father.sonPos[max + 1];
            for (int i = max + 1; i < max_size - 1; i++) {
                if (pos == i) {
                    newNode.key[newNode.len] = cur.key[0];
                    newNode.sonPos[newNode.len + 1] = cur.index_num;
                    newNode.len++;
                    newNode.key[newNode.len] = father.key[i];
                    newNode.sonPos[newNode.len + 1] = father.sonPos[i + 1];
                    newNode.len++;
                    continue;
                }
                newNode.key[newNode.len] = father.key[i];
                newNode.sonPos[newNode.len + 1] = father.sonPos[i + 1];
                newNode.len++;
            }
            if (pos == max_size - 1) {
                newNode.key[newNode.len] = cur.key[0];
                newNode.sonPos[newNode.len + 1] = cur.index_num;
                newNode.len++;
            }
        }
        spilt_node.objNode = newNode;
        spilt_node.need_spilt = true;
        writeAndCache(newNode);
        writeAndCache(father);
        return spilt_node;
    }

    spiltNode addIndex(node cur, node &father) {
        T min_index = findMin(cur);
        spiltNode spilt_node;
        int pos = binarySearch(father.key, father.len, min_index);
        if (father.len < max_size - 1) {
            for (int i = father.len; i > pos; i--) {
                father.key[i] = father.key[i - 1];
                father.sonPos[i + 1] = father.sonPos[i];
            }
            father.key[pos] = min_index;
            father.sonPos[pos + 1] = cur.index_num;
            father.len++;
            writeAndCache(father);
            spilt_node.objNode = father;
            spilt_node.need_spilt = false;
            return spilt_node;
        }
        node newNode;
        newNode.isLeaf = false;
        if (Reuse.empty()) {
            newNode.index_num = total;
            total++;
        } else {
            newNode.index_num = Reuse.back();
            Reuse.pop_back();
        }
        newNode.len = 0;
        int max = (max_size - 1) / 2;// .0.1.   .3.|.4. max = 2 max_size = 6 pos = 4
        if (pos <= max) {
            father.len = max + 1;
            newNode.sonPos[newNode.len] = father.sonPos[max + 1];
            for (int i = max + 1; i < max_size - 1; i++) {
                newNode.key[newNode.len] = father.key[i];
                newNode.len++;
                newNode.sonPos[newNode.len] = father.sonPos[i + 1];
            }
            for (int i = max; i > pos; i--) {
                father.key[i] = father.key[i - 1];
                father.sonPos[i + 1] = father.sonPos[i];
            }
            father.key[pos] = min_index;
            father.sonPos[pos + 1] = cur.index_num;
        } else {
            father.len = max;
            newNode.sonPos[newNode.len] = father.sonPos[max + 1];
            for (int i = max + 1; i < max_size - 1; i++) {
                if (pos == i) {
                    newNode.key[newNode.len] = min_index;
                    newNode.sonPos[newNode.len + 1] = cur.index_num;
                    newNode.len++;
                    newNode.key[newNode.len] = father.key[i];
                    newNode.sonPos[newNode.len + 1] = father.sonPos[i + 1];
                    newNode.len++;
                    continue;
                }
                newNode.key[newNode.len] = father.key[i];
                newNode.sonPos[newNode.len + 1] = father.sonPos[i + 1];
                newNode.len++;
            }
            if (pos == max_size - 1) {
                newNode.key[newNode.len] = min_index;
                newNode.sonPos[newNode.len + 1] = cur.index_num;
                newNode.len++;
            }
        }
        spilt_node.objNode = newNode;
        spilt_node.need_spilt = true;
        writeAndCache(newNode);
        writeAndCache(father);
        return spilt_node;

    }

    T findMin(node tmp) {
        if (tmp.isLeaf) return tmp.key[0];
        int index = tmp.sonPos[0];
        node newNode;
        //if (!Cache.find(index, newNode)) readAndCache(newNode, index);
        readAndCache(newNode, index);
        return findMin(newNode);
    }

    // 二分查找函数,查找第一个不大于obj的位置，如果位置为max_size-1，则是最后一个son块（key比son要少一个）
    int binarySearch(T array[], int len, const T& obj) {
        int l = 0, r = len;
        while (l < r) {
            int mid = (l + r) / 2;
            if (array[mid] >= obj) r = mid;
            else l = mid + 1;
        }
        return l;
    }

    int keyBinarySearch(T array[], int len, const char obj[]) {
        int l = 0, r = len;
        while (l < r) {
            int mid = (l + r) / 2;
            if (equal(array[mid].first, obj)) r = mid;
            else l = mid + 1;
        }
        return l;
    }

    // 删除条目可能会不存在
    // true--Merge / Borrow  false--no operation
    EraseType erase(const T& obj, node &objNode) {
        int pos = binarySearch(objNode.key, objNode.len, obj);
        if (!objNode.isLeaf) {
            int index;
            if (objNode.key[pos] == obj && pos < objNode.len) {
                index = objNode.sonPos[++pos];
            } else index = objNode.sonPos[pos];
            node nextNode;
            //if (!Cache.find(index, nextNode)) readAndCache(nextNode, index);
            readAndCache(nextNode, index);
            if (nextNode.isLeaf) {
                EraseType dataType = dataErase(obj, nextNode);
                if (!dataType.need_merge && !dataType.need_update) return dataType;
                if (!dataType.need_merge && dataType.need_update) {
                    if (pos != 0) {
                        objNode.key[pos - 1] = dataType.updateData;
                        writeAndCache(objNode);
                        dataType.need_update = false;
                        return dataType;
                    } else {
                        return dataType;
                    }
                }
                if (dataType.need_merge && !dataType.need_update) {
                    // 区分是否是objNode的最后一个块
                    return MergeData(pos, objNode, nextNode, dataType);
                }
                if (dataType.need_merge && dataType.need_update) {
                    dataType = MergeData(pos, objNode, nextNode, dataType);
                    if (pos != 0) {
                        objNode.key[pos - 1] = dataType.updateData;
                        dataType.need_update = false;
                        writeAndCache(objNode);
                        return dataType;
                    }
                    return dataType;
                }
            } else {
                EraseType indexType = erase(obj, nextNode);
                if (!indexType.need_merge && !indexType.need_update) return indexType;
                if (!indexType.need_merge && indexType.need_update) {
                    if (pos != 0) {
                        objNode.key[pos - 1] = indexType.updateData;
                        writeAndCache(objNode);
                        indexType.need_update = false;
                        return indexType;
                    } else return indexType;
                }
                if (indexType.need_merge && !indexType.need_update) {
                    return MergeIndex(pos, objNode, nextNode, indexType);
                }
                if (indexType.need_merge && indexType.need_update) {
                    indexType = MergeIndex(pos, objNode, nextNode, indexType);
                    if (pos != 0) {
                        objNode.key[pos - 1] = indexType.updateData;
                        indexType.need_update = false;
                        writeAndCache(objNode);
                        return indexType;
                    } else return indexType;
                }
            }
        } else {
            EraseType dataType = dataErase(obj, objNode);
            writeAndCache(objNode);
            return dataType;
        }
    }

    EraseType MergeData(int pos, node &objNode, node &nextNode, EraseType &dataType) {
        int min_size = max_size / 2;
        if (pos != objNode.len) {
            int brother_index = objNode.sonPos[pos + 1];
            node brother;// brother是data块
            //if (!Cache.find(brother_index, brother)) readAndCache(brother, brother_index);
            readAndCache(brother, brother_index);
            // 借块或元素
            if (brother.len > min_size) {
                nextNode.key[nextNode.len] = brother.key[0];
                for (int i = 0; i < brother.len - 1; i++) brother.key[i] = brother.key[i + 1];
                nextNode.len++;
                brother.len--;
                objNode.key[pos] = brother.key[0];
                writeAndCache(objNode);
                writeAndCache(brother);
                writeAndCache(nextNode);
                dataType.need_merge = false;
                return dataType;
            } else {
                for (int i = nextNode.len, j = 0; i < nextNode.len + brother.len; i++, j++) {
                    nextNode.key[i] = brother.key[j];
                }
                nextNode.len += brother.len;
                nextNode.next_index = brother.next_index;
                brother.next_index = -1;
                // 更新objNode brother-->pos
                for (int i = pos; i < objNode.len - 1; i++) {
                    objNode.key[i] = objNode.key[i + 1];
                    objNode.sonPos[i + 1] = objNode.sonPos[i + 2];
                }
                objNode.len--;
                writeAndCache(nextNode);
                writeAndCache(objNode);
                //Cache.erase(brother.index_num);
                Reuse.push_back(brother.index_num);
                dataType.updateData = nextNode.key[0];
                if (objNode.len >= min_size) {
                    dataType.need_merge = false;
                    return dataType;
                } else return dataType;
            }
        } else {
            int brother_index = objNode.sonPos[pos - 1];
            node brother;// brother是data块
            //if (!Cache.find(brother_index, brother)) readAndCache(brother, brother_index);
            readAndCache(brother, brother_index);
            if (brother.len > min_size) {
                // 借brother的最后一个元素
                for (int i = nextNode.len; i > 0; i--) {
                    nextNode.key[i] = nextNode.key[i - 1];
                }
                nextNode.len++;
                nextNode.key[0] = brother.key[brother.len - 1];
                brother.len--;
                objNode.key[pos - 1] = nextNode.key[0];
                dataType.updateData = nextNode.key[0];
                writeAndCache(nextNode);
                writeAndCache(brother);
                writeAndCache(objNode);
                dataType.need_merge = false;
                return dataType;
            } else {
                // 向前并块
                for (int i = brother.len, j = 0; i < brother.len + nextNode.len; i++, j++) {
                    brother.key[i] = nextNode.key[j];
                }
                brother.len += nextNode.len;
                brother.next_index = nextNode.next_index;
                nextNode.next_index = -1;
                objNode.len--;
                writeAndCache(brother);
                writeAndCache(objNode);
                //Cache.erase(nextNode.index_num);
                Reuse.push_back(nextNode.index_num);
                if (objNode.len >= min_size) {
                    dataType.need_merge = false;
                    return dataType;
                } else return dataType;
            }
        }
    }

    EraseType MergeIndex(int pos, node &objNode, node &nextNode, EraseType &indexType){
        int min_size = (max_size - 1) / 2;
        if (pos != objNode.len ) {
            int brother_index = objNode.sonPos[pos + 1];
            node brother;
            //if (!Cache.find(brother_index, brother)) readAndCache(brother, brother_index);
            readAndCache(brother, brother_index);
            if (brother.len > min_size) {
                // 将brother 的第一个块借给nextNode
                T min_index = findMin(brother);
                nextNode.key[nextNode.len] = min_index;
                nextNode.sonPos[nextNode.len + 1] = brother.sonPos[0];
                nextNode.len++;
                for (int i = 0; i < brother.len - 1; i++) {
                    brother.sonPos[i] = brother.sonPos[i + 1];
                    brother.key[i] = brother.key[i + 1];
                }
                brother.sonPos[brother.len - 1] = brother.sonPos[brother.len];
                brother.len--;
                objNode.key[pos] = findMin(brother);
                writeAndCache(brother);
                writeAndCache(objNode);
                writeAndCache(nextNode);
                indexType.need_merge = false;
                return indexType;
            } else {
                // 将brother Merge 到nextNode上
                nextNode.key[nextNode.len] = findMin(brother);
                nextNode.sonPos[nextNode.len + 1] = brother.sonPos[0];
                for (int i = nextNode.len + 1, j = 0; i < nextNode.len + brother.len + 1; i++, j++) {
                    nextNode.key[i] = brother.key[j];
                    nextNode.sonPos[i + 1] = brother.sonPos[j + 1];
                }
                nextNode.len += (brother.len + 1);
                for (int i = pos; i < objNode.len - 1; i++) {
                    objNode.key[i] = objNode.key[i + 1];
                    objNode.sonPos[i + 1] = objNode.sonPos[i + 2];
                }
                objNode.len--;
                Reuse.push_back(brother.index_num);
                //Cache.erase(brother.index_num);
                writeAndCache(objNode);
                writeAndCache(nextNode);
                if (objNode.len >= min_size) {
                    indexType.need_merge = false;
                    return indexType;
                } else return indexType;
            }
        } else {
            int brother_index = objNode.sonPos[pos - 1];
            node brother;
            //if (!Cache.find(brother_index, brother)) readAndCache(brother, brother_index);
            readAndCache(brother, brother_index);
            if (brother.len > min_size) {
                T min_num = findMin(nextNode);
                for (int i = nextNode.len; i > 0; i--) {
                    nextNode.key[i] = nextNode.key[i - 1];
                    nextNode.sonPos[i + 1] = nextNode.sonPos[i];
                }
                nextNode.sonPos[1] = nextNode.sonPos[0];
                nextNode.key[0] = min_num;
                nextNode.sonPos[0] = brother.sonPos[brother.len];
                objNode.key[pos - 1] = findMin(nextNode);
                indexType.updateData = objNode.key[pos - 1];
                brother.len--;
                nextNode.len++;
                writeAndCache(nextNode);
                writeAndCache(brother);
                writeAndCache(objNode);
                indexType.need_merge = false;
                return indexType;
            } else {
                // 向前并块
                T min_num = findMin(nextNode);
                brother.key[brother.len] = min_num;
                brother.sonPos[brother.len + 1] = nextNode.sonPos[0];
                for (int i = brother.len + 1,j = 0; i < brother.len + nextNode.len + 1; i++, j++) {
                    brother.key[i] = nextNode.key[j];
                    brother.sonPos[i + 1] = nextNode.sonPos[j + 1];
                }
                brother.len += (nextNode.len + 1);
                objNode.len--;
                //Cache.erase(nextNode.index_num);
                Reuse.push_back(nextNode.index_num);
                writeAndCache(brother);
                writeAndCache(objNode);
                if (objNode.len >= min_size) {
                    indexType.need_merge = false;
                    return indexType;
                } else return indexType;
            }
        }
    }

    EraseType dataErase(const T& obj, node &objNode) {
        int pos = binarySearch(objNode.key, objNode.len, obj);
        if (objNode.key[pos] != obj) {
            EraseType type(objNode.key[0], false, false);
            return type;
        }
        int min_size = max_size / 2;
        // pos是objNode.key[]数组中key的位置
        for (int i = pos; i < objNode.len - 1; i++) {
            objNode.key[i] = objNode.key[i + 1];
        }
        objNode.len--;
        if (objNode.len >= min_size) {
            writeAndCache(objNode);
            if (pos != 0) {
                EraseType type(objNode.key[0], false, false);
                return type;
            } else {
                EraseType type(objNode.key[0], false, true);
                return type;
            }
        }
        if (pos != 0) {
            EraseType type(objNode.key[0], true, false);
            return type;
        } else {
            EraseType type(objNode.key[0], true, true);
            return type;
        }
    }


public:
    explicit BPT(const string &filename, const string &space) {
        file.initialise(filename); // 创建文件
        file.clear();
        spaceFile.initialise(space);
        spaceFile.clear();
        int all;
        spaceFile.get_info(all, 1);
        if (all != 0) {
            for (int i = 0; i < all; i++) {
                int x;
                spaceFile.read(x, (i + 1) * sizeof(int));
                Reuse.push_back(x);
            }
        }
        file.get_info(total, 1);
        if (total != 0) file.read(root, sizeof(int) * info_len);
    }
    ~BPT() {
        // 把缓存中的元素写入文件当中
        // 先缓存，在弹出的时候再写入文件
        //Cache.clear_(file, info_len);
        spaceFile.write_info(Reuse.size(), 1);
        for (int i = 0; i < Reuse.size(); i++) {
            spaceFile.write(Reuse[i], (i + 1) * sizeof(int));
        }
        file.write_info(total, 1);
        file.write(root, info_len * sizeof(int));
        file.clear();
    }


    // 便于查找父节点，我们进行递归插入
    // true 代表成功插入
    bool insert(const T& x) {
        if (total == 0) {
            // 说明是开头
            if (Reuse.empty()) {
                root_num = total;
                total++;
            } else {
                root_num = Reuse[Reuse.size() - 1];
                Reuse.pop_back();
            }
            node root_(false, root_num);
            int leaf_num;
            if (Reuse.empty()) {
                leaf_num = total;
                total++;
            } else {
                leaf_num = Reuse[Reuse.size() - 1];
                Reuse.pop_back();
            }
            root_.sonPos[root_.len] = leaf_num;
            root = root_;
            node newLeaf(true, leaf_num);
            newLeaf.key[newLeaf.len] = x;
            newLeaf.len++;
            newLeaf.index_num = leaf_num;
            writeAndCache(root);
            writeAndCache(newLeaf);
            return true;
        }
        spiltNode spilt_node = insert(x, root);
        //if (spilt_node.exist) return false;
        if (spilt_node.need_spilt) {
            node newRoot;
            newRoot.isLeaf = false;
            if (Reuse.empty()) {
                newRoot.index_num = total;
                total++;
            } else {
                newRoot.index_num = Reuse.back();
                Reuse.pop_back();
            }
            newRoot.len = 1;
            newRoot.sonPos[0] = root.index_num;
            newRoot.sonPos[1] = spilt_node.objNode.index_num;
            T min_index = findMin(spilt_node.objNode);
            newRoot.key[0] = min_index;
            root = newRoot;
            writeAndCache(newRoot);
        }
        return true;
    }

    // 如果是最后一个就向前借块，否则向后借块，减少对索引值的更改
    void erase(const T& x) {
        int pos = binarySearch(root.key, root.len, x);
        int index;
        if (root.key[pos] == x && pos < root.len) {
            index = root.sonPos[++pos];
        } else index = root.sonPos[pos];
        node nextNode;
        //if (!Cache.find(index, nextNode)) readAndCache(nextNode, index);
        readAndCache(nextNode, index);
        EraseType type = erase(x, nextNode);
        if (!type.need_update && !type.need_merge) return;
        if (!type.need_merge && type.need_update) {
            if (pos != 0) root.key[pos - 1] = type.updateData;
            return;
        }
        if (type.need_merge && !type.need_update) {
            if (root.len != 0) {
                if (!nextNode.isLeaf) MergeIndex(pos, root, nextNode, type);
                else MergeData(pos, root, nextNode, type);
                return;
            } else {
                int son_index = root.sonPos[0];
                node newRoot;
                //if (!Cache.find(son_index, newRoot)) readAndCache(newRoot, son_index);
                readAndCache(newRoot, son_index);
                if (!newRoot.isLeaf) {
                    //Cache.erase(root.index_num);
                    Reuse.push_back(root.index_num);
                    root = newRoot;
                    return;
                } else return;
            }
        }
        if (type.need_update && type.need_merge) {
            if (pos != 0) root.key[pos - 1] = type.updateData;
            if (root.len != 0) {
                if (!nextNode.isLeaf) MergeIndex(pos, root, nextNode, type);
                else MergeData(pos, root, nextNode, type);
                return;
            } else {
                int son_index = root.sonPos[0];
                node newRoot;
                //if (!Cache.find(son_index, newRoot)) readAndCache(newRoot, son_index);
                readAndCache(newRoot, son_index);
                if (newRoot.isLeaf) return;
                //Cache.erase(root.index_num);
                Reuse.push_back(root.index_num);
                root = newRoot;
                return;
            }
        }
    }

    Yuki::vector<Value> find(const char key[]) {
        Yuki::vector<Value> all;
        int pos = keyBinarySearch(root.key, root.len, key);
        node nextNode;
        int index = root.sonPos[pos];
        //if (!Cache.find(index, nextNode)) readAndCache(nextNode, index);
        readAndCache(nextNode, index);
        while (!nextNode.isLeaf) {
            int pos_ = keyBinarySearch(nextNode.key, nextNode.len, key);
            int index_ = nextNode.sonPos[pos_];
            //if (!Cache.find(index_, nextNode)) readAndCache(nextNode, index_);
            readAndCache(nextNode, index_);
        }
        int pos_ = keyBinarySearch(nextNode.key, nextNode.len, key);
        if (pos_ == nextNode.len) {
            if (nextNode.next_index == -1) return all;
            //if (!Cache.find(nextNode.next_index, nextNode)) readAndCache(nextNode, nextNode.next_index);
            readAndCache(nextNode, nextNode.next_index);
            pos_ = keyBinarySearch(nextNode.key, nextNode.len, key);
            if (strcmp(nextNode.key[pos_].first, key) != 0) return all;
        } else if (strcmp(nextNode.key[pos_].first, key) != 0) return all;
        for (int i = pos_; i < nextNode.len; i++) {
            if (strcmp(nextNode.key[i].first, key) == 0) all.push_back(nextNode.key[i].second);
            else return all;
        }
        while (true) {
            if (nextNode.next_index == -1) return all;
            //if (!Cache.find(nextNode.next_index, nextNode)) readAndCache(nextNode, nextNode.next_index);
            readAndCache(nextNode, nextNode.next_index);
            for (int i = 0; i < nextNode.len; i++) {
                if (strcmp(nextNode.key[i].first, key) == 0) all.push_back(nextNode.key[i].second);
                else return all;
            }
        }
    }

    bool empty() {
        if (total == 0) return true;
        else return false;
    }

    bool findKV(const char Key[], Value &obj) {
        int pos = keyBinarySearch(root.key, root.len, Key);
        node nextNode;
        if (strcmp(root.key[pos].first, Key) == 0) pos++;
        int index = root.sonPos[pos];
        //if (!Cache.find(index, nextNode)) readAndCache(nextNode, index);
        readAndCache(nextNode, index);
        while (!nextNode.isLeaf) {
            int pos_ = keyBinarySearch(nextNode.key, nextNode.len, Key);
            if (strcmp(nextNode.key[pos_].first, Key) == 0) {
                pos_++;
            }
            int index_ = nextNode.sonPos[pos_];
            //if (!Cache.find(index_, nextNode)) readAndCache(nextNode, index_);
            readAndCache(nextNode, index_);
        }
        int pos_ = keyBinarySearch(nextNode.key, nextNode.len, Key);
        if (strcmp(nextNode.key[pos_].first, Key) != 0) return false;
        else {
            obj = nextNode.key[pos_].second;
            return true;
        }
    }

    void update(const T &obj) {
        int pos = binarySearch(root.key, root.len, obj);
        node nextNode;
        if (root.key[pos] == obj) pos++;
        int index = root.sonPos[pos];
        //if (!Cache.find(index, nextNode)) readAndCache(nextNode, index);
        readAndCache(nextNode, index);
        while (!nextNode.isLeaf) {
            int pos_ = binarySearch(nextNode.key, nextNode.len, obj);
            if (nextNode.key[pos_] == obj) pos_++;
            int index_ = nextNode.sonPos[pos_];
            //if (!Cache.find(index_, nextNode)) readAndCache(nextNode, index_);
            readAndCache(nextNode, index_);
        }
        int pos_ = binarySearch(nextNode.key, nextNode.len, obj);
        nextNode.key[pos_] = obj;
        writeAndCache(nextNode);
    }

};

#endif //TICKETSYSTEM_BPTREE_HPP
