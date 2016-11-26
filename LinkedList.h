#ifndef AISDI_LINEAR_LINKEDLIST_H
#define AISDI_LINEAR_LINKEDLIST_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace aisdi {

    template<typename Type>
    class LinkedList {
    public:
        using difference_type = std::ptrdiff_t;
        using size_type = std::size_t;
        using value_type = Type;
        using pointer = Type *;
        using reference = Type &;
        using const_pointer = const Type *;
        using const_reference = const Type &;

        class ConstIterator;

        class Iterator;

        using iterator = Iterator;
        using const_iterator = ConstIterator;

        class Node {

        public:
            Node *prev;
            Node *next;
            value_type val;

            Node() {
                prev = nullptr;
                next = nullptr;
                val = 0;
            }

            Node(const Type &item) {
                prev = nullptr;
                next = nullptr;
                val = item;
            }
        };

        Node *root;
        Node *last;
        size_type size;

        LinkedList() {
            this->root = nullptr;
            this->last = nullptr;
            this->size = 0;
        }

        LinkedList(std::initializer_list<Type> l) : LinkedList() {
            for (auto it = l.begin(); it != l.end(); ++it) {
                this->append(*it);
            }
        }

        void swapList(LinkedList<Type> &a, LinkedList<Type> &b) {
            std::swap(a.size, b.size);
            std::swap(a.root, b.root);
            std::swap(a.last, b.last);
        }

        LinkedList(const LinkedList &other) : LinkedList() {
            for (auto it = other.begin(); it != other.end(); ++it) {
                this->append(*it);
            }
        }

        LinkedList(LinkedList &&other) : LinkedList() {
            swapList(*this, other);
        }

        ~LinkedList() {
            if (this->root != nullptr) {
                this->erase(this->begin(), this->end());
            }

        }

        LinkedList &operator=(LinkedList other) {
            swapList(*this, other);
            return *this;
        }

        bool isEmpty() const {
            return ((int) this->size == 0);
        }

        size_type getSize() const {
            return this->size;
        }

        void append(const Type &item) {
            Node *new_node;

            new_node = new Node(item);
            new_node->next = nullptr;
            new_node->prev = this->last;

            if (this->root == nullptr) {
                this->root = new_node;
                this->last = new_node;
                ++size;
                return;
            }

            this->last->next = new_node;
            this->last = new_node;

            ++size;
        }

        void prepend(const Type &item) {
            insert(this->cbegin(), item);
        }

        void insert(const const_iterator &insertPosition, const Type &item) {
            Node *temp = new Node(item);

            Node *new_next = insertPosition.node;

            if (insertPosition.node == this->root) {
                temp->next = new_next;
                new_next->prev = temp;
                root = temp;

                ++size;
            } else if (insertPosition.node == nullptr) { this->append(item); }
            else {
                new_next->prev->next = temp;
                temp->prev = new_next->prev;
                temp->next = new_next;
                new_next->prev = temp;
                ++size;
            }

        }

        Type popFirst() {
            if (root == nullptr) throw std::out_of_range("");

            Node *popped = root;
            Type temp = popped->val;


            root = root->next;
            if (root != nullptr)
                root->prev = nullptr;

            delete popped;

            --size;
            return temp;
        }

        Type popLast() {
            Type temp = this->last->val;

            if (last == root) return popFirst();

            Node *node = this->last->prev;

            node->next = nullptr;

            delete this->last;

            this->last = node;

            --size;
            return temp;
        }

        void erase(const const_iterator &possition) {
            if (possition.node == nullptr) throw std::out_of_range("");

            if (possition.node == root) {
                popFirst();
                return;
            }

            if (possition.node == last) {
                popLast();
                return;
            }

            possition.node->prev->next = possition.node->next;
            possition.node->next->prev = possition.node->prev;

            delete possition.node;
            --size;
        }

        void erase(const const_iterator &firstIncluded, const const_iterator &lastExcluded) {
            if (firstIncluded == lastExcluded) return;

            Node *tempPrev = firstIncluded.node->prev;
            Node *tempNext;
            Node *temp = firstIncluded.node;

            while (temp != lastExcluded.node) {
                tempNext = temp->next;
                delete temp;
                temp = tempNext;
                --size;
            }

            if (temp != nullptr) {
                temp->prev = tempPrev;
            } else {
                this->last = tempPrev;
            }

            if (tempPrev != nullptr) {
                tempPrev->next = temp;
            } else {
                root = temp;
            }
        }

        iterator begin() {
            if ((int) this->size == 0) return end();
            else return Iterator(ConstIterator(this, this->root));
        }

        iterator end() {
            return Iterator(ConstIterator(this, nullptr));
        }

        const_iterator cbegin() const {
            return ConstIterator(this, root);
        }

        const_iterator cend() const {
            return ConstIterator(this, nullptr);
        }

        const_iterator begin() const {
            return cbegin();
        }

        const_iterator end() const {
            return cend();
        }
    };

    template<typename Type>
    class LinkedList<Type>::ConstIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename LinkedList::value_type;
        using difference_type = typename LinkedList::difference_type;
        using pointer = typename LinkedList::const_pointer;
        using reference = typename LinkedList::const_reference;

        const LinkedList *list;
        Node *node;

        explicit ConstIterator() {
            list = nullptr;
            node = nullptr;
        }

        ConstIterator(const LinkedList<value_type> *list, Node *node) {
            this->list = list;
            this->node = node;
        }

        reference operator*() const {
            return this->node->val;
        }

        ConstIterator &operator++() {
            if (this->node->next == nullptr) throw std::out_of_range("");

            this->node = this->node->next;
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator it(this->list, this->node);
            this->operator++();
            return it;
        }

        ConstIterator &operator--() {
            if (this->node == nullptr || this->node->prev == nullptr) throw std::out_of_range("");

            this->node = this->node->prev;
            return *this;
        }

        ConstIterator operator--(int) {
            ConstIterator it(this->list, this->node);
            this->operator--();
            return it;
        }

        ConstIterator operator+(difference_type d) const {
            Node *temp = this->node;

            for (int i = 0; i < d; ++i) {
                if (temp->next == nullptr) throw std::out_of_range("");

                temp = temp->next;
            }

            ConstIterator it(this->list, temp);
            return it;
        }

        ConstIterator operator-(difference_type d) const {
            Node *temp = this->node;

            for (int i = 0; i < -d; ++i) {
                if (temp->prev == nullptr) throw std::out_of_range("");

                temp = temp->prev;
            }

            ConstIterator it(this->list, temp);
            return it;
        }

        bool operator==(const ConstIterator &other) const {
            return (this->list == other.list && this->node == other.node);
        }

        bool operator!=(const ConstIterator &other) const {
            return !this->operator==(other);
        }
    };

    template<typename Type>
    class LinkedList<Type>::Iterator : public LinkedList<Type>::ConstIterator {
    public:
        using pointer = typename LinkedList::pointer;
        using reference = typename LinkedList::reference;

        explicit Iterator() {}

        Iterator(const ConstIterator &other)
                : ConstIterator(other) {}

        Iterator &operator++() {
            ConstIterator::operator++();
            return *this;
        }

        Iterator operator++(int) {
            auto result = *this;
            ConstIterator::operator++();
            return result;
        }

        Iterator &operator--() {
            ConstIterator::operator--();
            return *this;
        }

        Iterator operator--(int) {
            auto result = *this;
            ConstIterator::operator--();
            return result;
        }

        Iterator operator+(difference_type d) const {
            return ConstIterator::operator+(d);
        }

        Iterator operator-(difference_type d) const {
            return ConstIterator::operator-(d);
        }

        reference operator*() const {
            // ugly cast, yet reduces code duplication.
            return const_cast<reference>(ConstIterator::operator*());
        }
    };

}

#endif // AISDI_LINEAR_LINKEDLIST_H
