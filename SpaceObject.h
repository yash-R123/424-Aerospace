#ifndef SPACEOBJECT_H
#define SPACEOBJECT_H

#include <string>

class SpaceObject {
protected:
    std::string name;
    double mass;

public:
    SpaceObject();
    SpaceObject(const std::string& objectName, double objectMass);
    virtual ~SpaceObject();

    std::string getName() const;
    double getMass() const;

    void setName(const std::string& objectName);
    void setMass(double objectMass);
};

#endif
