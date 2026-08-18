#pragma once

class Renderer
{
public:
	void Draw();

private:
	Renderer();
	~Renderer();

	void RenderStart();
	void RenderQuad();
	void RenderEnd();

private:
	friend class Engine;
};

