//#include "../D3DMgr/IBind/Bind_Includes.h"

class RTechnique
{
	friend class RTechniqueDB;
	friend class RPass;

	enum BindModes
	{
		BIND_MESH = 0, BIND_MESH_AND_TECHNIQUE = 1, BIND_TECHNIQUE = 2
	};

public:
	RTechnique() = default;

	const uint32_t& Id() const noexcept
	{
		return m_Id;
	};
	const uint8_t& BindMode() const noexcept
	{
		return m_BindMode;
	}
	std::vector<std::unique_ptr<Bind::IBind>>* Binds() noexcept
	{
		return &m_Binds;
	}

	void BindTechnique() noexcept
	{
		for (const auto& it : m_Binds)
		{
			it ? it->Bind() : void();
		}
	};

private:
	uint32_t m_Id = 0;										// bitwise id
	uint8_t m_BindMode = BIND_TECHNIQUE;					// bind mode of the pass
	std::string m_Camera = "";								// camera that this technique will use for rendering, empty for no change
	bool m_BindLights = false;								// generate light data for mesh, required only for direct rendering passes
	std::string m_RB = "", m_DSB = "";						// render and depth buffers to switch to before processing pass
	int8_t m_depthState = -1;								// depth state for the current pass, -1 for no change
	std::vector<std::unique_ptr<Bind::IBind>> m_Binds;		// technique binds
	bool m_IsCShadowTechnique = false;						// requires orthogonal camera and proper shadow depth buffer provided
};

class RTechniqueDB
{
private:
	friend class RPass;

	std::vector<RTechnique> m_Techniques;
	bool m_InitializedDefaults = false;

	// used in cascade shadow mapping pass
	struct CascadeShadowMapData
	{
		struct
		{
			float width = 1.0f * DF::CSM::FOVmult;
			float height = 1.0f / DF::D3DM->GetAspectRatio() * DF::CSM::FOVmult;
		} cascade0;				// most detailed cascade

		struct
		{
			std::vector<float> nearZ;
			std::vector<float> farZ;
		} cascadeData;

		std::vector<D3D11_VIEWPORT> vp;

		// initialize CSM database
		CascadeShadowMapData() noexcept;

	} CSM_Data;

public:
	RTechniqueDB() noexcept {};
	RTechniqueDB(const RTechniqueDB&) = default;
	~RTechniqueDB() = default;

	static RTechniqueDB& Get() noexcept
	{
		static RTechniqueDB _SInstance;
		return _SInstance;
	}

	// generate technique and store it into the vector
	void InitDefaultTechniques() noexcept;
	size_t Size() const noexcept;
	CascadeShadowMapData& CSMData() noexcept;
};