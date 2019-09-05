#pragma once

#define WIN_32_LEAN_AND_MEAN
#include <Windows.h>
#include <atomic>
#include <random>
#include <thread>
#include <tuple>
#undef GetJob

#include <BitwiseUtility.h>
#include <Range.h>

inline namespace JobSchedulerInternalUtility
{
        template <class C>
        struct ClosureTraits : public ClosureTraits<decltype(&C::operator())>
        {};
        template <class R, class C, class... Args>
        struct ClosureTraits<R (C::*)(Args... args)>
        {
                using SetableArgs = std::tuple<Args...>;
        };
        template <class R, class C, class... Args>
        struct ClosureTraits<R (C::*)(Args... args) const>
        {
                using SetableArgs = std::tuple<Args...>;
        };

        template <typename CopyConstructableType = void>
        inline void InPlaceForwardConstruct(void* mem)
        {}
        template <typename CopyConstructableType>
        inline void InPlaceForwardConstruct(void* mem, CopyConstructableType&& object)
        {
#pragma push_macro("new");
#undef new
                new (mem) CopyConstructableType(std::forward<CopyConstructableType>(object));
#pragma pop_macro("new");
        }
        template <typename CopyConstructableType, typename... CopyConstructableTypes>
        inline typename std::enable_if<sizeof...(CopyConstructableTypes)>::type
        InPlaceForwardConstruct(void* mem, CopyConstructableType&& object, CopyConstructableTypes&&... objects)
        {
#pragma push_macro("new");
#undef new
                new (mem) CopyConstructableType(std::forward<CopyConstructableType>(object));
#pragma pop_macro("new");
                char* memAlias = static_cast<char*>(mem);
                InPlaceForwardConstruct(memAlias + sizeof(CopyConstructableType),
                                        std::forward<CopyConstructableTypes>(objects)...);
        }
} // namespace JobSchedulerInternalUtility

// The JobSchedulerInternals are a modified implementation of the lock free JobQueue specified by Stefan Reinalter on
// his blog, molecular-matters. It should be noted that code has been modified/added as seen fit.
//
// https://blog.molecular-matters.com/2015/09/25/job-system-2-0-lock-free-work-stealing-part-3-going-lock-free/
//
// The MIT License(MIT)
//
// Copyright(c) 2012-2017 Stefan Reinalter
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
inline namespace JobScheduler
{
        struct JobInternal;
}
inline namespace JobSchedulerGlobals
{
        inline DWORD                     g_tls_access_value = TlsAlloc();
        inline unsigned                  g_num_threads      = std::thread::hardware_concurrency();
        inline std::vector<std::thread>  g_worker_threads;
        inline volatile bool             g_worker_thread_active = true;
        inline thread_local std::mt19937 g_thread_local_rng_engine;

        inline constexpr unsigned CACHE_LINE_SIZE    = std::hardware_destructive_interference_size;
        inline constexpr unsigned MAX_JOBS_PER_FRAME = 8192U;
} // namespace JobSchedulerGlobals
inline namespace JobSchedulerUtility
{
        inline unsigned GenerateRandomNumber(unsigned min, unsigned max)
        {
                return (g_thread_local_rng_engine() % (max - min)) + min;
        }
} // namespace JobSchedulerUtility
inline namespace JobScheduler
{
        using JobFunction = void (*)(JobInternal*);
        struct alignas(64) JobInternal
        {
                JobFunction           function;
                JobInternal*          parent;
                volatile long         unfinished_jobs;
                static constexpr auto PADDING_SIZE =
                    CACHE_LINE_SIZE - sizeof(function) - sizeof(parent) - sizeof(unfinished_jobs);
                char padding[PADDING_SIZE];
        };
        template <unsigned MAX_JOBS>
        struct JobAllocator
        {
                static constexpr uint64_t MAX_JOBS = nextPowerOf2(MAX_JOBS);
                static constexpr uint64_t MASK     = MAX_JOBS - 1;
                alignas(8) JobInternal* job_buffer = 0;
                alignas(8) uint64_t allocated_jobs = 0;
                JobAllocator()
                {}
                void Initialize()
                {
                        job_buffer = (JobInternal*)_aligned_malloc(sizeof(JobInternal) * MAX_JOBS, 64);
                }
                JobInternal* Allocate()
                {
                        return job_buffer + (allocated_jobs++ & MASK);
                }
                void Release()
                {
                        _aligned_free(job_buffer);
                }
                ~JobAllocator()
                {}
        };

        template <unsigned MAX_JOBS>
        struct JobQueue
        {
                static constexpr auto MAX_JOBS = nextPowerOf2(MAX_JOBS);
                static constexpr auto MASK     = MAX_JOBS - 1;
                JobInternal**         m_jobs;
				volatile int64_t m_bottom;
                volatile int64_t m_top;
                JobQueue()
                {}
                void Initialize() volatile
                {
                        m_jobs   = (JobInternal**)_aligned_malloc(sizeof(JobInternal*) * MAX_JOBS, 8);
                        m_bottom = 0;
                        m_top    = 0;
                }
                void Shutdown() volatile
                {
                        _aligned_free(m_jobs);
                }
                JobQueue(JobQueue&& other) noexcept
                {
                        m_bottom     = other.m_bottom;
                        m_top        = other.m_top;
                        m_jobs       = other.m_jobs;
                        other.m_jobs = 0;
                }
                ~JobQueue()
                {}
                void Push(JobInternal* job) volatile
                {
                        int64_t b        = m_bottom;
                        m_jobs[b & MASK] = job;

                        // ensure the job is written before b+1 is published to other threads.
                        // on x86/64, a compiler barrier is enough.
                        std::atomic_thread_fence(std::memory_order_seq_cst);

                        m_bottom = b + 1;
                }
                JobInternal* Pop(void) volatile
                {
                        int64_t b = m_bottom - 1;
                        InterlockedExchange64(&m_bottom, b);

                        int64_t t = m_top;
                        if (t <= b)
                        {
                                // non-empty queue
                                JobInternal* job = m_jobs[b & MASK];
                                if (t != b)
                                {
                                        // there's still more than one item left in the queue
                                        return job;
                                }

                                // this is the last item in the queue
                                if (InterlockedCompareExchange64(&m_top, t + 1, t) != t)
                                {
                                        // failed race against steal operation
                                        job = nullptr;
                                }

                                m_bottom = t + 1;
                                return job;
                        }
                        else
                        {
                                // deque was already empty
                                m_bottom = t;
                                return nullptr;
                        }
                }
                JobInternal* Steal(void) volatile
                {
                        int64_t t = m_top;

                        // ensure that top is always read before bottom.
                        // loads will not be reordered with other loads on x86, so a compiler barrier is enough.
                        std::atomic_thread_fence(std::memory_order_seq_cst);

                        int64_t b = m_bottom;
                        if (t < b)
                        {
                                // non-empty queue
                                JobInternal* job = m_jobs[t & MASK];

                                // the interlocked function serves as a compiler barrier, and guarantees that the read happens
                                // before the CAS.
                                if (InterlockedCompareExchange64(&m_top, t + 1, t) != t)
                                {
                                        // a concurrent steal or pop operation removed an element from the deque in the
                                        // meantime.
                                        return nullptr;
                                }

                                return job;
                        }
                        else
                        {
                                // empty queue
                                return nullptr;
                        }
                }
        };
} // namespace JobScheduler
inline namespace JobSchedulerGlobals
{
        inline thread_local JobAllocator<MAX_JOBS_PER_FRAME> g_thread_local_job_allocator_temp;
        inline thread_local JobAllocator<MAX_JOBS_PER_FRAME> g_thread_local_job_allocator_static;

        alignas(8) volatile inline JobQueue<MAX_JOBS_PER_FRAME>* volatile g_job_queues;
} // namespace JobSchedulerGlobals
inline namespace JobSchedulerInternal
{
        inline auto& GetWorkerThreadQueue()
        {
                return g_job_queues[reinterpret_cast<unsigned long long>(TlsGetValue(g_tls_access_value))];
        }
        inline void Run(JobInternal* job)
        {
                auto& queue = GetWorkerThreadQueue();
                queue.Push(job);
        }
        inline JobInternal* AllocateJob()
        {
                return g_thread_local_job_allocator_temp.Allocate();
        }
        inline JobInternal* CreateJob(JobFunction function)
        {
                JobInternal* job     = AllocateJob();
                job->function        = function;
                job->parent          = nullptr;
                job->unfinished_jobs = 1;
                return job;
        }
        inline JobInternal* CreateJobAsChild(JobInternal* parent, JobFunction function)
        {
                InterlockedIncrement(&parent->unfinished_jobs);
                JobInternal* job     = AllocateJob();
                job->function        = function;
                job->parent          = parent;
                job->unfinished_jobs = 1;
                return job;
        }
        inline bool HasJobCompleted(JobInternal* job)
        {
                return job->unfinished_jobs == 0;
        }
        inline void Finish(JobInternal* job)
        {
                const long unfinished_jobs = InterlockedDecrement(&job->unfinished_jobs);
                if (unfinished_jobs == 0 && job->parent)
                {
                        Finish(job->parent);
                }
        }
        inline void Execute(JobInternal* job)
        {
                job->function(job);
                Finish(job);
        }
        inline JobInternal* GetJob()
        {
                auto&        queue = GetWorkerThreadQueue();
                JobInternal* job   = queue.Pop();
                if (!job)
                {
                        // this is not a valid job because our own queue is empty, so try stealing from some other queue
                        unsigned int randomIndex = GenerateRandomNumber(0, g_num_threads);
                        auto&        stealQueue  = g_job_queues[randomIndex];
                        if (&stealQueue == &queue)
                        {
                                // don't try to steal from ourselves
                                Sleep(0);
                                return nullptr;
                        }

                        JobInternal* stolenJob = stealQueue.Steal();
                        if (!stolenJob)
                        {
                                // we couldn't steal a job from the other queue either, so we just yield our time slice
                                // for now
                                Sleep(0);
                                return nullptr;
                        }

                        return stolenJob;
                }
                return job;
        }
        inline void Wait(JobInternal* job)
        {
                while (!HasJobCompleted(job))
                {
                        JobInternal* nextJob = GetJob();
                        if (nextJob)
                        {
                                Execute(nextJob);
                        }
                        // std::atomic_thread_fence(std::memory_order_seq_cst);
                }
        }
        inline void WorkerThreadMain(uint64_t index)
        {
                TlsSetValue(g_tls_access_value, (LPVOID)index);
                while (g_worker_thread_active)
                {
                        JobInternal* job = GetJob();
                        if (job)
                        {
                                Execute(job);
                        }
                }
        }
} // namespace JobSchedulerInternal
inline namespace JobScheduler
{
        inline void Initialize()
        {
                g_thread_local_job_allocator_static.Initialize();
                g_thread_local_job_allocator_temp.Initialize();
                unsigned long long thread_index      = 0;
                auto               g_job_queues_size = sizeof(JobQueue<MAX_JOBS_PER_FRAME>) * g_num_threads;
                g_job_queues = (volatile JobQueue<MAX_JOBS_PER_FRAME>*)_aligned_malloc(g_job_queues_size,
                                                                                       alignof(JobQueue<MAX_JOBS_PER_FRAME>));
                assert(g_job_queues);
                for (volatile JobQueue<MAX_JOBS_PER_FRAME>* itr = g_job_queues; itr != &g_job_queues[g_num_threads]; ++itr)
                {
                        itr->Initialize();
                        thread_index++;
                }

                thread_index = 0;
                TlsSetValue(g_tls_access_value, reinterpret_cast<LPVOID>(thread_index));
                thread_index++;
                for (; thread_index < g_num_threads; thread_index++)
                        g_worker_threads.push_back(std::thread(WorkerThreadMain, thread_index));
        }
        inline void Shutdown()
        {
                g_thread_local_job_allocator_static.Release();
                g_thread_local_job_allocator_temp.Release();
                g_worker_thread_active = false;
                for (auto& itr : g_worker_threads)
                        itr.join();
                // for (auto& itr : g_worker_threads)
                //        itr.~thread();
                for (unsigned i = 0; i < g_num_threads; i++)
                        g_job_queues[i].Shutdown();
                _aligned_free(const_cast<JobQueue<MAX_JOBS_PER_FRAME>*>(g_job_queues));
                TlsFree(g_tls_access_value);
        }
} // namespace JobScheduler

void function(int x, float y, char b);

inline namespace JobSchedulerAbstractionsInternal
{
        template <typename Allocator = TempJobAllocator, typename CallableType, typename... Args>
        inline JobInternal* CreateJobData(CallableType&& callable, Args&&... args)
        {
                JobInternal* thisJob = Allocator::Allocate();

                static_assert(sizeof(std::tuple<Args&&...>) + sizeof(CallableType &&) <= JobInternal::PADDING_SIZE,
                              "lambda is too large to fit the Job's padding buffer");
                char* bufferAlias = thisJob->padding;
                InPlaceForwardConstruct(bufferAlias, std::forward<CallableType>(callable));
                InPlaceForwardConstruct(bufferAlias + sizeof(CallableType), std::tuple<Args&&...>(std::forward<Args>(args)...));

                thisJob->function = [](JobInternal* job) {
                        char*         bufferAlias    = job->padding;
                        CallableType& callableObject = *reinterpret_cast<CallableType*>(bufferAlias);
                        using ArgsTupleType          = std::tuple<Args&&...>;

                        bufferAlias += sizeof(CallableType);
                        ArgsTupleType& argsTuple = *reinterpret_cast<ArgsTupleType*>(bufferAlias);
                        std::apply(callableObject, std::forward<ArgsTupleType>(argsTuple));

                        callableObject.~CallableType();
                        argsTuple.~ArgsTupleType();
                };

                return thisJob;
        }


        template <typename Allocator, typename R, typename Lambda, typename... Args>
        struct ParallelForJobImpl
        {
            protected:
                JobInternal*              root;
                std::vector<JobInternal*> children;
                ParallelForJobImpl(Lambda&& lambda, unsigned begin, unsigned end, unsigned chunkSize)
                {
                        root     = CreateJobData([]() {});
                        children = CreateParallelForSubJobs(std::forward<Lambda>(lambda), begin, end, chunkSize);
                }

            private:
                template <unsigned... Is>
                static R ParallelForApplyImpl(Lambda&&              lambda,
                                              unsigned              index,
                                              std::tuple<Args...>&& tuple,
                                              std::index_sequence<Is...>)
                {
                        return lambda(index, std::get<Is>(tuple)...);
                }

                static void ParallelForApply(Lambda&& lambda, unsigned index, std::tuple<Args...>&& tuple)
                {
                        ParallelForApplyImpl(std::forward<Lambda>(lambda),
                                             index,
                                             std::forward<std::tuple<Args...>>(tuple),
                                             std::make_index_sequence<sizeof...(Args)>{});
                }

                JobInternal* CreateParallelForSubJobImpl(Lambda&& lambda, unsigned begin, unsigned end)
                {
                        JobInternal* thisJob     = Allocator::Allocate();
                        char*        bufferAlias = thisJob->padding;

                        InPlaceForwardConstruct(bufferAlias, std::forward<Lambda>(lambda));
                        bufferAlias += sizeof(Lambda);

                        InPlaceForwardConstruct(bufferAlias, std::tuple<unsigned, unsigned>(begin, end));

                        thisJob->function = [](JobInternal* job) {
                                char* bufferAlias = job->padding;
                                auto* lambda      = reinterpret_cast<Lambda*>(bufferAlias);
                                bufferAlias += sizeof(Lambda);

                                auto [_begin, _end] = *reinterpret_cast<std::tuple<unsigned, unsigned>*>(bufferAlias);
                                if constexpr (sizeof...(Args))
                                {
                                        bufferAlias += sizeof(std::tuple<unsigned, unsigned>);
                                        auto _args = *reinterpret_cast<std::tuple<Args...>*>(bufferAlias);
                                        for (unsigned i = _begin; i < _end; i++)
                                        {
                                                ParallelForApply(
                                                    std::forward<Lambda>(*lambda), i, std::forward<std::tuple<Args...>>(_args));
                                        }
                                }
                                else
                                {
                                        for (unsigned i = _begin; i < _end; i++)
                                        {
                                                (*lambda)(i);
                                        }
                                }
                        };
                        return thisJob;
                }

                template <typename Lambda>
                std::vector<JobInternal*> CreateParallelForSubJobs(Lambda&& lambda,
                                                                   unsigned begin,
                                                                   unsigned end,
                                                                   unsigned chunkSize)
                {
                        std::vector<JobInternal*> output;
                        if (end - begin <= chunkSize)
                                output.push_back(CreateParallelForSubJobImpl(std::forward<Lambda>(lambda), begin, end));
                        else
                        {
                                unsigned leftBegin  = begin;
                                unsigned leftEnd    = ((end - begin) / 2) + begin;
                                unsigned rightBegin = leftEnd;
                                unsigned rightEnd   = end;

                                auto left =
                                    CreateParallelForSubJobs(std::forward<Lambda>(lambda), leftBegin, leftEnd, chunkSize);

                                auto right =
                                    CreateParallelForSubJobs(std::forward<Lambda>(lambda), rightBegin, rightEnd, chunkSize);

                                output.insert(output.end(), left.begin(), left.end());
                                output.insert(output.end(), right.begin(), right.end());
                        }

                        return output;
                }

                void ResetJobs()
                {
                        root->unfinished_jobs = children.size() + 1;
                        root->parent          = 0;
                        for (const auto& itr : children)
                        {
                                itr->parent          = root;
                                itr->unfinished_jobs = 1;
                        }
                }

                void Run()
                {
                        JobSchedulerInternal::Run(root);
                        for (const auto& itr : children)
                                JobSchedulerInternal::Run(itr);
                }

            public:
                void Wait()
                {
                        JobSchedulerInternal::Wait(root);
                }
                void SetArgs(Args... args)
                {
                        for (const auto& itr : children)
                        {
                                char* bufferAlias = itr->padding;
                                bufferAlias += sizeof(Lambda) + sizeof(std::tuple<unsigned, unsigned>);
                                auto bufferArgsAlias = reinterpret_cast<std::tuple<Args...>*>(bufferAlias);
                                InPlaceForwardConstruct(bufferArgsAlias, std::tuple(args...));
                        }
                }
                void SetRange(unsigned begin, unsigned end, unsigned chunkSize = 256)
                {
                        char* bufferAlias = children[0]->padding;
                        auto& lambda      = *reinterpret_cast<Lambda*>(bufferAlias);
                        children          = CreateParallelForSubJobs(std::forward<Lambda>(lambda), begin, end, chunkSize);
                }
                void operator()(Args... args)
                {
                        SetArgs(args...);
                        ResetJobs();
                        Run();
                }
                JobInternal* GetRootJob()
                {
                        return root;
                }
        };
        template <typename Allocator, typename Lambda>
        struct ParallelForJob : public ParallelForJob<Allocator, decltype(&Lambda::operator())>
        {
                ParallelForJob(Lambda&& lambda, unsigned begin, unsigned end, unsigned chunkSize = 256) :
                    ParallelForJob<Allocator, decltype(&Lambda::operator())>(std::forward<Lambda>(lambda),
                                                                             begin,
                                                                             end,
                                                                             chunkSize)
                {}
                ParallelForJob(Lambda&& lambda) :
                    ParallelForJob<Allocator, decltype(&Lambda::operator())>(std::forward<Lambda>(lambda), 0, 0, 0)
                {}
        };
        template <typename Allocator, typename R, typename Lambda, typename... Args>
        struct ParallelForJob<Allocator, R (Lambda::*)(unsigned, Args...)>
            : public ParallelForJobImpl<Allocator, R, Lambda, Args...>
        {
                ParallelForJob(Lambda&& lambda, unsigned begin, unsigned end, unsigned chunkSize) :
                    ParallelForJobImpl<Allocator, R, Lambda, Args...>(std::forward<Lambda>(lambda), begin, end, chunkSize)
                {}
        };
        template <typename Allocator, typename R, typename Lambda, typename... Args>
        struct ParallelForJob<Allocator, R (Lambda::*)(unsigned, Args...) const>
            : public ParallelForJobImpl<Allocator, R, Lambda, Args...>
        {
                ParallelForJob(Lambda&& lambda, unsigned begin, unsigned end, unsigned chunkSize) :
                    ParallelForJobImpl<Allocator, R, Lambda, Args...>(std::forward<Lambda>(lambda), begin, end, chunkSize)
                {}
        };

        template <typename Component, typename JobAllocator, typename R, typename Lambda, typename... Args>
        struct ParallelForActiveImpl
        {
            protected:
                ParallelForActiveImpl(Lambda&& lambda, unsigned chunkSize)
                {
                        root = CreateJobData([]() {});

                        auto  rg_PoolIndex                  = Component::SGetTypeIndex();
                        auto& rg_ComponentRandomAccessPools = GEngine::Get()->GetHandleManager()->m_ComponentRandomAccessPools;
                        if (rg_ComponentRandomAccessPools.m_mem_starts.size() <= rg_PoolIndex)
                                return;
                        auto rg_ComponentCount = rg_ComponentRandomAccessPools.m_element_counts[rg_PoolIndex];
                        children = CreateParallelForSubJobs(std::forward<Lambda>(lambda), 0, rg_ComponentCount, chunkSize);
                }

            private:
                JobInternal*              root;
                std::vector<JobInternal*> children;

                JobInternal* CreateParallelForSubJobImpl(Lambda&& lambda, unsigned begin, unsigned end)
                {


                        JobInternal* thisJob     = JobAllocator::Allocate();
                        char*        bufferAlias = thisJob->padding;

                        InPlaceForwardConstruct(bufferAlias, std::forward<Lambda>(lambda));
                        bufferAlias += sizeof(Lambda);

                        InPlaceForwardConstruct(bufferAlias, std::tuple<unsigned, unsigned>(begin, end));

                        thisJob->function = [](JobInternal* job) {
                                char* bufferAlias = job->padding;
                                auto* lambda      = reinterpret_cast<Lambda*>(bufferAlias);
                                bufferAlias += sizeof(Lambda);

                                auto [_begin, _end] = *reinterpret_cast<std::tuple<unsigned, unsigned>*>(bufferAlias);

                                if constexpr (sizeof...(Args))
                                {
                                        auto  _PoolIndex = Component::SGetTypeIndex();
                                        auto& _ComponentRandomAccessPools =
                                            GEngine::Get()->GetHandleManager()->m_ComponentRandomAccessPools;
                                        auto  _ComponentCount = _ComponentRandomAccessPools.m_element_counts[_PoolIndex];
                                        auto& _IsActives      = _ComponentRandomAccessPools.m_element_isactives[_PoolIndex];
                                        auto  _ComponentsBegin =
                                            reinterpret_cast<Component*>(_ComponentRandomAccessPools.m_mem_starts[_PoolIndex]);
                                        active_range<Component> _active_range(
                                            _ComponentsBegin + _begin, _ComponentCount + _end, _IsActives);
                                        bufferAlias += sizeof(std::tuple<unsigned, unsigned>);
                                        auto _args = *reinterpret_cast<std::tuple<Args...>*>(bufferAlias);
                                        for (auto& itr : _active_range)
                                        {
                                                ParallelForApply(std::forward<Lambda>(*lambda),
                                                                 itr,
                                                                 std::forward<std::tuple<Args...>>(_args));
                                        }
                                }
                                else
                                {
                                        auto  _PoolIndex = Component::SGetTypeIndex();
                                        auto& _ComponentRandomAccessPools =
                                            GEngine::Get()->GetHandleManager()->m_ComponentRandomAccessPools;
                                        auto  _ComponentCount = _ComponentRandomAccessPools.m_element_counts[_PoolIndex];
                                        auto& _IsActives      = _ComponentRandomAccessPools.m_element_isactives[_PoolIndex];
                                        auto  _ComponentsBegin =
                                            reinterpret_cast<Component*>(_ComponentRandomAccessPools.m_mem_starts[_PoolIndex]);
                                        active_range<Component> _active_range(
                                            _ComponentsBegin + _begin, _ComponentCount + _end, _IsActives);

                                        for (auto& itr : _active_range)
                                        {
                                                (*lambda)(itr);
                                        }
                                }
                        };
                        return thisJob;
                }
                template <typename Lambda>
                std::vector<JobInternal*> CreateParallelForSubJobs(Lambda&& lambda,
                                                                   unsigned begin,
                                                                   unsigned end,
                                                                   unsigned chunkSize)
                {
                        std::vector<JobInternal*> output;
                        if (end - begin <= chunkSize)
                                output.push_back(CreateParallelForSubJobImpl(std::forward<Lambda>(lambda), begin, end));
                        else
                        {
                                unsigned leftBegin  = begin;
                                unsigned leftEnd    = ((end - begin) / 2) + begin;
                                unsigned rightBegin = leftEnd;
                                unsigned rightEnd   = end;

                                auto left =
                                    CreateParallelForSubJobs(std::forward<Lambda>(lambda), leftBegin, leftEnd, chunkSize);

                                auto right =
                                    CreateParallelForSubJobs(std::forward<Lambda>(lambda), rightBegin, rightEnd, chunkSize);

                                output.insert(output.end(), left.begin(), left.end());
                                output.insert(output.end(), right.begin(), right.end());
                        }

                        return output;
                }
                template <unsigned... Is>
                static R ParallelForApplyImpl(Lambda&&              lambda,
                                              Component&            component,
                                              std::tuple<Args...>&& tuple,
                                              std::index_sequence<Is...>)
                {
                        return lambda(component, std::get<Is>(tuple)...);
                }

                static void ParallelForApply(Lambda&& lambda, Component& component, std::tuple<Args...>&& tuple)
                {
                        ParallelForApplyImpl(std::forward<Lambda>(lambda),
                                             component,
                                             std::forward<std::tuple<Args...>>(tuple),
                                             std::make_index_sequence<sizeof...(Args)>{});
                }
                void ResetJobs()
                {
                        root->unfinished_jobs = static_cast<long>(children.size()) + 1;
                        root->parent          = 0;
                        for (const auto& itr : children)
                        {
                                itr->parent          = root;
                                itr->unfinished_jobs = 1;
                        }
                }
                void Run()
                {
                        JobSchedulerInternal::Run(root);
                        for (const auto& itr : children)
                                JobSchedulerInternal::Run(itr);
                }

            public:
                void Wait()
                {
                        JobSchedulerInternal::Wait(root);
                }
                void SetArgs(Args... args)
                {
                        for (const auto& itr : children)
                        {
                                char* bufferAlias = itr->padding;
                                bufferAlias += sizeof(Lambda) + sizeof(std::tuple<unsigned, unsigned>);
                                auto bufferArgsAlias = reinterpret_cast<std::tuple<Args...>*>(bufferAlias);
                                InPlaceForwardConstruct(bufferArgsAlias, std::tuple(args...));
                        }
                }
                void SetRange(unsigned begin, unsigned end, unsigned chunkSize = 256)
                {
                        char* bufferAlias = children[0]->padding;
                        auto& lambda      = *reinterpret_cast<Lambda*>(bufferAlias);
                        children          = CreateParallelForSubJobs(std::forward<Lambda>(lambda), begin, end, chunkSize);
                }
                void operator()(Args... args)
                {
                        SetArgs(args...);
                        ResetJobs();
                        Run();
                }
                JobInternal* GetRootJob()
                {
                        return root;
                }
        };
        template <typename Component, typename JobAllocator, typename Lambda>
        struct ParallelForActiveComponentsLambdaExpander
            : public ParallelForActiveComponentsLambdaExpander<Component, JobAllocator, decltype(&Lambda::operator())>
        {
                ParallelForActiveComponentsLambdaExpander(Lambda&& lambda, unsigned chunkSize) :
                    ParallelForActiveComponentsLambdaExpander<Component, JobAllocator, decltype(&Lambda::operator())>(
                        std::forward<Lambda>(lambda),
                        chunkSize)
                {}
        };
        template <typename Component, typename JobAllocator, typename R, typename Lambda, typename... Args>
        struct ParallelForActiveComponentsLambdaExpander<Component, JobAllocator, R (Lambda::*)(Component&, Args...) const>
            : public ParallelForActiveImpl<Component, JobAllocator, R, Lambda, Args...>
        {
                ParallelForActiveComponentsLambdaExpander(Lambda&& lambda, unsigned chunkSize) :
                    ParallelForActiveImpl<Component, JobAllocator, R, Lambda, Args...>(std::forward<Lambda>(lambda), chunkSize)
                {}
        };
        template <typename Component, typename JobAllocator, typename R, typename Lambda, typename... Args>
        struct ParallelForActiveComponentsLambdaExpander<Component, JobAllocator, R (Lambda::*)(Component&, Args...)>
            : public ParallelForActiveImpl<Component, JobAllocator, R, Lambda, Args...>
        {
                ParallelForActiveComponentsLambdaExpander(Lambda&& lambda, unsigned chunkSize) :
                    ParallelForActiveImpl<Component, JobAllocator, R, Lambda, Args...>(std::forward<Lambda>(lambda), chunkSize)
                {}
        };

        template <typename Component, typename JobAllocator, typename R, typename Lambda, typename... Args>
        struct ParallelForComponentsImpl
        {
            protected:
                ParallelForComponentsImpl(Lambda&& lambda, unsigned chunkSize)
                {
                        root = CreateJobData([]() {});

                        auto  rg_PoolIndex                  = Component::SGetTypeIndex();
                        auto& rg_ComponentRandomAccessPools = GEngine::Get()->GetHandleManager()->m_ComponentRandomAccessPools;
                        auto  rg_ComponentCount             = rg_ComponentRandomAccessPools.m_element_counts[rg_PoolIndex];

                        children = CreateParallelForSubJobs(std::forward<Lambda>(lambda), 0, rg_ComponentCount, chunkSize);
                }

            private:
                JobInternal*              root;
                std::vector<JobInternal*> children;

                JobInternal* CreateParallelForSubJobImpl(Lambda&& lambda, unsigned begin, unsigned end)
                {
                        JobInternal* thisJob     = JobAllocator::Allocate();
                        char*        bufferAlias = thisJob->padding;

                        InPlaceForwardConstruct(bufferAlias, std::forward<Lambda>(lambda));
                        bufferAlias += sizeof(Lambda);

                        InPlaceForwardConstruct(bufferAlias, std::tuple<unsigned, unsigned>(begin, end));

                        thisJob->function = [](JobInternal* job) {
                                char* bufferAlias = job->padding;
                                auto* lambda      = reinterpret_cast<Lambda*>(bufferAlias);
                                bufferAlias += sizeof(Lambda);

                                auto [_begin, _end] = *reinterpret_cast<std::tuple<unsigned, unsigned>*>(bufferAlias);

                                if constexpr (sizeof...(Args))
                                {
                                        auto  _PoolIndex = Component::SGetTypeIndex();
                                        auto& _ComponentRandomAccessPools =
                                            GEngine::Get()->GetHandleManager()->m_ComponentRandomAccessPools;
                                        auto _ComponentCount = _ComponentRandomAccessPools.m_element_counts[_PoolIndex];
                                        auto _ComponentsBegin =
                                            reinterpret_cast<Component*>(_ComponentRandomAccessPools.m_mem_starts[_PoolIndex]);
                                        range<Component> _range(_ComponentsBegin + _begin, _ComponentCount + _end);


                                        bufferAlias += sizeof(std::tuple<unsigned, unsigned>);
                                        auto _args = *reinterpret_cast<std::tuple<Args...>*>(bufferAlias);
                                        for (auto& itr : _range)
                                        {
                                                ParallelForApply(std::forward<Lambda>(*lambda),
                                                                 itr,
                                                                 std::forward<std::tuple<Args...>>(_args));
                                        }
                                }
                                else
                                {
                                        auto  _PoolIndex = Component::SGetTypeIndex();
                                        auto& _ComponentRandomAccessPools =
                                            GEngine::Get()->GetHandleManager()->m_ComponentRandomAccessPools;
                                        auto _ComponentCount = _ComponentRandomAccessPools.m_element_counts[_PoolIndex];
                                        auto _ComponentsBegin =
                                            reinterpret_cast<Component*>(_ComponentRandomAccessPools.m_mem_starts[_PoolIndex]);
                                        range<Component> _range(_ComponentsBegin + _begin, _ComponentCount + _end);

                                        for (auto& itr : _range)
                                        {
                                                (*lambda)(itr);
                                        }
                                }
                        };
                        return thisJob;
                }
                template <typename Lambda>
                std::vector<JobInternal*> CreateParallelForSubJobs(Lambda&& lambda,
                                                                   unsigned begin,
                                                                   unsigned end,
                                                                   unsigned chunkSize)
                {
                        std::vector<JobInternal*> output;
                        if (end - begin <= chunkSize)
                                output.push_back(CreateParallelForSubJobImpl(std::forward<Lambda>(lambda), begin, end));
                        else
                        {
                                unsigned leftBegin  = begin;
                                unsigned leftEnd    = ((end - begin) / 2) + begin;
                                unsigned rightBegin = leftEnd;
                                unsigned rightEnd   = end;

                                auto left =
                                    CreateParallelForSubJobs(std::forward<Lambda>(lambda), leftBegin, leftEnd, chunkSize);

                                auto right =
                                    CreateParallelForSubJobs(std::forward<Lambda>(lambda), rightBegin, rightEnd, chunkSize);

                                output.insert(output.end(), left.begin(), left.end());
                                output.insert(output.end(), right.begin(), right.end());
                        }

                        return output;
                }
                template <unsigned... Is>
                static R ParallelForApplyImpl(Lambda&&              lambda,
                                              Component&            component,
                                              std::tuple<Args...>&& tuple,
                                              std::index_sequence<Is...>)
                {
                        return lambda(component, std::get<Is>(tuple)...);
                }

                static void ParallelForApply(Lambda&& lambda, Component& component, std::tuple<Args...>&& tuple)
                {
                        ParallelForApplyImpl(std::forward<Lambda>(lambda),
                                             component,
                                             std::forward<std::tuple<Args...>>(tuple),
                                             std::make_index_sequence<sizeof...(Args)>{});
                }
                void ResetJobs()
                {
                        root->unfinished_jobs = children.size() + 1;
                        root->parent          = 0;
                        for (const auto& itr : children)
                        {
                                itr->parent          = root;
                                itr->unfinished_jobs = 1;
                        }
                }
                void Run()
                {
                        JobSchedulerInternal::Run(root);
                        for (const auto& itr : children)
                                JobSchedulerInternal::Run(itr);
                }

            public:
                void Wait()
                {
                        JobSchedulerInternal::Wait(root);
                }
                void SetArgs(Args... args)
                {
                        for (const auto& itr : children)
                        {
                                char* bufferAlias = itr->padding;
                                bufferAlias += sizeof(Lambda) + sizeof(std::tuple<unsigned, unsigned>);
                                auto bufferArgsAlias = reinterpret_cast<std::tuple<Args...>*>(bufferAlias);
                                InPlaceForwardConstruct(bufferArgsAlias, std::tuple(args...));
                        }
                }
                void SetRange(unsigned begin, unsigned end, unsigned chunkSize = 256)
                {
                        char* bufferAlias = children[0]->padding;
                        auto& lambda      = *reinterpret_cast<Lambda*>(bufferAlias);
                        children          = CreateParallelForSubJobs(std::forward<Lambda>(lambda), begin, end, chunkSize);
                }
                void operator()(Args... args)
                {
                        SetArgs(args...);
                        ResetJobs();
                        Run();
                }
                JobInternal* GetRootJob()
                {
                        return root;
                }
        };
        template <typename Component, typename JobAllocator, typename Lambda>
        struct ParallelForComponentsLambdaExpander
            : public ParallelForComponentsLambdaExpander<Component, JobAllocator, decltype(&Lambda::operator())>
        {
                ParallelForComponentsLambdaExpander(Lambda&& lambda, unsigned chunkSize) :
                    ParallelForComponentsLambdaExpander<Component, JobAllocator, decltype(&Lambda::operator())>(
                        std::forward<Lambda>(lambda),
                        chunkSize)
                {}
        };
        template <typename Component, typename JobAllocator, typename R, typename Lambda, typename... Args>
        struct ParallelForComponentsLambdaExpander<Component, JobAllocator, R (Lambda::*)(Component&, Args...) const>
            : public ParallelForComponentsImpl<Component, JobAllocator, R, Lambda, Args...>
        {
                ParallelForComponentsLambdaExpander(Lambda&& lambda, unsigned chunkSize) :
                    ParallelForComponentsImpl<Component, JobAllocator, R, Lambda, Args...>(std::forward<Lambda>(lambda),
                                                                                           chunkSize)
                {}
        };
        template <typename Component, typename JobAllocator, typename R, typename Lambda, typename... Args>
        struct ParallelForComponentsLambdaExpander<Component, JobAllocator, R (Lambda::*)(Component&, Args...)>
            : public ParallelForComponentsImpl<Component, JobAllocator, R, Lambda, Args...>
        {
                ParallelForComponentsLambdaExpander(Lambda&& lambda, unsigned chunkSize) :
                    ParallelForComponentsImpl<Component, JobAllocator, R, Lambda, Args...>(std::forward<Lambda>(lambda),
                                                                                           chunkSize)
                {}
        };
} // namespace JobSchedulerAbstractionsInternal


inline namespace JobSchedulerValidation
{
        template <typename TupleA, typename TupleB, unsigned I, unsigned J>
        constexpr bool IntersectsImpl_HoldIConst_PopJs(TupleA tupleA,
                                                       TupleB tupleB,
                                                       std::index_sequence<I>,
                                                       std::index_sequence<J>)
        {
                return (std::get<I>(tupleA) == std::get<J>(tupleB));
        }
        template <typename TupleA, typename TupleB, unsigned I, unsigned J, unsigned... Js>
        constexpr typename std::enable_if<sizeof...(Js), bool>::type
        IntersectsImpl_HoldIConst_PopJs(TupleA tupleA, TupleB tupleB, std::index_sequence<I>, std::index_sequence<J, Js...>)
        {
                // does element[I] == element[J]?
                if (std::get<I>(tupleA) == std::get<J>(tupleB))
                        return true;
                // does element[I] == any of the other element[Js]?
                return IntersectsImpl_HoldIConst_PopJs(tupleA, tupleB, std::index_sequence<I>{}, std::index_sequence<Js...>{});
        }
        template <typename TupleA, typename TupleB, unsigned I, unsigned... Js>
        constexpr bool IntersectsImpl_PopI(TupleA tupleA, TupleB tupleB, std::index_sequence<I>, std::index_sequence<Js...>)
        {
                return IntersectsImpl_HoldIConst_PopJs(tupleA, tupleB, std::index_sequence<I>{}, std::index_sequence<Js...>{});
        }
        template <typename TupleA, typename TupleB, unsigned I, unsigned... Is, unsigned... Js>
        constexpr typename std::enable_if<sizeof...(Is), bool>::type IntersectsImpl_PopI(TupleA tupleA,
                                                                                         TupleB tupleB,
                                                                                         std::index_sequence<I, Is...>,
                                                                                         std::index_sequence<Js...>)
        {
                bool IntersectionFound =
                    IntersectsImpl_HoldIConst_PopJs(tupleA, tupleB, std::index_sequence<I>{}, std::index_sequence<Js...>{});
                if (IntersectionFound)
                        return true;
                return IntersectsImpl_PopI(tupleA, tupleB, std::index_sequence<Is...>{}, std::index_sequence<Js...>{});
        }

        template <typename... Args>
        struct Reads
        {
                std::tuple<const Args&...> data;
                Reads(Args&... args) : data(args...)
                {}
                template <std::size_t N>
                const auto& get() const
                {
                        return std::get<N>(data);
                }
        };
        template <typename... Args>
        struct ::std::tuple_size<::Reads<Args...>> : std::integral_constant<std::size_t, sizeof...(Args)>
        {};
        template <std::size_t N, typename... Args>
        struct ::std::tuple_element<N, ::Reads<Args...>>
        {
                using type = decltype(std::declval<::Reads<Args...>>().get<N>());
        };

        template <typename... Args>
        struct Writes
        {
                std::tuple<Args&...> data;
                Writes(Args&... args) : data(args...)
                {}
                template <std::size_t N>
                auto& get() const
                {
                        return std::get<N>(data);
                }
        };
        template <typename... Args>
        struct ::std::tuple_size<::Writes<Args...>> : std::integral_constant<std::size_t, sizeof...(Args)>
        {};
        template <std::size_t N, typename... Args>
        struct ::std::tuple_element<N, ::Writes<Args...>>
        {
                using type = decltype(std::declval<::Writes<Args...>>().get<N>());
        };

        template <typename TupleA, typename TupleB>
        constexpr bool Intersects(TupleA tupleA, TupleB tupleB)
        {
                return IntersectsImpl_PopI(tupleA,
                                           tupleB,
                                           std::make_index_sequence<std::tuple_size<TupleA>::value>{},
                                           std::make_index_sequence<std::tuple_size<TupleB>::value>{});
        }
        template <typename... TupleElements, unsigned... Is>
        std::tuple<TupleElements*...> AddrOfTupleElementsImpl(std::tuple<TupleElements&...> input, std::index_sequence<Is...>)
        {
                return std::tuple<TupleElements*...>((&std::get<Is>(input))...);
        }
        template <typename... TupleElements>
        std::tuple<TupleElements*...> AddrOfTupleElements(std::tuple<TupleElements&...> input)
        {
                return AddrOfTupleElementsImpl(input, std::make_index_sequence<sizeof...(TupleElements)>{});
        }

        template <typename... Reads_ts, typename... Writes_ts>
        constexpr bool Validate(Reads<Reads_ts...> reads, Writes<Writes_ts...> writes)
        {
                auto reads_addrs  = AddrOfTupleElements(reads.data);
                auto writes_addrs = AddrOfTupleElements(writes.data);
                return !Intersects(reads_addrs, writes_addrs);
        }
} // namespace JobSchedulerValidation

inline namespace JobSchedulerAbstractions
{
        struct TempJobAllocator
        {
            private:
                static inline auto& AllocatorImpl = g_thread_local_job_allocator_temp;

            public:
                static inline JobInternal* Allocate()
                {
                        return g_thread_local_job_allocator_temp.Allocate();
                }
        };
        struct StaticJobAllocator
        {
            private:
                static inline auto& AllocatorImpl = g_thread_local_job_allocator_static;

            public:
                static inline JobInternal* Allocate()
                {
                        return g_thread_local_job_allocator_static.Allocate();
                }
        };

        template <typename JobAllocator = TempJobAllocator, typename Lambda>
        auto ParallelFor(Lambda&& lambda)
        {
                return ParallelForJob<JobAllocator, Lambda>(std::forward<Lambda>(lambda));
        }
        template <typename Component, typename JobAllocator = TempJobAllocator, typename Lambda>
        auto ParallelForActiveComponents(Lambda&& lambda, unsigned chunkSize = 256)
        {
                return ParallelForActiveComponentsLambdaExpander<Component, JobAllocator, Lambda>(std::forward<Lambda>(lambda),
                                                                                                  chunkSize);
        }
        template <typename Component, typename JobAllocator = TempJobAllocator, typename Lambda>
        auto ParallelForComponents(Lambda&& lambda, unsigned chunkSize = 256)
        {
                return ParallelForComponentsLambdaExpander<Component, JobAllocator, Lambda>(std::forward<Lambda>(lambda),
                                                                                            chunkSize);
        }

        template <typename Allocator = TempJobAllocator>
        struct Job
        {
                JobInternal* rootJob;

                template <typename Lambda, typename... Args>
                Job(Lambda&& lambda, Args&&... args)
                {
                        if constexpr (sizeof...(Args))
                                rootJob =
                                    CreateJobData<Allocator>(std::forward<Lambda>(lambda), std::forward<Args...>(args...));
                        else
                                rootJob = CreateJobData<Allocator>(std::forward<Lambda>(lambda));

                        rootJob->parent          = 0;
                        rootJob->unfinished_jobs = 1;
                }
                void Wait()
                {
                        JobSchedulerInternal::Wait(rootJob);
                }
                void operator()()
                {
                        JobSchedulerInternal::Run(rootJob);
                }
        };
} // namespace JobSchedulerAbstractions