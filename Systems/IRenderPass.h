#pragma once

class IRenderPass {
public:
    virtual ~IRenderPass() = default;

    virtual void Execute() = 0;
};
