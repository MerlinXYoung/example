#pragma once

#if 1
#include <memory>
#include <cassert>
#include <cstring>
#include "stdex.h"
template<class T_>
class Buffer
{

public:
    using value_type = T_;
    using pointer = T_*;
    using const_pointer = const T_*;
    using reference = T_&;
    using const_reference = const T_&;
    using value_ptr = std::unique_ptr<T_[]>;
    Buffer():ptr_(nullptr),size_(0),len_(0){}
    ~Buffer()=default;

    template<class Buf>
    void get(Buf& buf)
    {
        if(full())
        {
            expand();
        }
        buf.base = data() + len_;
        buf.len = size_ - len_;
    }
    bool empty()const
    {
        return len_ == 0;
    }
    bool full()const 
    {
        return size_ == len_;
    }
    pointer data(){
        return ptr_.get();
    }
    const_pointer data()const{
        return ptr_.get();
    }
    size_t len()const{
        return len_;
    }
    void use(size_t len){
        len_+= len;
    }
    void tidy(size_t len)
    {
        if(len == len_)
        {    
            len_ = 0;
            return;
        }
        size_t r = len_ - len;
        size_t size=r<128?128:r;
        value_ptr ptr(new T_[size]);
        assert(ptr);
        memcpy(ptr.get(), ptr_.get(), len_);
        std::swap(ptr, ptr_);
        size_ = size;
        len_ = r;
    }
private:
    void expand()
    {
        size_t size = size_==0?128:size_*1.73;
        value_ptr ptr(new T_[size]);
        assert(ptr);
        memcpy(ptr.get(), ptr_.get(), len_);
        std::swap(ptr, ptr_);
        size_ = size;
    }

private:
    value_ptr ptr_;
    size_t size_;
    size_t len_;
};
#else

#include <vector>
template<class T_, class C_ = std::vector<T_>>
class Buffer
{
public:
    using value_type = T_;
    using pointer = T_*;
    using const_pointer = const T_*;
    using reference = T_&;
    using const_reference = const T_&;

    Buffer():data_(),len_(0){}
    Buffer(size_t size):data_(size),len_(0){}
    Buffer(Buffer&& other){
        data_ = std::move(other.data_);
        len_ = other.len_;
    }
    ~Buffer()=default;

    template<class Buf>
    void get(Buf& buf)
    {
        if(full())
        {
            expand();
        }
        buf.base = &data_[len_];
        buf.len = data_.size() - len_;
    }
    bool empty()const
    {
        return data_.size() == 0;
    }
    bool full()const 
    {
        return data_.size() == len_;
    }
    pointer data(){
        return data_.data();
    }
    const_pointer data()const{
        return data_.data();
    }
    size_t len()const{
        return len_;
    }
    void use(size_t len){
        len_+= len;
    }
    void tidy(size_t len)
    {
        C_ data(data_.begin()+len, data_.begin()+len_);
        std::swap(data_, data);
    }
private:
    void expand()
    {
        data_.resize(data_.size()*2);
    }

private:
    C_ data_;
    size_t len_;
    
};
#endif

