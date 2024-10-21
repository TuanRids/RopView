#ifndef NOBSERVER_H
#define NOBSERVER_H

#include "vector"
#include "FactoryMgr.h"
class nObserver {
public: 
	virtual void update() = 0;
	virtual ~nObserver() = default;
};

// Free to create and call notify anywhere
class nSubject {
private:
	std::vector<std::shared_ptr<nObserver>> observers; // Static observers
public:
	void addObserver(const std::shared_ptr<nObserver>& a_object)
	{
		observers.push_back(a_object);
	}
	// Notify all: nullptr
	void notify(nFrame* currentFrame) {
		for (const auto& object : observers) {
			// check if object is the same with nFrame
			if (currentFrame == nullptr) 
			{ object->update();}
			nFrame* frameObserver = dynamic_cast<nFrame*>(object.get());
			if (frameObserver && frameObserver != currentFrame) {
				object->update();
			}
		}
	}
};

#endif // NOBSERVER_H
