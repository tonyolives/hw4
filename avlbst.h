#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here ROTATES and InsertFix and RemoveFix
    void rotateLeft(AVLNode<Key, Value>* x);
    void rotateRight(AVLNode<Key, Value>* x);
    void insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void removeFix(AVLNode<Key, Value>* n, int8_t diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // get key and value
    const Key& key = new_item.first;
    const Value& value = new_item.second;

    // tree empty!
    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(key, value, nullptr);
        return;
    }

    // traverse tree like BST
    // static cast to convert base pointer to derived type since getleft, getright, etc alll return Node<Key,Value>*
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;

    while (curr != nullptr) {
        parent = curr;

        if (key < curr->getKey()) {
            curr = curr->getLeft();
        }
        else if (key > curr->getKey()) {
            curr = curr->getRight();
        }
        else {
            // key exists, overwrite value
            curr->setValue(value);
            return;
        }
    }

    // create new node n attach
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(key, value, parent);
    // key is less = LEFTT
    if (key < parent->getKey()) {
        parent->setLeft(newNode);
    }
    // key is more = RIGHT
    else {
        parent->setRight(newNode);
    }

    // update balance and check conditions
    if (parent->getBalance() == 1 || parent->getBalance() == -1) {
        parent->setBalance(0); // height unchanged, done
        return;
    }

    // parent balance was 0 before insertion — now update
    if (newNode == parent->getLeft()) {
        parent->setBalance(1);
    } else {
        parent->setBalance(-1);
    }

    // recurse upward to fix
    insertFix(parent, newNode);
}

// insert fix! (using class slides for AVL implementation)
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n)
{
    if (p == nullptr) return;
    AVLNode<Key, Value>* g = p->getParent();
    if (g == nullptr) return;

    // determine side of p relative to g
    if (p == g->getLeft()) {
        g->updateBalance(1); // inserted into left subtree

        if (g->getBalance() == 0) return;
        else if (g->getBalance() == 1) {
            insertFix(g, p); // keep propagating up
        } else if (g->getBalance() == 2) {
            // rebalancing needed
            if (n == p->getLeft()) {
                // zig-zig (left-left)
                rotateRight(g);
                p->setBalance(0);
                g->setBalance(0);
            } else {
                // zig-zag (left-right)
                rotateLeft(p);
                rotateRight(g);
                if (n->getBalance() == 1) {
                    p->setBalance(0); g->setBalance(-1);
                } else if (n->getBalance() == 0) {
                    p->setBalance(0); g->setBalance(0);
                } else {
                    p->setBalance(1); g->setBalance(0);
                }
                n->setBalance(0);
            }
        }
    }
    else {
        // symmetric case: p is right child
        g->updateBalance(-1); // inserted into right subtree

        if (g->getBalance() == 0) return;
        else if (g->getBalance() == -1) {
            insertFix(g, p); // keep propagating up
        } else if (g->getBalance() == -2) {
            // rebalancing needed
            if (n == p->getRight()) {
                // zig-zig (right-right)
                rotateLeft(g);
                p->setBalance(0);
                g->setBalance(0);
            } else {
                // zig-zag (right-left)
                rotateRight(p);
                rotateLeft(g);
                if (n->getBalance() == -1) {
                    p->setBalance(0); g->setBalance(1);
                } else if (n->getBalance() == 0) {
                    p->setBalance(0); g->setBalance(0);
                } else {
                    p->setBalance(-1); g->setBalance(0);
                }
                n->setBalance(0);
            }
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (node == nullptr) return;

    // step 1: if node has two children, swap with predecessor
    if (node->getLeft() && node->getRight()) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
        this->nodeSwap(node, pred);
    }

    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* child = nullptr;
    int8_t diff = 0;

    if (node->getLeft() != nullptr) {
        child = node->getLeft();
    } else if (node->getRight() != nullptr) {
        child = node->getRight();
    }

    // link child to parent
    if (parent != nullptr) {
        if (parent->getLeft() == node) {
            parent->setLeft(child);
            diff = +1;
        } else {
            parent->setRight(child);
            diff = -1;
        }
    } else {
        this->root_ = child;
    }

    if (child != nullptr) {
        child->setParent(parent);
    }

    delete node;

    // patch AVL balance
    removeFix(parent, diff);
}

// SEVENTH TIME REWRITING THIS PLS FKIN WORK PLEASEEE

template<typename Key, typename Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int8_t diff)
{
    if (n == nullptr) return;

    AVLNode<Key, Value>* p = n->getParent();
    int8_t nextDiff = 0;
    if (p != nullptr) {
        nextDiff = (n == p->getLeft()) ? -1 : 1;
    }

    // left subtree lost a node
    if (diff == +1)
    {
        if (n->getBalance() == -1)
        {
            n->setBalance(0);
            removeFix(p, nextDiff);
        }
        else if (n->getBalance() == 0)
        {
            n->setBalance(1);
            return;
        }
        else if (n->getBalance() == 1)
        {
            AVLNode<Key, Value>* c = n->getRight();
            if (c == nullptr) return;

            if (c->getBalance() == 0)
            {
                // height unchanged after rotation
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
                return;
            }
            else if (c->getBalance() == 1)
            {
                // right-right case
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, nextDiff);
            }
            else if (c->getBalance() == -1)
            {
                // right-left case
                AVLNode<Key, Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(n);

                int8_t b = g->getBalance();
                if (b == 1) {
                    n->setBalance(0); c->setBalance(-1);
                } else if (b == 0) {
                    n->setBalance(0); c->setBalance(0);
                } else {
                    n->setBalance(1); c->setBalance(0);
                }
                g->setBalance(0);
                removeFix(p, nextDiff);
            }
        }
    }

    // right subtree lost a node
    else if (diff == -1)
    {
        if (n->getBalance() == 1)
        {
            n->setBalance(0);
            removeFix(p, nextDiff);
        }
        else if (n->getBalance() == 0)
        {
            n->setBalance(-1);
            return;
        }
        else if (n->getBalance() == -1)
        {
            AVLNode<Key, Value>* c = n->getLeft();
            if (c == nullptr) return;

            if (c->getBalance() == 0)
            {
                // height unchanged after rotation
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
                return;
            }
            else if (c->getBalance() == -1)
            {
                // left-left case
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, nextDiff);
            }
            else if (c->getBalance() == 1)
            {
                // left-right case
                AVLNode<Key, Value>* g = c->getRight();
                rotateLeft(c);
                rotateRight(n);

                int8_t b = g->getBalance();
                if (b == 1) {
                    n->setBalance(-1); c->setBalance(0);
                } else if (b == 0) {
                    n->setBalance(0); c->setBalance(0);
                } else {
                    n->setBalance(0); c->setBalance(1);
                }
                g->setBalance(0);
                removeFix(p, nextDiff);
            }
        }
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

// HELPERS!

template<typename Key, typename Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = x->getRight();
    x->setRight(y->getLeft());
    if (y->getLeft() != nullptr) {
        y->getLeft()->setParent(x);
    }
    y->setParent(x->getParent());

    if (x->getParent() == nullptr) {
        this->root_ = y;
    } else if (x == x->getParent()->getLeft()) {
        x->getParent()->setLeft(y);
    } else {
        x->getParent()->setRight(y);
    }

    y->setLeft(x);
    x->setParent(y);
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = x->getLeft();
    x->setLeft(y->getRight());
    if (y->getRight() != nullptr) {
        y->getRight()->setParent(x);
    }
    y->setParent(x->getParent());

    if (x->getParent() == nullptr) {
        this->root_ = y;
    } else if (x == x->getParent()->getLeft()) {
        x->getParent()->setLeft(y);
    } else {
        x->getParent()->setRight(y);
    }

    y->setRight(x);
    x->setParent(y);
}


#endif
