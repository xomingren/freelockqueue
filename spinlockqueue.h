#pragma once
#include <atomic>
#include<mutex>
namespace spinlockqueue {
    const int ERROR_VOID_LIST = -9999;
    class spinlock {

    public:
        spinlock() : flag_(false)
        {}

        void lock()
        {
            bool expect = false;
            while (!flag_.compare_exchange_weak(expect, true))
            {
                expect = false;
            }
        }

        void unlock()
        {
            flag_.store(false);
        }

    private:
        std::atomic<bool> flag_;
    };
    template<class T>
    class spinlockqueue
    {
        struct node
        {
            T value_;
            node* next_;
            node(const T& x, node* next_ = nullptr)
                : value_(x)
                , next_(next_)
            {}
        };
    public:
        spinlockqueue() :head_(nullptr), tail_(nullptr), size_(0)
        {

        }
        ~spinlockqueue()
        {
            node* cur = head_;
            while (cur)
            {
                node* next_ = cur->next_;
                delete cur;
                cur = next_;
            }
        }
        void enqueue(const T& val)
        {
            locker_.lock();
            node* newnode = new node(val);
            if (tail_)
            {
                tail_->next_ = newnode;
                tail_ = tail_->next_;
            }
            else
            {
                tail_ = newnode;
                head_ = tail_;
            }
            ++size_;
            locker_.unlock();
        }
        T dequeue()
        {
            locker_.lock();
            if (head_ == nullptr)
            {
                locker_.unlock();
                return ERROR_VOID_LIST;
            }
            node* oldhead = head_;
            T ret = head_->value_;
            head_ = head_->next_;
            if (head_ == nullptr)
                tail_ = nullptr;
            delete oldhead; oldhead = nullptr;
            --size_;
            locker_.unlock();
            return ret;
        }
        size_t size()const
        {
            return size_;
        }
        bool empty()
        {
            return size_ == 0;
        }

    private:
        spinlockqueue(const spinlockqueue& others) = delete;
        spinlockqueue& operator=(const spinlockqueue& others) = delete;
        node* head_;
        node* tail_;
        //spinlock locker_;
        std::mutex locker_;
        size_t size_;
    };

}
