#ifndef NOBSERVER_H
#define NOBSERVER_H

#include "vector"
#include "FactoryMgr.h"
class nObserver {
public: 
	virtual void update() = 0;
	virtual ~nObserver() = default;
};

class nSubject {
private:
	std::vector<std::shared_ptr<nObserver>> observers;
public:
	void addObserver(const std::shared_ptr<nObserver>& a_object)
	{
		observers.push_back(a_object);
	}


	void notify() {
		for (const auto& object : observers) {
			if (object) {
				object->update();
			}
		}


	}
};

#endif // NOBSERVER_H
