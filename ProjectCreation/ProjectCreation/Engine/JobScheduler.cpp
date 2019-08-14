#include "JobScheduler.h"

inline namespace JobScheduler
{
        void Initialize()
        {
#pragma push_macro("new");
#undef new
                new (&JobSchedulerInternal::WorkerThreads) AllocVector<std::thread, CACHE_LINE_SIZE>(NumWorkerThreads);
                new (&JobSchedulerInternal::JobQueues)
                    AllocVector<JobQueue<1024>>(NumWorkerThreads + 1); // main thread also has a job queue
                new (&JobSchedulerInternal::StaticJobAllocatorImpl) AllocVector<RingBuffer<Job, MaxJobs>>(NumWorkerThreads + 1);
                new (&JobSchedulerInternal::TempJobAllocatorImpl) AllocVector<RingBuffer<Job, MaxJobs>>(NumWorkerThreads + 1);
                TlsSetValue(TLSIndex_GenericIndex, (LPVOID)NumWorkerThreads);
                for (auto& itr : JobQueues)
                {
                        new (&itr) JobQueue<1024>();
                }
                for (auto& itr : StaticJobAllocatorImpl)
                {
                        new (&itr) RingBuffer<Job, MaxJobs>();
                }
                for (auto& itr : TempJobAllocatorImpl)
                {
                        new (&itr) RingBuffer<Job, MaxJobs>();
                }
                unsigned i = 0;
                for (auto& itr : WorkerThreads)
                {
                        new (&itr) std::thread(WorkerThreadMain, i);
                        i++;
                }
                auto test2 = TempJobAllocatorImpl[0].Allocate();
                auto test  = &StaticJobAllocatorImpl;
#pragma pop_macro("new");
        }

        void Shutdown()
        {
                RunWorkerThreads = false;
                for (auto& itr : WorkerThreads)
                {
                        itr.join();
                        itr.~thread();
                }
                for (auto& itr : TempJobAllocatorImpl)
                {
                        itr.~RingBuffer();
                }
                for (auto& itr : StaticJobAllocatorImpl)
                {
                        itr.~RingBuffer();
                }
                for (auto& itr : JobQueues)
                {
                        itr.~JobQueue();
                }
                JobQueues.Free();
                StaticJobAllocatorImpl.Free();
                TempJobAllocatorImpl.Free();
                WorkerThreads.Free();
                TlsFree(TLSIndex_GenericIndex);
        }

} // namespace JobScheduler