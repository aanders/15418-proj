/*
 * Describes an interface for a red-black balanced binary tree.
 */

#ifndef _RBTREE_H
#define _RBTREE_H

enum class NodeColor { BLACK, RED };

template <class T>
class RBNode {
public:
  T val;
  NodeColor color;
  RBNode* left;
  RBNode* right;
  RBNode* parent;
  unsigned int size;
  char PADDING[24];
  
  // Constructor
  RBNode(T val, NodeColor color) : val(val), color(color),
      left(nullptr), right(nullptr), parent(nullptr), size(1) {}
};

template <class T>
class RBTree {
protected:
  RBNode<T>* _root;
  
  bool (*comp)(T a, T b);
  
  void _ibalance(RBNode<T>* inserted);
  void _prepTree(RBNode<T>* toRemove);
  void _rbalance(RBNode<T>* removed);
  RBNode<T>* _rotateLeft(RBNode<T>* node);
  RBNode<T>* _rotateRight(RBNode<T>* node);
  int _verifyHelper(RBNode<T>* current);
  void _traverseHelper(RBNode<T>* current);

public:

  /* Constructor
   * c returns true iff a < b
   */
  RBTree(bool (*c)(T a, T b)) : _root(nullptr), comp(c) {}

  /* Looks up a node by value in the tree.
   * Returns NULL if no node with the specified value is found.
   * Optional arg for_del decrements size fields on traversal
   * (used internally, probably no other uses for this) */
  RBNode<T>* lookup(T val, bool for_del = false);

  /* Looks up a node by index in the tree.
   * e.g. lookupByIdx(0) returns the smallest element
   * Returns NULL if index is out of bounds.
   * Optional arg for_del decrements size fields on traversal
   * (used internally, probably no other uses for this) */
  RBNode<T>* lookupByIdx(unsigned int i, bool for_del = false);
  
  /* Inserts a node with value val into the tree, keeping the tree
   * roughly balanced.
   */
  T insert(T val);
  
  /* Removes a node with value val into the tree, keeping the tree
   * roughly balanced.  The value removed is returned; or, if the
   * value did not exist in the tree, 0 is returned.
   */
  T remove(T val);
  
  /* Removes a node from the tree by index, keeping the tree
   * roughly balanced.  The value removed is returned; or, if the
   * value did not exist in the tree, 0 is returned.
   */
  T removeByIdx(unsigned int i);

  /* Removes a specific node from the tree by index, keeping the tree
   * roughly balanced.  The value removed is returned; or, if the
   * value did not exist in the tree, 0 is returned.
   * This function is intended primarily for internal use.
   */
  T removeNode(RBNode<T> *node);
  
  /* Verifies this tree to make sure that it satisfies all the
   * red-black tree properties as well as checking that it is
   * a binary search tree.
   */
  bool verify();

  /*
   * Perform an in-order traversal of the tree and print
   * each node's value, color, and size as <val><color><size>
   * where <val> and <size> are integers and <color> is R or B
   */
  void traverse();
  
};

#include "rbtree.tpp"

#endif /* _RBTREE_H */
