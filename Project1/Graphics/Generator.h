#pragma once
#ifndef GENERATOR_H
#define GENERATOR_H

#include "DrawShapes.h"
#include "Voxel.h"

class Generator {
public:
	virtual ~Generator() {}
	virtual void Update() = 0;
	virtual void Draw(Shapes& shapes, const glm::mat4& vp, const glm::vec4& color) = 0;
	virtual void DrawImgui() = 0;
};

#endif