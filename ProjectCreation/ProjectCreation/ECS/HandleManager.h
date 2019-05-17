#pragma once
#include <unordered_map>
#include <vector>
#include "Handle.h"

template <typename T>
class HandleManager
{
    public:
        class iterator;

    public:
        // uint32_t  HandleId
        // T* raw data pointer
        std::vector<HandleData>              m_HandleData;
        std::vector<T*>                      m_ObjectData;
        std::vector<std::pair<uint32_t, T*>> m_HandleSpace;

        Handle<T> GetHandle(T* object)
        {
                uint32_t i = 0;
                for (; i < m_ObjectData.size(); ++i)
                {
                        if (m_ObjectData[i] == nullptr)
                        {
                                // m_HandleSpace[i].second = object;
                                // m_HandleSpace[i].first  = m_HandleSpace[i].first;

                                m_ObjectData[i] = object;
                                m_HandleData[i] = HandleData();

                                Handle<T> out;
                                out.m_Id         = i;
                                out.m_HandleData = m_HandleData[i];
                                return out;

                                // return Handle<T>(i, m_HandleSpace[i].first);
                        }
                        else if (m_ObjectData[i] == object)
                        {
                                Handle<T> out;
                                out.m_Id         = i;
                                out.m_HandleData = m_HandleData[i];
                        }
                        // return Handle<T>(i, m_HandleSpace[i].first);
                }
                // m_HandleSpace.push_back(std::make_pair(0, object));
                m_ObjectData.push_back(object);
                m_HandleData.push_back(HandleData());
                Handle<T> out;
                out.m_Id         = i;
                out.m_HandleData = m_HandleData[i];
                return out;
                // return Handle<T>(i, m_HandleSpace[i].first);
        }
        T* GetObject(Handle<T> handle)
        {
                return m_ObjectData[handle.m_Id];
        }
        size_t GetSize()
        {
                return m_HandleData.size();
        }
        iterator begin()
        {
                return iterator();
        }
        T& operator[](uint32_t idx)
        {
                return *m_ObjectData[idx];
		}
};

template <typename T>
class HandleManager<T>::iterator
{
        iterator operator++(int);
};

class IHandleContainer
{};


// I interface pointer			IComponent
// B Base class pointer e.g.	Component<Transform>
// template <typename I>
// template <typename B>
// class HandleManagerNew<I>::HandleContainer
//{
//    public:
//        class iterator
//        {};
//
//        B*       operator();
//        iterator begin() const;
//        iterator end() const;
//};

template <typename I>
class HandleManagerAssociative
{

    public:
        class Container;

        // private:
        //    // pair
        //    //		uint32_t = typeId
        //    //		vector<I*> = pool
        //    std::vector<std::pair<uint32_t, Container>> m_TypeAssociativeObjectPools;

        // public:
        // template <typename B>
        // Container* GetContainer();
};
//
template <typename I>
class HandleManagerAssociative<I>::Container
{
        //    //private
        //		// Handle sorted pool
        //        //std::vector<I*> m_ObjectPool;
        //    //public:
        //    //    class iterator;
        //
        //    //    void      initialize(size_t);
        //    //    iterator  begin() const;
        //    //    iterator  end() const;
        //    //    uint32_t  size() const;
        //    //    I&        operator[](Handle<I>);
        //    //    I         operator[](int) const;
        //    //    Handle<I> AddObject(I*);
        //    //    I*        GetObject(Handle<I>);
        //    //    iterator  operator++(int);
};

// template <typename I>
// class HandleManagerAssociative<I>::Container::iterator
//{
//    private:
//        HandleManagerAssociative<I>::Container* m_OwnerContainer;
//
//    public:
//        I&       operator->();
//        I&       operator*();
//        bool     operator==(const HandleManagerAssociative<I>::Container::iterator other) const;
//        bool     operator!=(const HandleManagerAssociative<I>::Container::iterator other) const;
//        iterator operator++(int);
//};
//
// template <typename I>
// inline void HandleManagerAssociative<I>::Container::initialize(size_t)
//{}
//
// template <typename I>
// inline HandleManagerAssociative<I>::Container::iterator HandleManagerAssociative<I>::Container::begin() const
//{
//        return iterator();
//}
//
// template <typename I>
// inline HandleManagerAssociative<I>::Container::iterator HandleManagerAssociative<I>::Container::end() const
//{
//        return iterator();
//}
//
// template <typename I>
// inline uint32_t HandleManagerAssociative<I>::Container::size() const
//{
//        return uint32_t();
//}
//
// template <typename I>
// inline I& HandleManagerAssociative<I>::Container::operator[](Handle<I> handle)
//{
//        // TODO: insert return statement here
//}
//
// template <typename I>
// inline I HandleManagerAssociative<I>::Container::operator[](int) const
//{
//        return I();
//}
//
// template <typename I>
// inline Handle<I> HandleManagerAssociative<I>::Container::AddObject(I*)
//{
//        return Handle<I>();
//}
//
// template <typename I>
// inline I* HandleManagerAssociative<I>::Container::GetObject(Handle<I>)
//{
//        return NULL;
//}
//
// template <typename I>
// inline HandleManagerAssociative<I>::Container::iterator HandleManagerAssociative<I>::Container::operator++(int)
//{
//        return iterator();
//}
//
// template <typename I>
// inline I& HandleManagerAssociative<I>::Container::iterator::operator->()
//{
//        // TODO: insert return statement here
//}
//
// template <typename I>
// inline I& HandleManagerAssociative<I>::Container::iterator::operator*()
//{
//        // TODO: insert return statement here
//}
// template <typename I>
// inline bool HandleManagerAssociative<I>::Container::iterator::operator==(
//    const HandleManagerAssociative<I>::Container::iterator other) const
//{
//        return false;
//}
//
// template <typename I>
// inline bool HandleManagerAssociative<I>::Container::iterator::operator!=(
//    const HandleManagerAssociative<I>::Container::iterator other) const
//{
//        return false;
//}
//
// template <typename I>
// inline HandleManagerAssociative<I>::Container::iterator HandleManagerAssociative<I>::Container::iterator::operator++(int)
//{
//        return iterator();
//}
//
// template <typename I>
// template <typename B>
// inline HandleManagerAssociative<I>::Container* HandleManagerAssociative<I>::GetContainer()
//{
//        return NULL;
//}
