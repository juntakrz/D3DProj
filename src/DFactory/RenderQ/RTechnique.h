#include "../D3DMgr/IBind/Bind_Includes.h"

class RTechniqueStep
{
	friend class RenderQ;
	friend class RTechnique;
public:
	RTechniqueStep(std::vector<std::unique_ptr<Bind::IBind>> fxBinds, uint8_t pathId, uint8_t pathPriority = 0) noexcept
		: m_FXBinds(std::move(fxBinds)), m_PassId(pathId), m_PassPriority(pathPriority) {};

	void StepBind() noexcept;		// bind bindables used in this step

private:
	uint8_t m_PassId;
	uint8_t m_PassPriority;
	std::vector<std::unique_ptr<Bind::IBind>> m_FXBinds;
};

class RTechnique
{
public:
	RTechnique(uint32_t id) noexcept;
	RTechnique(const RTechnique&) = default;
	~RTechnique() = default;

	const uint32_t& GetId() const noexcept;
	std::vector<RTechniqueStep>* Steps() noexcept;

private:
	uint32_t m_id;	// bitwise id
	std::vector<RTechniqueStep> m_Steps;
};