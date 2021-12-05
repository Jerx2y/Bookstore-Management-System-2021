#include <string>
#include <vector>
#include <cstring>
#include <sstream>
#include <fstream>

#include "blocklist.hpp"
#include "memoryriver.hpp"

using std::string;
using std::vector;

bool Node::operator<(const Node &rhs) const {
    return strcmp(first, rhs.first) < 0 || (!strcmp(first, rhs.first) && second < rhs.second);
    // TODO
}

bool Node::operator==(const Node &rhs) const {
    return !strcmp(first, rhs.first) && second == rhs.second;
}

void Block::merge(Block &obj) {
    Node res[kSize];
    int ipos = 0, jpos = 0, rpos = 0;
    while (ipos < size && jpos < obj.size)
        if (array_[ipos] < array_[jpos])
            res[rpos++] = array_[ipos++];
        else res[rpos++] = obj.array_[jpos++];
    while (ipos < size)
        res[rpos++] = array_[ipos++];
    while (jpos < obj.size)
        res[rpos++] = obj.array_[jpos++];
    for (int i = 0; i < rpos; ++i)
        array_[i] = res[i];
    size = rpos;
    maxvar = array_[size - 1];
}

Block Block::split() {
    Block res;
    res.size = size / 2;
    res.maxvar = maxvar;
    for (int i = size - 1, j = 0; j < res.size; --i, ++j)
        res.array_[j] = array_[i], array_[i] = Node();
    size -= res.size;
    maxvar = array_[size - 1];
    return res;
}

Block Block::add(const Node &var) {
    int targetpos = -1;
    for (int i = 0; i < size; ++i) {
        if (array_[i] < var) continue;
        targetpos = i;
        // std::cout << (var < array_[i]) << " " << (var.first < array_[i].first) << std::endl;
        // std::cout << "$"<< var.first << "$$" << array_[i].first <<"$"<< std::endl;
        break;
    }
    if (targetpos == -1)
        targetpos = size;
    size++;
    for (int i = size - 1; i > targetpos; --i)
        array_[i] = array_[i - 1];
    array_[targetpos] = var;
    maxvar = array_[size - 1];
    if (size > kLimit) return split();
    else return Block();
}

bool Block::dec(const Node &var) {
    int ipos = -1;
    for (int i = 0; i < size; ++i)
        if (array_[i] == var) {
            ipos = i;
            break;
        }
    if (ipos == -1) return false;
    size--;
    for (int i = ipos; i < size; ++i)
        array_[i] = array_[i + 1];
    array_[size] = Node();
    if (size) maxvar = array_[size - 1];
    else maxvar = Node();
    return true;
}

void BlockList::initialize(const string& filename) {
    blockindex_.initialise(filename + ".idx.bin");
    block_.initialise(filename + ".bin");
}

void BlockList::insert(const string &fir, const int &scd, const int &val) {
    Node var(fir, scd, val);
    BlockIndex index;
    blockindex_.read(index);
    // std::cout << " !!! " << index.maxvar[0].first << std::endl;
    int ipos = -1;
    index.find(var, ipos);
    // std::cout << " !@# " << index.maxvar[0].first << std::endl;
    Block curblock;
    if (ipos != -1) block_.read(curblock, index.getoffset(ipos));
    else ipos = 0, index.getoffset(ipos) = block_.write(curblock);
    Block extend = curblock.add(var);
    // curblock.print();
    block_.update(curblock, index.getoffset(ipos));
    // std::cout << " &&&&&& " << index.maxvar[0].first << std::endl;
    if (!extend.empty()) {
        int offset = block_.write(extend);
        index.extend(curblock.maxvar, extend.maxvar, offset, ipos + 1);
    }
    blockindex_.update(index, 0);
}

void BlockList::erase(const string &fir, const int &scd, const int &val) {
    Node var(fir, scd, val);
    BlockIndex index;
    blockindex_.read(index);
    int ipos;
    index.find(var, ipos);
    Block curblock;
    block_.read(curblock, index.getoffset(ipos));
    curblock.dec(var);
//    if (index.inrange(ipos + 1)) {
//        Block nxtblock;
//        block_.read(nxtblock, index.getoffset(ipos + 1));
//        if (curblock.size + nxtblock.size < ksLimit) {
//            block_.Delete(index.getoffset(ipos + 1));
//            curblock.merge(nxtblock);
//            index.shrink(curblock.maxvar, ipos + 1);
//        }
//    }
    blockindex_.update(index, 0);
    block_.update(curblock, index.getoffset(ipos));
}

void BlockList::query(const string &var, vector<int> &res) {
    res.clear();
    BlockIndex index;
    blockindex_.read(index);
    int lpos, rpos;
    index.query(var, lpos, rpos);
    // std::cout << lpos << " # " << rpos << std::endl;
    for (int i = lpos; i <= rpos; ++i) {
        Block curblock;
        block_.read(curblock, index.getoffset(i));
        for (int j = 0; j < curblock.size; ++j)
            if (var == curblock.array_[j].first)
                res.push_back(curblock.array_[j].offset_);
    }
}