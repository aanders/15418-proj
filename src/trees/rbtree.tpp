/*
 * Implementation of rbtree interface
 */

#include "rbtree.h"

/*
 * lookup
 */
template <class T>
RBNode<T>* RBTree<T>::lookup(T val) {
  RBNode<T>* node = _root;
  
  if (!_root) {
    // Tree has no content
    return nullptr;
  }
  
  while (node->val != val) {
    if (val < node->val) {
      node = node->left;
    } else {
      node = node->right;
    }
    if (!node) {
      // We reached the end of the branch
      // and could not find the requested value
      return nullptr;
    }
  }
  
  // Success!  Return the node
  return node;
}

/*
 * insert
 */
template <class T>
T RBTree<T>::insert(T val) {
  if (!_root) {
    // This value becomes the root node
    _root = new RBNode<T>(val, NodeColor::BLACK);
    return val;
  } else {
    
    // Go as far as we can in the tree and then
    // insert the new value
    RBNode<T>* node = _root;
    RBNode<T>* newNode = nullptr;  // don't create the new node
                                // until we know we can insert it
    while (true) {
      if (val < node->val) {
        if (node->left) {
          node = node->left;
        } else {
          newNode = new RBNode<T>(val, NodeColor::RED);
          newNode->parent = node;
          node->left = newNode;
          _ibalance(newNode);
          return val;
        }
      } else if (val > node->val) {
        if (node->right) {
          node = node->right;
        } else {
          newNode = new RBNode<T>(val, NodeColor::RED);
          newNode->parent = node;
          node->right = newNode;
          _ibalance(newNode);
          return val;
        }
      } else {
        // Value is already in the tree.  For simplicity,
        // we do not allow duplicate elements
        // TODO: allow duplicate elements
        return nullptr;
      }
    }
  }
}

/*
 * _ibalance()
 * 
 * Protected helper to re-balance the tree after insertion
 */
template <class T>
void RBTree<T>::_ibalance(RBNode<T>* inserted) {
  if (inserted == _root) {
    // Easy peasy.  Color node black and we're done
    inserted->color = NodeColor::BLACK;
  } else if (inserted->parent->color == NodeColor::BLACK) {
    // Also easy.  No rebalancing needed in this case
    return;
  } else {
    // Okay, these are the tricky cases (our parent is red)
    
    // Since we're not the root, and our parent is not the root (it is
    // red, but the root is always black), we must have a grandparent
    // Get the grandparent node and the uncle node (other child of grandparent)
    RBNode<T>* gparent = inserted->parent->parent;
    RBNode<T>* uncle = (gparent->left == inserted->parent) ?
        gparent->right : gparent->left;
    // get the uncle node's color
    NodeColor uncleColor = (uncle) ? uncle->color : NodeColor::BLACK;
    
    // Now, if the uncle is red, we can color our parent and the uncle black
    // and color the grandparent red, then rebalance starting from the gparent
    if (uncleColor == NodeColor::RED) {
      uncle->color = NodeColor::BLACK;
      inserted->parent->color = NodeColor::BLACK;
      gparent->color = NodeColor::RED;
      _ibalance(gparent);
    } else {
      // OK our parent is red and its sibling (our uncle) is black.
      // First, if necessary, reorient the subtree rooted at our parent
      // to ensure that the two red nodes are along the same branch
      RBNode<T>* middleNode;
      bool isLeftBranch = true;
      if (gparent->left == inserted->parent) {
        if (inserted->parent->right == inserted) {
          // Reorient so inserted is in place of the parent
          // and the parent is the left subtree of inserted
          // Then both red nodes are along the left branch from gparent
          middleNode = _rotateLeft(inserted->parent);
        } else {
          // we're okay, no need to reorient
          middleNode = inserted->parent;
        }
      } else {
        isLeftBranch = false;  // 'cause this is the right branch
        if (inserted->parent->left == inserted) {
          // Reorient so inserted is in place of the parent
          // and the parent is the right subtree of inserted
          // Then both red nodes are along the right branch from gparent
          middleNode = _rotateRight(inserted->parent);
        } else {
          // we're okay, no need to reorient
          middleNode = inserted->parent;
        }
      }
      // Okay, now both red nodes are along either the left branch of
      // the grandparent or the right.  (The particular branch is indicated
      // by the flag isLeftBranch.)  We now reorient to put the middle
      // red node where the grandparent is now
      if (isLeftBranch) {
        _rotateRight(gparent);
      } else {
        _rotateLeft(gparent);
      }
      // Finally fix the colors.  middleNode has taken the place of
      // gparent, so it needs to be black.  gparent has moved between
      // middleNode and uncle (both black) so it needs to be red.
      middleNode->color = NodeColor::BLACK;
      gparent->color = NodeColor::RED;
    }
  }
}

/*
 * remove
 */
template <class T>
T RBTree<T>::remove(T val) {
  
  // Search the tree to find the node containing val
  RBNode<T>* node = lookup(val);
  if (!node) {
    // Tree does not contain the value to remove
    return 0;
  }
  
  // Remove the node from the tree
  if (!node->left) {
    // the node has only a right subtree
    // Prepare for removal
    _prepTree(node);
    // Connect the parent of this node directly to the subtree
    if (node->parent) {
      if (node->parent->left == node) {
        node->parent->left = node->right;
      } else {
        node->parent->right = node->right;
      }
    } else {
      // Node was the root of the tree
      _root = node->right;
    }
    if (node->right) node->right->parent = node->parent;
    delete node;
  } else if (!node->right) {
    // the node only has a left subtree
    // Prepare for removal
    _prepTree(node);
    // Connect the parent of this node directly to the subtree
    if (node->parent) {
      if (node->parent->left == node) {
        node->parent->left = node->left;
      } else {
        node->parent->right = node->left;
      }
    } else {
      // Node was the root of the tree
      _root = node->left;
    }
    if (node->left) node->left->parent = node->parent;
    delete node;
  } else {
    // The node has two subtrees
    // Get the greatest element in the left subtree
    RBNode<T>* r = node->left;
    while (r->right) {
      r = r->right;
    }
    
    // This element will replace the node that we are deleting,
    // and we'll delete the node with that element instead.  Note
    // that r has no right subtree (it is the greatest element) so
    // this is easy.
    node->val = r->val;
    
    // Prepare r for removal
    _prepTree(r);
    if (r->parent->right == r) {
      r->parent->right = r->left;
      if (r->left) r->left->parent = r->parent;
    } else {
      r->parent->left  = r->left;
      if (r->left) r->left->parent = r->parent;
    }
    delete r;
  }
  return val;
}

/*
 * _prepTree
 *
 * Protected helper to prepare the tree for removal of a node
 *
 * We only remove nodes with at most one subtree.  Depending on
 * the color of the node and the subtree, we may not have to do
 * anything to the tree, or we may have to do extensive modification.
 *
 */
template <class T>
void RBTree<T>::_prepTree(RBNode<T>* toRemove) {
  /*
   * Precondition:
   * toRemove != NULL
   *   -- AND --
   * At least one of toRemove->left and toRemove->right is NULL
   */
  if (toRemove->color == NodeColor::BLACK) {
    if (!toRemove->left) {
      // toRemove has only a right subtree
      if (toRemove->right && toRemove->right->color == NodeColor::RED) {
        toRemove->right->color = NodeColor::BLACK;
      } else {
        _rbalance(toRemove);
      }
    } else {
      // toRemove has only a left subtree
      if (toRemove->left && toRemove->left->color == NodeColor::RED) {
        toRemove->left->color = NodeColor::BLACK;
      } else {
        _rbalance(toRemove);
      }
    }
  } // else {
    //    toRemove->color == NodeColor::RED, don't need to do anything
    // }
}

/*
 * _rbalance
 *
 * Protected helper to rebalance the tree "after" removal
 *
 * Since we only remove nodes with at most one subtree, rebalancing
 * is only necessary if the node we removed is black and has no
 * subtrees.  (The other cases, enumerated below, are straightforward
 * and are implemented in _prepTree.
 *   o   The node we removed is red.  This introduces no violations
 *   o   The node we removed is black, and the root of its subtree is red
 *       We put the subtree root at the old node's location and color it black.
 *   o   The node we removed is black, and the root of its subtree is black.
 *       This is impossible, because paths through the subtree would have one
 *       more black node than the path to the NULL subtree of the current node
 * Thus, the only remaining case is if we removed a black node with two NULL
 * subtrees.)  However, this function calls itself recursively as we travel
 * up the tree, so we have no general information about the structure of the
 * tree which can be used here.
 *
 * The argument to this function is the address of the node which was
 * "removed".  The node is not actually removed from the tree until it is
 * rebalanced, so this is ok to do.
 */

template <class T>
void RBTree<T>::_rbalance(RBNode<T>* removed) {
  if (!removed) {
    // weird
    // std::cerr << "weird" << std::endl;
    return;
  } else if (!removed->parent) {
    // we removed the root, which is black, so the number of black
    // nodes along the path to each leaf has decreased by one
    // i.e. the tree properties are satisfied
    return;
  } else {
    // Calculate the sibling of the removed node
    bool rightSibling = (removed->parent->left == removed);
    RBNode<T>* sibling = (rightSibling) ?
        removed->parent->right : removed->parent->left;
    NodeColor siblingColor = (sibling) ? sibling->color : NodeColor::BLACK;
    
    // Incidentally, the rules of a red-black tree lead to the conclusion
    // that the sibling cannot be NULL.  If it is, there was a black
    // violation before the removal because the parent's subtree through
    // the removed node had black-height at least 2 while the parent's
    // subtree through the NULL sibling has black-height 1.
    if (!sibling) {
      //std::cerr << "BUG #1503: NULL sibling encountered (_rbalance)" << std::endl;
      //std::cerr << "   at " << removed->val << std::endl;
      return;
    }
    
    // Try to get the removed node's sibling to be black
    // If it's red, its children must be black.  Then we can rotate
    // the subtree rooted at the parent such that the sibling
    // becomes the new root.  Then one of the sibling's children
    // is the removed node's new sibling, which must be black.
    // To fix the colors in the rotation, note that the parent
    // is black if the sibling is red, so we can switch the parent
    // and sibling colors after the rotation
    // This doesn't actually fix the black violation, but puts the
    // tree in a form which will be dealt with later.
    if (siblingColor == NodeColor::RED) {
      if (rightSibling) {
        _rotateLeft(removed->parent);
      } else {
        _rotateRight(removed->parent);
      }
      sibling->color = NodeColor::BLACK;
      removed->parent->color = NodeColor::RED;
    }
    
    // If our parent, sibling, and sibling's children are all black,
    // we can just repaint the sibling red.  That makes the count of
    // black nodes through paths including the sibling decrease by one
    // to match the new count for paths which used to include the
    // removed node.  However, paths that don't go through the subtree
    // rooted at our parent are still one greater, so we recurse up
    
    // need to recompute sibling and color since it might have changed
    sibling = (rightSibling) ? removed->parent->right : removed->parent->left;
    siblingColor = (sibling) ? sibling->color : NodeColor::BLACK;
    // Also compute the sibling children's colors
    NodeColor siblingLColor = (sibling->left) ?
        sibling->left->color : NodeColor::BLACK;
    NodeColor siblingRColor = (sibling->right) ?
        sibling->right->color : NodeColor::BLACK;
    if ((removed->parent->color == NodeColor::BLACK) &&
        (siblingColor           == NodeColor::BLACK) &&
        (siblingLColor          == NodeColor::BLACK) &&
        (siblingRColor          == NodeColor::BLACK))   {
      sibling->color = NodeColor::RED;
      _rbalance(removed->parent);
    } else {
      // Okay at least one of those was red.  Maybe it was the parent.
      // If the parent is red, and the siblings children are both black,
      // e can make the sibling red and the parent black instead.  That
      // doesn't change the black count along paths through the sibling
      // but increases it by one on those through the removed node. :)
      if ((removed->parent->color == NodeColor::RED)   &&
          (siblingLColor          == NodeColor::BLACK) &&
          (siblingRColor          == NodeColor::BLACK))   {
        sibling->color = NodeColor::RED;
        removed->parent->color = NodeColor::BLACK;
        // and we're done!
        return;
      }
      
      // All right, running out of options.  At least one of the sibling's
      // children is red.  Let's make it the outer one
      // btw we better have a black sibling for this to work
      //if (sibling->color == NodeColor::RED) {
      //  std::cerr << "BUG #1591: sibling's color was RED in the wrong spot"
      //      << std::endl;
      //  return;
      //}
      if (rightSibling && siblingRColor == NodeColor::BLACK) {
        // Right branch, sibling's right child is black
        RBNode<T>* newRoot = _rotateRight(sibling);
        sibling->color = NodeColor::RED;
        newRoot->color = NodeColor::BLACK; // what used to be the sibling's
                                           // left child
        // removed's new sibling is now newRoot
        sibling = newRoot;
      } else if ((!rightSibling) && siblingLColor == NodeColor::BLACK) {
        // Left branch, sibling's left child is black
        RBNode<T>* newRoot = _rotateLeft(sibling);
        sibling->color = NodeColor::RED;
        newRoot->color = NodeColor::BLACK; // what used to be the sibling's
                                           // right child
        // removed's new sibling is now newRoot
        sibling = newRoot;
      }
      
      // OKAY this needs to stop.  We've got a parent of some color
      // and a black sibling with a red outer child.  So rotating to
      // put the sibling at the parent's location (copying the parent's
      // color) will push the parent down along the path where we removed
      // the node.  Then coloring the parent black increases the count
      // of black nodes along the path where we removed the node, evening
      // everything up and bringing a stop to this horrendousness.
      if (rightSibling) {
        _rotateLeft(removed->parent);
        sibling->right->color = NodeColor::BLACK;  // in case the parent is red
      } else {
        _rotateRight(removed->parent);
        sibling->left->color = NodeColor::BLACK;
      }
      sibling->color = removed->parent->color;
      removed->parent->color = NodeColor::BLACK;
    }
  }
}

/*
 * _rotateLeft
 *
 * Helper function to rotate the subtree rooted at
 * node to the left
 *
 * returns the new root of the subtree
 */
template <class T>
RBNode<T>* RBTree<T>::_rotateLeft(RBNode<T>* node) {
  if (!node || !node->right) {
    // Not possible to rotate
    return node;
  }
  RBNode<T>* newRoot = node->right;
  
  // First, change the root of the subtree to newRoot
  if (node->parent) {
    // Update the child of node's parent
    if (node->parent->left == node) {
      node->parent->left = newRoot;
    } else {
      node->parent->right = newRoot;
    }
    newRoot->parent = node->parent;
  } else {
    // node is the root of the entire tree
    newRoot->parent = nullptr;
    _root = newRoot;
  }
  // Now update the rest of the pointers
  node->parent = newRoot;
  node->right = newRoot->left;
  if (newRoot->left) newRoot->left->parent = node;
  newRoot->left = node;
  
  return newRoot;
}

/*
 * _rotateRight
 *
 * Helper function to rotate the subtree rooted at
 * node to the left
 *
 * returns the new root of the subtree
 */
template <class T>
RBNode<T>* RBTree<T>::_rotateRight(RBNode<T>* node) {
  if (!node || !node->left) {
    // Not possible to rotate
    return node;
  }
  RBNode<T>* newRoot = node->left;
  
  // First, change the root of the subtree to newRoot
  if (node->parent) {
    // Update the child of node's parent
    if (node->parent->left == node) {
      node->parent->left = newRoot;
    } else {
      node->parent->right = newRoot;
    }
    newRoot->parent = node->parent;
  } else {
    // node is the root of the entire tree
    newRoot->parent = nullptr;
    _root = newRoot;
  }
  // Now update the rest of the pointers
  node->parent = newRoot;
  node->left = newRoot->right;
  if (newRoot->right) newRoot->right->parent = node;
  newRoot->right = node;
  
  return newRoot;
}


/*
 * verify
 *
 * Make sure that this tree satisfies the red-black tree properties
 */
template <class T>
bool RBTree<T>::verify() {
  if (!_root) {
    // No tree, so it's vacuously true
    return true;
  } else if (_root->color == NodeColor::RED) {
    return false;
  } else {
    return (_verifyHelper(_root) > 0);
  }
}

/*
 * _verifyHelper
 *
 * Protected helper function for verify()
 * Verifies the subtree rooted at current.  The return value
 * is the black height of the subtree, or 0 if a violation
 * was found within this tree.  NULL nodes have a black
 * height of 1.
 */
template <class T>
int RBTree<T>::_verifyHelper(RBNode<T>* current) {
  if (!current) {
    // NULL nodes have a black height of 1
    return 1;
  } else {
    if (current->color == NodeColor::RED) {
      if (current->left && current->left->color == NodeColor::RED) {
        //std::cerr << "RED violation at " <<
        //    current->val << ": RED left child" << std::endl;
        return 0;
      } else if (current->right && current->right->color == NodeColor::RED) {
        //std::cerr << "RED violation at " <<
        //    current->val << ": RED right child" << std::endl;
        return 0;
      } 
    }
    // Okay no red violation.  Check for black violations
    int lBlackHeight = _verifyHelper(current->left);
    int rBlackHeight = _verifyHelper(current->right);
    if (lBlackHeight && rBlackHeight && lBlackHeight != rBlackHeight) {
      //std::cerr << "BLACK violation at " << current->val << std::endl;
      return 0;
    } else if (lBlackHeight == 0 || rBlackHeight == 0) {
      // A violation was found in one of the subtrees.  We already
      // printed the error message, so just propagate the result up
      return 0;
    } else {
      // The red-black properties are satisfied.  We just need to check if
      // the tree ordering properties are satisfied
      if (current->left && current->left->val > current->val) {
        //std::cerr << "Node ordering error at " << current->val <<
        //    "; left child " << current->left->val << " is greater" << std::endl;
        return 0;
      } else if (current->right && current->right->val < current->val) {
        //std::cerr << "Node ordering error at " << current->val <<
        //  "; right child " << current->right->val << " is greater" << std::endl;
        return 0;
      } else {
        // Verify tree integrity
        if (current->left && current->left->parent != current) {
          //std::cerr << "Improper parent pointer at " << current->left->val
          //    << std::endl;
          return 0;
        } else if (current->right && current->right->parent != current) {
          //std::cerr << "Improper parent pointer at " << current->right->val
          //    << std::endl;
          return 0;
        }
        
        // Everything is ok.
        return lBlackHeight + ((current->color == NodeColor::BLACK) ? 1 : 0);
      }
    }
  }
}

