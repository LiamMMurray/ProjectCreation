#pragma once
class IComponent
{
public:
	IComponent() {}
	virtual ~IComponent() {}
	virtual void method1() = 0; // "= 0" part makes this method pure virtual, and
	                            // also makes this class abstract.
	virtual void method2() = 0;
};

// example
class FooComponent : public IComponent
{
private:
	int myMember;

public:
	FooComponent() {}
	~FooComponent() {}
	void method1();
	void method2();
};

// Provide implementation for the first method
void FooComponent::method1()
{
	// Your implementation
}

// Provide implementation for the second method
void FooComponent::method2()
{
	// Your implementation
}
