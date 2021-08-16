#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

#include "freelockqueue.h"
#include "spinlockqueue.h"
using namespace std;

//spinlockqueue::spinlockqueue<int> q;
freelockqueue::freelockqueue<int>q;

mutex mtx;
vector<int> vv;
const int singlethreadincrease = 1000000;
const int threadnum = 6;
size_t maxnum = singlethreadincrease * threadnum;

void consumer()
{
    while (true)
    {
        if (!q.empty())
        {
            int s = q.dequeue();
            if (s != spinlockqueue::ERROR_VOID_LIST)
            {
                mtx.lock();
                vv.push_back(s);
                if (vv.size() >= maxnum)
                {
                    mtx.unlock();
                    return;
                }
                mtx.unlock();
            }
        }
    }
}

void producer()
{
    while (true)
    {
        for (int i = 0; i < singlethreadincrease; ++i)
        {
            q.enqueue(i);
        }
    }
}

   

int main()
   {
       vector<std::thread>vp;
       vector<std::thread>vc;


       /// ////////////////////////////////////////////////////////////////////////////
       std::cout << "start " << threadnum << " workers_" << "every worker increase " << singlethreadincrease << std::endl;
       auto start = std::chrono::steady_clock::now();

       for (int i = 0; i < threadnum; ++i)
       {
           vp.push_back(thread(producer));
       }
       for (auto& w : vp)
       {
           w.detach();
       }

       for (int i = 0; i < threadnum; ++i)
       {
           vc.push_back(thread(consumer));
       }
       for (auto& c : vc)
       {
           c.join();
       }
       std::cout << "workers_ end" << std::endl;
       std::cout << "size of vector: " << vv.size() << std::endl;
       //std::cout << "nums of: " << singlethreadincrease - 1 << " is " <<std::count(vv.begin(), vv.end(), singlethreadincrease - 1) << std::endl;

       auto end = std::chrono::steady_clock::now();
       std::chrono::duration<double, std::micro> elapsed = end - start;
       cout << "total run time:" << (double)elapsed.count() / 1000000 << "s" << endl;
       /// ///////////////////////////////////////////////////////////////////////////////////////////////////////
       getchar();
   }