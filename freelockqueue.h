#pragma once
#include <atomic>
namespace freelockqueue {
    const int ERROR_VOID_LIST = -9999;

    template<class T>
    class freelockqueue
    {
        struct node
        {
            T value_;
            std::atomic<node*> next_;
            node(const T& x)
                : value_(x)
                , next_(nullptr)
            {}
        };

    public:
        freelockqueue()
        {
            tail_ = head_ = new node(T());
        }
        ~freelockqueue()
        {
            node* cur = head_.load();
            while (cur)
            {
                node* next_ = cur->next_;
                delete cur;
                cur = next_;
            }
        }

        void enqueue(const T& x)
        {
            node* newnode = new node(x);
            node* oldtail = nullptr;
            node* nullnode = nullptr;
            do
            {
                oldtail = tail_.load();
                nullnode = nullptr;//����ʧ��ʱnullnode������Ϊoldtail->next_������δ��
            } while (oldtail->next_.compare_exchange_weak(nullnode, newnode) != true);//����ɹ���tail��ʱλ�����ڶ����ڵ㣬�����߳������ᷢ��oldtail->next��Զ��Ϊ�գ��ᱻ��ס��ֱ�����߳�����tail����һλ
            //tail_.compare_exchange_weak(oldtail, newnode);
            tail_.store(newnode);
        }

        T dequeue()
        {
            //assert(!Empty());
            node* oldhead = nullptr;
            node* newhead = nullptr;
            T headvalue;
            do
            {
                oldhead = head_.load();
                newhead = oldhead->next_;
                if (newhead == nullptr)
                    return ERROR_VOID_LIST;
                headvalue = newhead->value_;
            } while (head_.compare_exchange_weak(oldhead, newhead) != true);
            //delete oldhead; oldhead = nullptr;       
            return headvalue;
        }

        bool empty()
        {
            return head_.load() == tail_.load();
        }
    private:
        freelockqueue(const freelockqueue<T>&) = delete;
        freelockqueue& operator=(const freelockqueue& others) = delete;
        std::atomic<node*> head_;
        std::atomic<node*> tail_;
    };
}