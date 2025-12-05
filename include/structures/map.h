#ifndef MAP_HPP_
#define MAP_HPP_
#include <iostream>
#include <stdexcept>
#include <utility>
#include <algorithm>

template<typename Key, typename Value> class Map; // Forward declration 

template<typename Key, typename Value>
class MapNode{
public:
    std::pair<Key, Value> data;
    MapNode* left;
    MapNode* right;
    MapNode* parent;
    int height;

    MapNode(const std::pair<Key, Value>& data): data{data}, left{nullptr}, right{nullptr}, parent{nullptr}, height{}{ }
    MapNode(const std::pair<Key, Value>& data, MapNode* left, MapNode* right, int height = 0)
    :data{data}, left{left}, right{right}, parent{nullptr}, height{height}{ }
    MapNode(std::pair<Key, Value>&& data): data{std::move(data)}, left{nullptr}, right{nullptr}, parent{nullptr}, height{}{ }
    MapNode(std::pair<Key, Value>&& data, MapNode* left, MapNode* right, int height = 0)
    :data{std::move(data)}, left{left}, right{right}, parent{nullptr}, height{height}{ }

};

template<typename Key, typename Value>
class MapIterator{
private:
    MapNode<Key, Value>* node;

    // return the next node in a sorted order
    MapNode<Key, Value>* successor(MapNode<Key, Value>* nodePtr){
        if(nodePtr == nullptr) return nullptr;
        // If right subtree isn't empty
        // Go right then down to far left
        if(nodePtr->right != nullptr){
            nodePtr = nodePtr->right;
            while(nodePtr->left != nullptr) nodePtr = nodePtr->left; // Keep going down and to the left
            return nodePtr;
        }

        // If right subtreee is empty
        // Keep goind up until we approach from a left suptree
        MapNode<Key, Value>* p = nodePtr->parent;
        while(p != nullptr && nodePtr == p->right){ 
            nodePtr = p;
            p = nodePtr->parent;
        }
        // We either reached the top of the tree (no bigger value),
        // Or we approached the parent from the left
        return p;
    }

    MapNode<Key, Value>* predecessor(MapNode<Key, Value>* nodePtr){
        if(nodePtr == nullptr) return nullptr;

        // If left subtree isn't empty
        // Go left then down to far right
        if(nodePtr->left != nullptr){
            nodePtr = nodePtr->left;
            while(nodePtr->right != nullptr) nodePtr = nodePtr->right; // Keep going down and to the right
            return nodePtr;
        }    

        // If left subtreee is empty
        // Keep goind up until we approach from a right suptree
        MapNode<Key, Value>* p = nodePtr->parent;
        while(p != nullptr && nodePtr == p->left){ 
            nodePtr = p;
            p = nodePtr->parent;
        }
        // We either reached the top of the tree (no bigger value),
        // Or we approached the parent from the right
        return p;
    }
public:
    Key first;
    Value second;
    MapIterator(MapNode<Key, Value>* node): node{node} {
        if(node!= nullptr){
            first = node->data.first;
            second = node->data.second;
        }
    }

    // Dereference
    std::pair<Key, Value>& operator*() const{
        if(node == nullptr)
            throw std::out_of_range("Can't dereference iterator");
        return node->data;
    }
    std::pair<Key, Value>* operator->() const{
        if(node == nullptr)
            throw std::out_of_range("Can't dereference iterator");
        return &(node->data);
    }

    // ++ opeartor
    MapIterator<Key, Value>& operator++(){
        // Update the node pointer
        node = successor(node);
        return *this;
    }

    // -- operator
    MapIterator<Key, Value>& operator--(){
        // Update the node pointer
        node = predecessor(node);
        return *this;
    }

    bool operator==(const MapIterator<Key, Value>& rhs)const{
        return node == rhs.node;
    }
    
    bool operator!=(const MapIterator<Key, Value>& rhs)const{
        return node != rhs.node;
    }
    friend class Map<Key, Value>;
};

template<typename Key, typename Value>
class Map{
private:
    MapNode<Key, Value>* root;
    int s{}; // Size
    static const int ALLOWED_IMBALANCE = 1;

    int height(MapNode<Key, Value>* nodePtr){
        // Used to get the heights of the nodes
        return (nodePtr == nullptr?-1:nodePtr->height);    
    }

    void insert(const std::pair<Key, Value>& item, MapNode<Key, Value>*& nodePtr){
        if(nodePtr == nullptr) {// We reached a leaf, insert the node
            nodePtr = new MapNode{item};
            ++s;
        } 

        else if (item.first < nodePtr->data.first)// Move left
            insert(item, nodePtr->left);

        else if (item.first > nodePtr->data.first) // Move right 
            insert(item, nodePtr->right);

        else { // Duplicate was found
               // Update the value
            nodePtr->data.second = item.second;
        }; 
        // Update children's parent pointer
        updateParentPointer(nodePtr);
        // Rebalace the tree
        balance(nodePtr);
    }

    void updateParentPointer(MapNode<Key, Value>* nodePtr){
        if (nodePtr == nullptr) return;
        if(nodePtr->right != nullptr) nodePtr->right->parent = nodePtr;
        if(nodePtr->left != nullptr) nodePtr->left->parent = nodePtr;
    }

    void insert(std::pair<Key, Value>&& item, MapNode<Key, Value>*& nodePtr){
        if(nodePtr == nullptr) {// We reached a leaf, insert the node
            nodePtr = new MapNode{item};
            ++s;
        }      

        else if (item.first < nodePtr->data.first) // Move left
            insert(std::move(item), nodePtr->left);

        else if (item.first > nodePtr->data.first) // Move right 
            insert(std::move(item), nodePtr->right);

        else { // Duplicate was found
               // Update the value
            nodePtr->data.second = item.second;
        }; 
        // Update children's parent pointer
        updateParentPointer(nodePtr);
        // Rebalance the tree
        balance(nodePtr);
    }

    void balance(MapNode<Key, Value>*& nodePtr){
        if(nodePtr == nullptr) return;

        if(height(nodePtr->left) - height(nodePtr->right) > ALLOWED_IMBALANCE){
            // Left side causes imbalance
            if(height(nodePtr->left->left)>= height(nodePtr->left->right))
                // left side of the left child casuse imbalance, case 1
                rotateWithLeftChild(nodePtr); // Single rotation
            else
                // right side of the left child casuse imbalance, case 2
                doubleWithLeftChild(nodePtr); // Double rotation
        }
        else if(height(nodePtr->right) - height(nodePtr->left) > ALLOWED_IMBALANCE){
            // Right side causes imbalance
            if(height(nodePtr->right->right) >= height(nodePtr->right->left))
                // right side of the child casuse imbalance, case 4
                rotateWithRightChild(nodePtr);
            else 
                // left side of the right child casuse imbalance, case 3
                doubleWithRightChild(nodePtr);
        }
        // Update the height of the node
        nodePtr->height = std::max(height(nodePtr->left), height(nodePtr->right)) +1;
    }
    void rotateWithLeftChild(MapNode<Key, Value>*& parent){
        // Hold the child node with a pointer
        MapNode<Key, Value>* child = parent->left; 
        // Repoint the parent's left pointer with the child's right's child
        parent->left = child->right;
        // Repoint the child's right pointer to its parent, rotation is done
        child->right = parent;
        // Update grandchildren's parent pointers
        updateParentPointer(parent);
        // Update the heights of parent & child
        parent->height = std::max(height(parent->left), height(parent->right))+1;
        child->height = std::max(height(child->right), height(child->left))+1;

        // Repoint the passed-by-reference pointer to the new parent
        parent = child;
        // Update the parent of the new parent
        parent->parent = parent->right->parent;
        // Update parent pointer of the children of the new parent
        updateParentPointer(parent);

    }
    void doubleWithLeftChild(MapNode<Key, Value>*& nodePtr){
        rotateWithRightChild(nodePtr->left);
        rotateWithLeftChild(nodePtr);
    }
    void rotateWithRightChild(MapNode<Key, Value>*& parent){
        // Hold the child node with a pointer
        MapNode<Key, Value>* child = parent->right;
        // Repoint the the parent's left pointer to the right child of its child
        parent->right = child->left;
        // Repoint the child's left pointer to its parent, rotaion is done
        child->left = parent;
        // Update grandchildren's parent pointers
        updateParentPointer(parent);
        // Update the heights of parent & child
        parent->height = std::max(height(parent->left), height(parent->right)) + 1;
        child->height = std::max(height(child->left), height(child->right)) + 1;

        // Repoint the passed-by-reference pointer to the new parent
        parent = child; 
        // Update the parent of the new parent
        parent->parent = parent->left->parent;
        // Update parent pointer of the children of the new parent
        updateParentPointer(parent);
    }
    void doubleWithRightChild(MapNode<Key, Value>*& nodePtr){
        rotateWithLeftChild(nodePtr->right);
        rotateWithRightChild(nodePtr);
    }

    MapNode<Key, Value>* findMin(MapNode<Key, Value>* nodePtr) const{
        if(nodePtr == nullptr) return nullptr;
        if(nodePtr->left == nullptr) return nodePtr; // Reached a leaf, return its pointer
        else return findMin(nodePtr->left); // Keep moving down the tree
    }

    MapNode<Key, Value>* findMax(MapNode<Key, Value>* nodePtr) const{
        if(nodePtr == nullptr) return nullptr;
        if(nodePtr->right == nullptr) return nodePtr; // Reached a leaf, return its pointer
        else return findMax(nodePtr->right); // Keep moving down the tree
    }

    void clear(MapNode<Key, Value>* nodePtr){
        if(nodePtr == nullptr) return; // Base case
        
        clear(nodePtr->right); // Clear nodes on the right
        clear(nodePtr->left);  // Clear nodes on the left
        delete nodePtr; // Delete the current node
    }

    void erase(const Key& item,MapNode<Key, Value>*& nodePtr){
        if(nodePtr == nullptr) return; // Item not found, return

        if(item < nodePtr->data.first) // Move left
            erase(item, nodePtr->left);

        else if (item > nodePtr->data.first) // Move right
            erase (item, nodePtr->right);

        else if (nodePtr->left != nullptr && nodePtr->right != nullptr){ 
            // Wanted node has two children
            // Replace the wanted node with the Minumum node on its right
            nodePtr->data = findMin(nodePtr->right)->data;
            // Erase the reaplced node
            erase(nodePtr->data.first, nodePtr->right);
        }

        else{ // Single child or no children
            // Hold the wanted node with a pointer
            MapNode<Key, Value>* oldNode = nodePtr;
            // Repoint the parent's pointer to the child of the wanted node
            nodePtr = (nodePtr->right == nullptr)? nodePtr->left : nodePtr->right; 
            // Erase the wanted node
            delete oldNode;
            // Decrement the size
            --s;
        }
        // Update children parent pointers
        updateParentPointer(nodePtr);
        // Rebalance the tree
        balance(nodePtr);
    }

    void erase(Key&& item, MapNode<Key, Value>*& nodePtr){
        if(nodePtr == nullptr) return; // Item not found, return

        if(item < nodePtr->data.first) // Move left 
            erase(std::move(item), nodePtr->left);

        else if (item > nodePtr->data.first) // Move right
            erase (std::move(item), nodePtr->right);

        else if (nodePtr->left != nullptr && nodePtr->right != nullptr){ 
            // Wanted node has two children
            // Replace the wanted node with the Minumum node on its right
            nodePtr->data = findMin(nodePtr->right)->data;
            // Erase the reaplced node
            erase(nodePtr->data.first, nodePtr->right);
        }

        else{ // Single child or no children
            // Hold the wanted node with a pointer
            MapNode<Key, Value>* oldNode = nodePtr;
            // Repoint the parent's pointer to the child of the wanted node
            nodePtr = (nodePtr->right == nullptr)? nodePtr->left : nodePtr->right; 
            // Erase the wanted node
            delete oldNode;
            // Decrement the size
            --s;
        }
        // Update children parent pointers
        updateParentPointer(nodePtr);
        // Rebalance the tree
        balance(nodePtr);
    }

    bool count(const Key& item, MapNode<Key, Value>* nodePtr) const{
        if(nodePtr == nullptr) 
            return false; // Item not found, return false

        else if (item < nodePtr->data.first) // Move left
            return count(item, nodePtr->left); 

        else if (item > nodePtr->data.first) // Move right
            return count(item, nodePtr->right); 

        else return true; // Item found, return true
    }

    bool count(Key&& item, MapNode<Key, Value>* nodePtr)const {
        if(nodePtr == nullptr) return false; // Item not found, return false

        else if (item < nodePtr->data.first) // Move left
            return count(std::move(item), nodePtr->left); 

        else if (item > nodePtr->data.first) // Move right
            return count(std::move(item), nodePtr->right);

        else return true; // Item found, return true
    }
    void __clone__setParents(MapNode<Key, Value>* nodePtr){
        if(nodePtr == nullptr) return; // Base case
        __clone__setParents(nodePtr->left);
        __clone__setParents(nodePtr->right);
        updateParentPointer(nodePtr);
    }

    void print(MapNode<Key, Value>* nodePtr){
        if (nodePtr == nullptr) return; // Base case

        print(nodePtr->left); // Print left subtree
        std::cout << nodePtr->data.first << ':' << nodePtr->data.second << " "; // Print current node
        print(nodePtr->right); // Print right subtree
    }

    MapNode<Key, Value>* clone(MapNode<Key, Value>* rhsNodePtr){
        if(rhsNodePtr == nullptr) return nullptr; // Base case
        return new MapNode{rhsNodePtr->data, clone(rhsNodePtr->left), clone(rhsNodePtr->right)};
    }

    MapNode<Key, Value>* find(const Key& item, MapNode<Key, Value>* nodePtr) const{
        if(nodePtr == nullptr) return nullptr;
        if(item < nodePtr->data.first) // Move left
            return find(item, nodePtr->left);
        else if(item > nodePtr->data.first) // Move right
            return find(item, nodePtr->right);
        else // Found item, return its pointer
            return nodePtr;
    }


public:


    // Default constructor
    Map():root{nullptr}, s{}{ }

    // Copy constructor
    Map(const Map<Key, Value>& rhs):s{rhs.size()}{
        root = clone (rhs.root);
        __clone__setParents(root);
    }

    // Destructor
    ~Map(){
        clear(root);
    }

    int size() const{
        return s;
    }

    // Copy assignment operator
    void operator=(const Map<Key, Value>& rhs){
        clear(root); // Empty the tree
        root = clone(rhs.root);
        __clone__setParents(root);
        s = rhs.s;
    }

    MapIterator<Key, Value> begin() const{
        return MapIterator<Key, Value>(findMin(root));
    }
    MapIterator<Key, Value> end() const{
        return MapIterator<Key, Value>(nullptr);
    }

    void insert(const std::pair<Key, Value>& item){
        insert(item, root);
    }

    void insert(std::pair<Key, Value>&& item){
        insert(std::move(item), root);
    }

    [[nodiscard]]std::pair<Key, Value>& findMin() const{
        if(root == nullptr){
            throw std::out_of_range("Can't find minimum in an empty map");
        }
        return findMin(root)->data;
    }

    [[nodiscard]]std::pair<Key, Value>& findMax() const{
        if(root == nullptr){
            throw std::out_of_range("Can't find maximum in an empty map");
        }
        return findMax(root)->data;
    }

    std::pair<MapIterator<Key, Value>, bool> emplace(const Key& key, const Value& value){
        insert(std::make_pair(key, value), root);
        return std::make_pair(find(key), true);
    }
    

    void clear(){
        clear(root);
        root = nullptr;
        s = 0;
    }

    void erase(const Key& item){
        erase(item, root);
    }

    void erase(Key&& item){
        erase(std::move(item), root);
    }

    void erase(MapIterator<Key, Value> iterator){
        if(iterator.node == nullptr) return;
        erase(iterator->first);
    }

    [[nodiscard]]bool count(const Key& item) const{
        return count(item, root);
    }

    [[nodiscard]]bool count(Key&& item) const{
        return count(std::move(item), root);
    }

    [[nodiscard]]MapIterator<Key, Value> find(const Key& item) const{
        return MapIterator<Key, Value>{find(item,root)};
    }

    [[nodiscard]]bool empty() const{
        return root == nullptr;
    }

    void print(){
        if (root == nullptr) return; // Tree is empty
        print(root);
        std::cout << std::endl;
    }

    Value& operator[](const Key& item){
        MapNode<Key, Value>* temp = root;
        while(temp != nullptr){
            if(item < temp->data.first) // Move left
                temp = temp->left;
            else if (item > temp->data.first) // Move right
                temp = temp->right;
            else 
                // Key was found return a reference to its value
                return temp->data.second; 
        }
        // Key not found, insert it
        insert(std::make_pair(item, Value{}), root);

        // Research for the inserted node and return a reference to its value
        temp = root;
        while (temp != nullptr){
            if (item < temp->data.first) // Move left
                temp = temp->left; 
            else if (item > temp->data.first) // Move right
                temp = temp->right;
            else
                // Key was found return a frerence to its value
                return temp->data.second;
        }

        throw std::logic_error("Couldn't find the key after insertion");
    }
};



#endif