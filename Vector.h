#ifndef AISDI_LINEAR_VECTOR_H
#define AISDI_LINEAR_VECTOR_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace aisdi {

    template<typename Type>
    class Vector {
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

        size_type size;
        size_type capacity;
        pointer buffer;

        Vector() {
            this->size = 0;
            this->capacity = 2;
            this->buffer = new value_type[this->capacity];
        }

        Vector(std::initializer_list<Type> l) : Vector() {

            for (auto it = l.begin(); it != l.end(); ++it) {
                this->append(*it);
            }
        }

        Vector(const Vector &other) {
            this->capacity = other.capacity;
            this->size = 0;
            buffer = new value_type[other.capacity];
            for (int i = 0; i < (int) other.size; ++i) {
                this->append(other.buffer[i]);
            }
        }

        void swapVector(Vector<Type> &a, Vector<Type> &b) {
            std::swap(a.size, b.size);
            std::swap(a.capacity, b.capacity);
            std::swap(a.buffer, b.buffer);
        }

        Vector(Vector &&other) : Vector() {
            swapVector(*this, other);
        }

        ~Vector() {
            if (this->buffer != nullptr) {
                delete[] this->buffer;
            }

        }

        Vector &operator=(Vector other) {
            swapVector(*this, other);
            return *this;
        }

        bool isEmpty() const {
            return ((int) this->size == 0);
        }

        size_type getSize() const {
            return this->size;
        }

        void append(const Type &item) {
            if (this->size >= this->capacity) {
                this->reallocate();
            }

            this->buffer[(int) this->size] = item;

            ++(this->size);
        }

        void reallocate() {
            pointer new_buffer = new value_type[2 * (this->capacity)];
            for (int i = 0; i < (int) this->size; ++i) {
                *(new_buffer + i) = *(this->buffer + i);
            }

            this->capacity *= 2;
            delete[] this->buffer;
            this->buffer = new_buffer;
        }

        void prepend(const Type &item) {
            const ConstIterator it(this, 0);
            this->insert(it, item);
        }

        void insert(const const_iterator &insertPosition, const Type &item) {
            if ((int) (this->size) >= (int) this->capacity) {
                this->reallocate();
            }

            pointer new_buffer = new value_type[2 * (this->capacity)];

            for (int i = 0; i < insertPosition.index; ++i) {
                *(new_buffer + i) = *(this->buffer + i);
            }

            new_buffer[insertPosition.index] = item;

            for (int i = (int) this->size; i > insertPosition.index; --i) {
                *(new_buffer + i) = *(this->buffer + i - 1);
            }

            delete[] this->buffer;

            this->buffer = new_buffer;

            ++(this->size);
        }

        Type popFirst() {
            if ((int) this->size == 0) throw std::out_of_range("");

            Type temp = *this->buffer;

            pointer new_buffer = new value_type[this->capacity];

            for (int i = 0; i < (int) this->size - 1; ++i) {
                *(new_buffer + i) = *(this->buffer + i + 1);
            }

            delete[] this->buffer;

            this->buffer = new_buffer;

            --(this->size);

            return temp;
        }

        Type popLast() {
            if ((int) this->size == 0) throw std::out_of_range("");

            Type temp = *(this->buffer + size - 1);

            pointer new_buffer = new value_type[this->capacity];

            for (int i = 0; i < (int) this->size - 1; ++i) {
                *(new_buffer + i) = *(this->buffer + i);
            }

            delete[] this->buffer;

            this->buffer = new_buffer;

            --(this->size);

            return temp;
        }

        void erase(const const_iterator &possition) {
            if ((int) this->size <= possition.index) throw std::out_of_range("");

            pointer new_buffer = new value_type[this->capacity];

            for (int i = 0; i < possition.index; ++i) {
                *(new_buffer + i) = *(this->buffer + i);
            }

            for (int i = possition.index; i < (int) this->size - 1; ++i) {
                *(new_buffer + i) = *(this->buffer + i + 1);
            }

            delete[] this->buffer;

            this->buffer = new_buffer;

            --(this->size);
        }

        void erase(const const_iterator &firstIncluded, const const_iterator &lastExcluded) {
            if (lastExcluded.index <= firstIncluded.index) return;

            if (firstIncluded.index >= (int) this->size || (int) this->size == 0) throw std::out_of_range("");

            pointer new_buffer = new value_type[this->capacity];

            for (int i = 0; i < firstIncluded.index; ++i) {
                *(new_buffer + i) = *(this->buffer + i);
            }

            for (int i = lastExcluded.index; i < (int) this->size; ++i) {
                *(new_buffer + i - lastExcluded.index + firstIncluded.index) = *(this->buffer + i);
            }

            delete[] this->buffer;

            this->buffer = new_buffer;

            this->size -= (lastExcluded.index - firstIncluded.index);
        }

        iterator begin() {
            if ((int) this->size == 0) return end();
            else return Iterator(ConstIterator(this, 0));
        }

        iterator end() {
            return Iterator(ConstIterator(this, (int) this->size));
        }

        const_iterator cbegin() const {
            return ConstIterator(this, 0);
        }

        const_iterator cend() const {
            return ConstIterator(this, this->size);
        }

        const_iterator begin() const {
            return cbegin();
        }

        const_iterator end() const {
            return cend();
        }
    };

    template<typename Type>
    class Vector<Type>::ConstIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename Vector::value_type;
        using difference_type = typename Vector::difference_type;
        using pointer = typename Vector::const_pointer;
        using reference = typename Vector::const_reference;

        const Vector *vect;
        int index;

        explicit ConstIterator() {
            this->vect = nullptr;
            this->index = 0;
        }

        ConstIterator(const Vector<value_type> *vect, int index) {
            this->vect = vect;
            this->index = index;
        }

        reference operator*() const {
            if (this->vect == nullptr || this->index == (int) this->vect->size) throw std::out_of_range("");

            return this->vect->buffer[this->index];
        }

        ConstIterator &operator++() {
            if (this->index + 1 > (int) this->vect->size) {
                throw std::out_of_range("");
            }

            ++index;
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator it(this->vect, this->index);
            this->operator++();
            return it;
        }

        ConstIterator &operator--() {
            if (this->index - 1 < 0) {
                throw std::out_of_range("");
            }
            --index;
            return *this;
        }

        ConstIterator operator--(int) {
            ConstIterator it(this->vect, this->index);
            this->operator--();
            return it;
        }

        ConstIterator operator+(difference_type d) const {
            if (this->index + d > (int) this->vect->size || this->index + d < 0) {
                throw std::out_of_range("");
            }

            ConstIterator it(this->vect, this->index + d);
            return it;
        }

        ConstIterator operator-(difference_type d) const {
            return this->operator+(-d);
        }

        bool operator==(const ConstIterator &other) const {
            return (this->vect == other.vect && this->index == other.index);
        }

        bool operator!=(const ConstIterator &other) const {
            return !this->operator==(other);
        }
    };

    template<typename Type>
    class Vector<Type>::Iterator : public Vector<Type>::ConstIterator {
    public:
        using pointer = typename Vector::pointer;
        using reference = typename Vector::reference;

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

#endif // AISDI_LINEAR_VECTOR_H
