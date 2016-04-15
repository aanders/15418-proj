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
  int size_left;
  int size_right;
  
  // Constructor
  RBNode(T val, NodeColor color) : val(val), color(color), left(nullptr),
      right(nullptr), parent(nullptr), size_left(0), size_right(0) {}
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

public:

  /* Constructor
   * c returns true iff a < b
   */
  RBTree(bool (*c)(T a, T b));

  /* Looks up a node by value in the tree.
   * Returns NULL if no node with the specified value is found. */
  RBNode<T>* lookup(T val);

  /* Looks up a node by index in the tree, as if the tree
   * were storing a sorted array of elements and received a
   * request for the element at index i
   */
  RBNode<T>* lookupByIdx(int i);
  
  /* Inserts a node with value val into the tree, keeping the tree
   * roughly balanced.  Duplicate values are not allowed; so if the
   * value already exists in the tree, it is not inserted and the
   * function returns null.  Otherwise, it returns the value inserted.
   */
  T insert(T val);
  
  /* Removes a node with value val into the tree, keeping the tree
   * roughly balanced.  The value removed is returned; or, if the
   * value did not exist in the tree, 0 is returned.
   */
  T remove(T val);
  
  /* Verifies this tree to make sure that it satisfies all the
   * red-black tree properties as well as checking that it is
   * a binary search tree.
   */
  bool verify();
  
};

#include "rbtree.tpp"

#endif /* _RBTREE_H */