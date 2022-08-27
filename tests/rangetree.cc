#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

class SegmentTree {
    vector<int> arr_;
    int size;
    vector<int> tree_;
    void buildTree(int node, int start, int end) {
        if (start == end) {
            tree_[node] = arr_[start];
        } else {
            int mid = start + ((end - start) >> 1);
            int leftNode = 2 * node + 1;
            int rightNode = 2 * node + 2;
            buildTree(leftNode, start, mid);
            buildTree(rightNode, mid + 1, end);
            tree_[node] = tree_[leftNode] + tree_[rightNode];
        }
    }

    int query(int node, int start, int end, int qstart, int qend) {
        if (start == qstart && end == qend) {
            return tree_[node];
        } else {
            int mid = start + ((end - start) >> 1);
            int leftNode = 2 * node + 1;
            int rightNode = 2 * node + 2;
            int lsum = 0, rsum = 0;
            if (qstart <= mid) {
                lsum = query(leftNode, start, mid, qstart, mid);
            }
            if (qend > mid) {
                rsum = query(rightNode, mid + 1, end, mid + 1, qend);
            }
            return lsum + rsum;
        }
    }

public:
    void debugPrint() {
        for (auto& node : tree_) {
            cout << node << ' ';
        }
        cout << '\n';
    }
    SegmentTree(vector<int>& v) : arr_(v), size(arr_.size()), tree_(2 * (static_cast<int>(log2(size)) + 1)) {
        buildTree(0, 0, size - 1);
    }
    void update(int node, int start, int end, int idx, int val) {
        if (start == end) {
            arr_[idx] = val;
            tree_[node] = val;
        } else {
            int mid = start + ((end - start) >> 1);
            int leftNode = 2 * node + 1;
            int rightNode = 2 * node + 2;
            if (idx >= start && idx <= leftNode) {
                update(leftNode, start, mid, idx, val);
            } else {
                update(rightNode, mid + 1, end, idx, val);
            }
            tree_[node] = tree_[leftNode] + tree_[rightNode];
        }
    }
    int query(int qstart, int qend) {
        return query(0, 0, size - 1, qstart, qend);
    }
};

int main() {
//
// [2, 1, 3, 9, 7, 4]
//
    vector<int> v{1, 2, 3, 4, 5, 6};
    SegmentTree st(v);
    st.debugPrint();
    cout << st.query(1, 4) << '\n';
}
