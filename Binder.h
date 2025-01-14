#pragma once
#include <vector>
#include <memory>

template<typename BindType>
class Binder
{
public:
	Binder();
	~Binder();

	void Bind(std::shared_ptr<BindType> val);
	void SetValue(BindType val);

private:
	BindType type;
	std::vector<std::weak_ptr<BindType>> bindList;
};

template<typename BindType>
inline Binder<BindType>::Binder()
{
}

template<typename BindType>
inline Binder<BindType>::~Binder()
{
}

template<typename BindType>
inline void Binder<BindType>::Bind(std::shared_ptr<BindType> val)
{
	bindList.emplace_back(val);
}

template<typename BindType>
inline void Binder<BindType>::SetValue(BindType val)
{

}
