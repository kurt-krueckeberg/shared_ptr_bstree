#ifndef bstree_h
#define bstree_h
#include <memory>
#include <utility>
#include <iostream>
#include <algorithm>
#include <memory>
#include <queue>
#include <initializer_list>

/* 
 * See Unbalanced search trees at https://opendatastructures.org/ods-cpp/6_2_Unbalanced_Binary_Searc.html  
 * 
 * See implmented with shared_ptr<Node>  https://thesai.org/Downloads/Volume6No3/Paper_9-Implementation_of_Binary_Search_Trees_Via_Smart_Pointers.pdf 
 */
template<typename T> class bstree {

    class Node; // Forward reference.

    class NodeLevelOrderPrinter { // used by ::printlevelOrder()
   
      std::ostream& ostr;
      int current_level;
      int height_;
       
      std::ostream& (Node::*pmf)(std::ostream&) const noexcept;

      void display_level(std::ostream& ostr, int level) const noexcept
      {
        ostr << "\n" << "current level = " <<  level << '\n'; 
         
        // Provide some basic spacing to tree appearance.
        /*
        std::size_t num = height_ - level + 1;
      
        std::string str( num, ' ');
      
        ostr << str; 
         */ 
      }
      
      public: 
      
      NodeLevelOrderPrinter (const bstree<T>& tree, std::ostream& (Node::*pmf_)(std::ostream&) const noexcept, std::ostream& ostr_in):  ostr{ostr_in}, current_level{0}, pmf{pmf_}
      { 
          height_ = tree.height(); 
      }

      NodeLevelOrderPrinter (const NodeLevelOrderPrinter& lhs): ostr{lhs.ostr}, current_level{lhs.current_level}, height_{lhs.height_}, pmf{lhs.pmf} {}
      
      void operator ()(const Node *pnode, int level)
      { 
          // Did current_level change?
          if (current_level != level) { 
         
              current_level = level;
         
              display_level(ostr, level);       
          }
                    
          (pnode->*pmf)(std::cout);
         
          std::cout << '\n' << std::flush;
      }
   };

    struct Node {

        T key;
        Node *parent; // USed for tree traversal only

        std::shared_ptr<Node> left; 
        std::shared_ptr<Node> right;

        Node();

        Node(const T& x, Node *parent_in = nullptr) noexcept : key{x}, parent{parent_in} 
        {
        } 

        Node(const Node& lhs) noexcept = delete;

        Node& operator=(const Node& lhs) noexcept = delete;
         
        Node(Node&& lhs) noexcept = delete;

        Node& operator=(Node&& lhs) noexcept = delete;
        
        friend std::ostream& operator<<(std::ostream& ostr, const Node& node) 
        {
            return node.print(ostr);
        }

        std::ostream& print(std::ostream& ostr) const noexcept
        {
            return ostr << key << ", " << std::flush;
        }
        
        bool isLeaf() const noexcept
        {
           return (!left && !right) ? true : false;
        }

        std::ostream& debug_print(std::ostream& ostr) const noexcept;
   };

   // tree private members
   std::shared_ptr<Node> root; 
   std::size_t size;

   bool remove(const T& x, std::shared_ptr<Node>& p); 

   bool insert(const T& x, std::shared_ptr<Node>& p) noexcept;

   void move_tree(bstree&& lhs) noexcept
   {
       root = std::move(lhs.root);
       size = lhs.size;
       lhs.size = 0;
   }
   
   template<typename Functor> void in_order(Functor f, const std::shared_ptr<Node>& current) const noexcept; 

   template<typename Functor> void post_order(Functor f, const std::shared_ptr<Node>& current) const noexcept; 

   template<typename Functor> void pre_order(Functor f, const std::shared_ptr<Node>& current) const noexcept; 
 
   std::size_t height(const std::shared_ptr<Node>& node) const noexcept;

   void pre_order_copy(const std::shared_ptr<Node>& src, std::shared_ptr<Node>& dest) noexcept
   {
      if (!src) return;
         
      dest = src;
   
      pre_order_copy(src->left, dest->left);
      pre_order_copy(src->right, dest->right);
   }
   
 
  public:

    bstree() : root{nullptr}, size{0}
    {
    } 

   ~bstree() = default;

    bstree(const bstree& lhs) : size(lhs.size)
    {
       pre_order_copy(lhs.root, root);
    }

    bstree(const std::initializer_list<T>& list) noexcept : size{0}
    {
        for (const auto& x : list)
            insert(x);
    }

    bstree(bstree&& lhs)
    {
       move_tree(std::forward<bstree>(lhs));
    }

    //bstree& operator=(const bstree& lhs) = default; This may be correct, but for now...

    bstree& operator=(const bstree& lhs)
    { 
       if (this != &lhs)  {
          size = lhs.size;
          pre_order_copy(lhs.root, root);
       } 
       return *this;
    }

    bstree& operator=(bstree&& lhs)
    {
        move_tree(std::forward<bstree>(lhs));
    }

    void printlevelOrder(std::ostream& ostr) const noexcept;

    void debug_printlevelOrder(std::ostream& ostr) const noexcept;
    
    bool empty() const noexcept
    {
        return (size == 0) ? true : false;
    }
    
    std::size_t height() const noexcept
    {
       if (empty()) 
          return 0;
       else
          return height(root);
    }

    bool insert(const T& x) noexcept;
    
    bool remove(const T& x)
    {
       bool rc = remove(x, root); 
       if (rc) --size;
       return rc; 
    }

    template<typename Functor> void in_order(Functor f) const noexcept
    {
        return in_order(f, root);
    }
 
    template<typename Functor> void post_order(Functor f) const noexcept
    {
        return post_order(f, root);
    }
 
    template<typename Functor> void pre_order(Functor f) const noexcept
    {
        return pre_order(f, root);
    }
  
    template<typename Functor> void levelOrderTravers(Functor f) const noexcept;

    //void levelOrderTravers();

    size_t height();

    Node* find(const T&);
    
    std::ostream& print(std::ostream& ostr) const noexcept
    {
        std::cout << "tree::size = " << size << ". contents = { ";

        in_order([](const auto& x) { std::cout << x << ", " << std::flush; });
        
        std::cout << "} " << std::endl;
        return ostr;
    }
    
    friend std::ostream& operator<<(std::ostream& ostr, const bstree& tree)
    {
        return tree.print(ostr);
    }
};

template<class T> std::ostream& bstree<T>::Node::debug_print(std::ostream& ostr) const noexcept
{
   ostr << " { key = "; 
 
   ostr << key << ' ';

   if (parent) 
      ostr << ", parent->key = " << parent->key; 
   else
      ostr << ", parent = nullptr";
 
   if (!left) 
     ostr << ", left = nullptr";
   else
      ostr << ", left->key = " <<  left->key;
   
   if (!right) 
     ostr << ", right = nullptr";
   else
      ostr << ", right->key = " << right->key;
   
   ostr << "}";
 
   return ostr;
}

template<typename T> bool bstree<T>::insert(const T& x) noexcept
{
  if (!root) {
     root = std::make_shared<Node>(x);     
     ++size;
     return true;
  } 
  else {

     auto rc = insert(x, root);
     if (rc) ++size;
     return rc;
  }
}

/*
 insert
 ------

 insert is recursive. We current is initially never nullptr because the caller ensures this. Thereafter nullptr is checked before
 each recursive call. 
*/

template<typename T> bool bstree<T>::insert(const T& x, std::shared_ptr<Node>& current) noexcept
{
  if (x == current->key) return false; 

  else if (x < current->key) {

       if (!current->left) 
            current->left =  std::make_shared<Node>(x, current.get());
       else 
           insert(x, current->left);
   
  } else { // x > current->key

        if (!current->right)  
            current->right = std::make_shared<Node>(x, current.get());
        
        else
            insert(x, current->right);

  } 
  
  return true;
}

/*
remove
------

There are three cases to consider when removing a key and its node:

1. The node is a leaf
2. The has has only one child
3. The node has two children

Case #1 can be combined with case #2 (how is explained later). Case #2 has two subcases:

* The node only has a left child
* the node only has a right child

Both cases can be handled by splicing in the sole child node in place of the node to be removed.
We must alter parent of the spliced-in node, so it has the correct parent, namely, the parent of the node being removed.

Recursion is used to descend the tree searching for the key x to remove. Recursion is used again when an internal node holds the key.
An internal node is a node that has two non-nullptr children. It is "removed" by replacing its keys with that of its in-order
successor. Since this leaves a duplicate key in the in-order successor, we remove this duplicate key, we calling remove with the
successor key and the root of the right subtree of the node to be removed:
 
    remove(successor_key, right_subtree_root);
 
 Input Parameters:

 x - key/node to remove
 p - current node, initially the root of the tree.
*/
template<typename T> bool bstree<T>::remove(const T& x, std::shared_ptr<Node>& p) 
{
   // If we are not done, if p is not nullptr (which would mean the child of a leaf node), and p's key is
   // less than current key, recurse the left subtree looking for it.
   if (p && x < p->key) 
      return remove(x, p->left);

   // ...else if we are not done, again because p is not nullptr (which would mean the child of a leaf node), and p's key is
   // greater than current key, recurse the right subtree looking for it.
   else if (p && x > p->key)
      return remove(x, p->right);

   // ...else if p is not null, we compare it to the key.
   else if (p && p->key == x) { 

       auto y = p->parent;
    
       // 1. If p has no left child, we replace it with its right child (which may be nullptr, if p is a leaf node).
       if (!p->left) {

           // ...remove node p by replacing it with its right child (which may be nullptr), effectively splicing
           // in the right subtree, and reset its parent to be the parent of the just-deleted node.
           p = p->right; 
           p->parent = y;  

       // ...else if p has no right child and it does have a left child (since the first if-test failed)...
       } else if (!p->right) { 

            // ...remove node p by replacing it with its left child (which may be nullptr), effectively splicing in the 
            // left subtree, and reset its parent to be the parent of the just-deleted node.
            p = p->left; 
            p->parent = y;  // reset its parent to be the parent of the just-deleted node.
       
       // 2. Else p is an internal node with two non-nullptr children, and we replace its key with its in-order predecessor
       } else { 

         std::shared_ptr<Node> q = p->right; // <--- Note: This code is not possible with unique_ptr

         while (q->left != nullptr) // The in-order successor is the min value of p's right subtree.
                q = q->left;        

          p->key = q->key; // Set in-order q's key in p's node effectively removing the key.

          remove(q->key, p->right); // ...now delete duplicate key q->key from p's right subtree
                                    
       }
       return true;
   }
   // Could not find x in p or any of its children
   return false;
}

template<typename T>
template<typename Functor> void bstree<T>::in_order(Functor f, const std::shared_ptr<Node>& current) const noexcept 
{
   if (current == nullptr) {

      return;
   }

   in_order(f, current->left);

   f(current->key); 

   in_order(f, current->right);
}

template<typename T>
template<typename Functor> void bstree<T>::pre_order(Functor f, const std::shared_ptr<Node>& current) const noexcept 
{
   if (current == nullptr) {

      return;
   }

   f(current->key); 
   pre_order(f, current->left);
   pre_order(f, current->right);
}

template<typename T>
template<typename Functor> void bstree<T>::post_order(Functor f, const std::shared_ptr<Node>& current) const noexcept 
{
   if (current == nullptr) {

      return;
   }

   post_order(f, current->left);
   post_order(f, current->right);

   f(current->key); 
}

template<typename T> inline void  bstree<T>::printlevelOrder(std::ostream& ostr) const noexcept
{
  NodeLevelOrderPrinter tree_printer(*this, &Node::print, ostr);  
  
  levelOrderTravers(tree_printer);
  
  std::cout << std::endl;
}

template<typename T> void bstree<T>::debug_printlevelOrder(std::ostream& ostr) const noexcept
{
  NodeLevelOrderPrinter tree_printer(*this, &Node::debug_print, ostr);  
  
  levelOrderTravers(tree_printer);
  
  ostr << std::flush;
}

template<typename T> std::size_t bstree<T>::height(const std::shared_ptr<Node>& current) const noexcept
{
  // From: algorithmsandme.com/level-order-traversal-of-binary-tree
  if (!current) return 0;
 
  int lh = height(current->left);
  int rh = height(current->right);
 
  return 1 + std::max(lh, rh);
}

template<typename T> template<typename Functor> void bstree<T>::levelOrderTravers(Functor f) const noexcept
{
   std::queue< std::pair<const Node*, int> > queue; 

   const Node* proot = root.get();

   if (!proot) return;
      
   auto initial_level = 1; // initial, top root level is 1.
   
   // 1. pair.first  is: const bstree<T>::Node*, the current node to visit.
   // 2. pair.second is: current level of tree.
   queue.push(std::make_pair(proot, initial_level));

   /*
    * TODO: I think this code assumes a balanced tree.
    * We may need to use the tree height instead of isLeaf()
    */ 
   
   while (!queue.empty()) {

       /*
        std::pair<const Node *, int> pair_ = queue.front();
        const Node *current = pair_.first;
        int current_level = pair_.second;
       */

        auto[current, current_level] = queue.front(); 

        f(current, current_level);  

        if(current->left)
            queue.push(std::make_pair(current->left.get(), current_level + 1));  

        if(current->right)
            queue.push(std::make_pair(current->right.get(), current_level + 1));  

        queue.pop(); 
   }

}
#endif
