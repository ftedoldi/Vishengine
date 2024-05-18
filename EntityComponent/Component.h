#pragma once

class Component{
public:
    virtual ~Component() = default;

    virtual void Start() = 0;
    virtual void Update() = 0;
};