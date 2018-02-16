#pragma once
#include "..\Texture2D.h"

class Texture2DDX12 : public Texture2D
{
public:
    
    // Inherited via Texture2D
    virtual int loadFromFile(std::string filename) override;
    virtual void bind(unsigned int slot) override;

private:

};