#pragma once

template<typename T>
class Singleton
{
	static T* _instance;

public:
	static T* GetInstance()
	{
		if (_instance == nullptr)
		{
			_instance = new T();
		}
		return _instance;
	}
	static void DestroyInstance()
	{
		if (_instance != nullptr)
		{
			delete _instance;
			_instance = nullptr;
		}
	}

protected:
	// 생성자와 소멸자를 protected로 설정하면 외부에서 싱글톤 객체를 생성, 소멸할 수 없다.
	Singleton() = default;
	virtual ~Singleton() = default;

	Singleton(const Singleton&) = delete;				// 복사생성자 방지
	Singleton& operator=(const Singleton&) = delete;	// 대입연산자 방지
	// && : rvalue 참조
	// 이동 연산 방지
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;

	virtual void Destroy() {}
};

// static 멤버 초기화
template<typename T>
T* Singleton<T>::_instance = nullptr;