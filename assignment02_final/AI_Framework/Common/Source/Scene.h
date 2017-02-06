#ifndef SCENE_H
#define SCENE_H

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	virtual void Init() = 0;
	virtual void Update(double _deltaTime) = 0;
	virtual void Render() = 0;
	virtual void Exit() = 0;
};

#endif // SCENE_H