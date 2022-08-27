#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

namespace structalgo {

class SegmentTree {
    int size_;
    vector<int> tree_;
    void buildTree(vector<int>& arr, int node, int start, int end) {
        if (start == end) {
            tree_[node] = arr[start];
        } else {
            int mid = start + ((end - start) >> 1);
            int leftNode = 2 * node + 1;
            int rightNode = 2 * node + 2;
            buildTree(arr, leftNode, start, mid);
            buildTree(arr, rightNode, mid + 1, end);
            tree_[node] = tree_[leftNode] + tree_[rightNode];
        }
    }

    int query(int node, int start, int end, int qstart, int qend) {
        if (qstart > end || qend < start) {
            return 0;
        }
        if (start >= qstart && end <= qend) {
            return tree_[node];
        }
        int mid = start + ((end - start) >> 1);
        int leftNode = 2 * node + 1;
        int rightNode = 2 * node + 2;
        return query(leftNode, start, mid, qstart, qend) +
               query(rightNode, mid + 1, end, qstart, qend);
    }

    void update(int node, int start, int end, int idx, int val) {
        if (start == end) {
            tree_[node] = val;
        } else {
            int mid = start + ((end - start) >> 1);
            int leftNode = 2 * node + 1;
            int rightNode = 2 * node + 2;
            if (idx >= start && idx <= mid) {
                update(leftNode, start, mid, idx, val);
            } else {
                update(rightNode, mid + 1, end, idx, val);
            }
            tree_[node] = tree_[leftNode] + tree_[rightNode];
        }
    }

public:
    void debugPrint() {
        for (auto& node : tree_) {
            cout << node << ' ';
        }
        cout << '\n';
    }
    SegmentTree(vector<int>& v) : size_(v.size()), tree_(size_ * 4) {
        buildTree(v, 0, 0, size_ - 1);
    }
    void update(int idx, int val) {
        update(0, 0, size_ - 1, idx, val);
    }
    
    int query(int qstart, int qend) {
        return query(0, 0, size_ - 1, qstart, qend);
    }
};

}
