#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool checkEqualDepth(Node* node, int currentDepth, int& maxDepth) {
    // base case
    if (node == nullptr) return true;
    
    // if it's a leaf node
    if (node->left == nullptr && node->right == nullptr) {
        // if first leaf node, save as depth
        if (maxDepth == -1) {
            maxDepth = currentDepth;
        }
        // not first node, check if depth and maxDepth the same
        else {
            return (currentDepth == maxDepth);
        }
    }

    // not leaf node, continue
    bool leftSubtree = checkEqualDepth(node->left, currentDepth + 1, maxDepth);
    bool rightSubtree = checkEqualDepth(node->right, currentDepth + 1, maxDepth);
    // check if both are equal at end
    return (leftSubtree && rightSubtree);
}




bool equalPaths(Node * root)
{
    // true -- all paths from leaves to root are same length
    // parameter = root = pointer to root of tree to check for equal paths

    if (root->left == nullptr && root->right == nullptr) {
        // just root
        return true;
    }

    // check first leaf node depth, compare others to it
    // marker to see first node's depth later
    int maxDepth = -1;
    return checkEqualDepth(root, 0, maxDepth);

}

