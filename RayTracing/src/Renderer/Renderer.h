#pragma once

#include "Walnut/Image.h"

#include "../Scene/Camera.h"
#include "Ray.h"
#include "../Scene/Scene.h"

#include <memory>
#include <glm/glm.hpp>

class Renderer {
public:
	struct Settings {
		bool Accumulate = true;
		bool Multithreading = true;
		bool PCHRandom = true;
		bool UseClockTime = true;
		bool UseFrameIndex = true;
		bool UseRayBounces = true;

		int RayBounces = 5;
		int ResolutionScale = 100;
	};
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& camera);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

	void ResetFrameIndex() { m_FrameIndex = 1; }
	int GetFrameIndex() { return m_FrameIndex; }
	Settings& GetSettings() { return m_Settings; }

	const uint32_t* GetImageData() const { return m_ImageData; }

	void SetTime(float time) { m_Time = time; }
	void GetTime(float& time) { m_Time = time; }
private:
	struct HitPayload {
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;

		int ObjectIndex;
	};

	void Accumulate(const uint32_t& x, const uint32_t& y);

	glm::vec4 RayGen(uint32_t x, uint32_t y); // PerPixel

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
	HitPayload Miss(const Ray& ray);
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	Settings m_Settings;

	std::vector<uint32_t> m_ImageHorizontalIterator, m_ImageVerticalIterator;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

	uint32_t m_FrameIndex = 1;

	float m_Time = 0.0f;
};