#pragma once

#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <iostream>
#include <algorithm>
#include <vector>

template <typename Type>
class SingleLinkedList {

    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {

        friend class SingleLinkedList;

        explicit BasicIterator(Node* node): node_(node) {
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_ = other.node_;
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            auto old_this = (*this);
            ++(*this);
            return old_this;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &(node_->value);
        }
    private:
        Node* node_ = nullptr;
    };

public:
    SingleLinkedList(): head_(Node())
    {
    }

    ~SingleLinkedList() {
        Clear();
    }

    template <typename Iter>
    SingleLinkedList(Iter begin, Iter end) {
        std::vector<Type> v(begin, end);
        for( auto it = v.rbegin(); it != v.rend(); ++it ) {
            PushFront(*it);
        }
    }
    
    SingleLinkedList(std::initializer_list<Type> values) {
        SingleLinkedList tmp(values.begin(), values.end());
        swap(tmp);
    }

    SingleLinkedList(const SingleLinkedList& other) {
        assert(size_ == 0 && head_.next_node == nullptr);
        try {
            SingleLinkedList tmp(other.begin(), other.end());
            swap(tmp);
        } catch (const std::bad_alloc&) {
            throw;
        }
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        assert(this->head_.next_node != rhs.head_.next_node);
        SingleLinkedList rhs_copy(rhs);
        swap(rhs_copy);
        return *this;
    }

    // Обменивает содержимое списков за время O(1)
    void swap(SingleLinkedList& other) noexcept {
        SingleLinkedList temp_list;
        temp_list.head_.next_node = other.head_.next_node;
        temp_list.size_ = other.size_;
        other.head_.next_node = head_.next_node;
        other.size_ = size_;
        head_.next_node = temp_list.head_.next_node;
        size_ = temp_list.size_;
        temp_list.head_.next_node = nullptr;
        temp_list.size_ = 0;
    }

    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Вставляет элемент value в начало списка за время O(1)
    void PushFront(const Type& value) {
        Node* front_node = new Node(value, head_.next_node);
        head_.next_node = front_node;
        ++size_;
    }

    // Очищает список за время O(N)
    void Clear() noexcept {
        while (head_.next_node != nullptr)
        {
            Node* to_delete = head_.next_node;
            head_.next_node = to_delete->next_node;
            delete to_delete;
            --size_;
        }
            
    }

    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept {
        if(IsEmpty()) {
            return end();
        }
        return Iterator{head_.next_node};
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator{nullptr};
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        if(IsEmpty()) {
            return end();
        }
        return cbegin();
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return cend();
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        if(IsEmpty()) {
            return end();
        }
        return ConstIterator{head_.next_node};
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator{nullptr};
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator{&head_};
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&head_) };
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&head_) };
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        ++size_;
        return Iterator{new_node};
    }

    void PopFront() noexcept {
        assert(size_ > 0);
        Node* new_first = head_.next_node->next_node;
        delete head_.next_node;
        head_.next_node = new_first;
        --size_;
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        auto new_node = pos.node_->next_node->next_node;
        delete pos.node_->next_node;
        pos.node_->next_node = new_node;
        return Iterator{new_node};
    }

private:
    Node head_;
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    SingleLinkedList<Type> tmp;
    tmp.swap(lhs);
    lhs.swap(rhs);
    rhs.swap(tmp);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return lhs == rhs || lhs < rhs;
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return lhs == rhs || lhs > rhs;
}