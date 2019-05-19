#pragma once
#include <unordered_map>
#include <vector>
#include "Handle.h"

// public:
// uint32_t          m_CurrentIndex;
// ComponentManager* m_ComponentManager;
//
// public:
// ComponentIterator(uint32_t index) :
//    m_CurrentIndex(index){
//
//    };
//
//
// T* operator->()
//{
//        if (m_CurrentIndex >= m_ComponentManager->GetSize())
//                return nullptr;
//        return (T*)m_ComponentManager->m_HandleManager.m_HandleSpace[m_CurrentIndex].second;
//}
//
// T& operator*() const
//{
//        return *(m_ComponentManager->m_HandleManager.m_HandleSpace[m_CurrentIndex].second);
//}
//
// ComponentIterator operator++(int)
//{
//        ComponentIterator temp = *this;
//        m_CurrentIndex++;
//        if (m_CurrentIndex >= m_ComponentManager->GetSize())
//        {
//                return temp;
//        }
//        IComponent* _c = m_ComponentManager->m_HandleManager.m_HandleSpace[m_CurrentIndex].second;
//
//        ///////////////////////////////////////////////
//        // TODO USE ::value possibly pattern to circumvent this call to temporary var creation
//        // WARNING getting ths to work would be complicated and hacky
//        ///////////////////////////////////////////////
//        auto _tempDesiredComponentInstance = T();
//        auto _desiredTypeId                = _tempDesiredComponentInstance.GetStaticTypeId();
//        ///////////////////////////////////////////////
//        if (_c->GetStaticTypeId() != _desiredTypeId)
//                return this->operator++(0);
//
//        return temp;
//}
//
// bool operator!=(const ComponentIterator other) const
//{
//        return this->m_CurrentIndex != other.m_CurrentIndex;
//}
//
// bool operator==(const ComponentIterator other) const
//{
//        return this->m_CurrentIndex == other.m_CurrentIndex;
//}


template <typename T>
class HandleManager
{
    public:
        class iterator;

    public:
        // uint32_t  HandleId
        // T* raw data pointer
        std::vector<HandleData> m_HandleData;
        std::vector<T*>         m_ObjectData;

        Handle<T> GetHandle(T* object)
        {
                uint32_t i = 0;
                for (; i < m_ObjectData.size(); ++i)
                {
                        if (m_ObjectData[i] == nullptr)
                        {
                                m_ObjectData[i] = object;
                                m_HandleData[i] = HandleData();

                                Handle<T> out;
                                out.m_Id         = i;
                                out.m_HandleData = m_HandleData[i];
                                return out;
                        }
                        else if (m_ObjectData[i] == object)
                        {
                                Handle<T> out;
                                out.m_Id         = i;
                                out.m_HandleData = m_HandleData[i];
                        }
                }
                m_ObjectData.push_back(object);
                m_HandleData.push_back(HandleData());
                Handle<T> out;
                out.m_Id         = i;
                out.m_HandleData = m_HandleData[i];
                return out;
        }
        T* GetObject(Handle<T> handle)
        {
                return m_ObjectData[handle.m_Id];
        }
        size_t size()
        {
                return m_HandleData.size();
        }
        iterator begin()
        {
                return iterator(*this);
        }
        iterator end()
        {
                return iterator(*this, m_HandleData.size());
        }
        T& operator[](uint32_t idx)
        {
                return *m_ObjectData[idx];
        }
};

template <typename T>
class HandleManager<T>::iterator
{
    private:
        uint32_t          m_CurrentIndex;
        HandleManager<T>& m_HandleManager;

    public:
        iterator(HandleManager<T>& handleManager, uint32_t idx = 0U);
        iterator& operator++();
        iterator  operator++(int);
        bool      operator!=(const iterator& other) const;
        T&        operator*();
        T*        data();
};

template <typename T>
HandleManager<T>::iterator::iterator(HandleManager<T>& handleManager, uint32_t idx) :
    m_HandleManager(handleManager),
    m_CurrentIndex(idx){

    };

template <typename T>
typename bool HandleManager<T>::iterator::operator!=(const iterator& other) const
{
        return this->m_CurrentIndex != other.m_CurrentIndex;
};

template <typename T>
typename T* HandleManager<T>::iterator::data()
{
        return m_HandleManager.m_ObjectData[m_CurrentIndex];
};

template <typename T>
typename HandleManager<T>::iterator& HandleManager<T>::iterator::operator++()
{
        m_CurrentIndex++;
        while (m_CurrentIndex < m_HandleManager.size() && !m_HandleManager.m_ObjectData[m_CurrentIndex]->IsEnabled())
        {
                m_CurrentIndex++;
        }
        return *this;
};

template <typename T>
typename HandleManager<T>::iterator HandleManager<T>::iterator::operator++(int)
{
        auto temp = *this;
        m_CurrentIndex++;
        while (m_CurrentIndex < m_HandleManager.size() && !m_HandleManager.m_ObjectData[m_CurrentIndex]->IsEnabled())
        {
                m_CurrentIndex++;
        }
        return temp;
};

template <typename T>
typename T& HandleManager<T>::iterator::operator*()
{
        return *m_HandleManager.m_ObjectData[m_CurrentIndex];
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
