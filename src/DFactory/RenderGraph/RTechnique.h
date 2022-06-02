//#include "../D3DMgr/IBind/Bind_Includes.h"

class RTechnique
{
private:
	friend class RenderGraph;

	//
	//	GENERAL TECHNIQUE DATA
	//
	enum BindModes
	{
		BIND_MESH = 0, BIND_MESH_AND_TECHNIQUE = 1, BIND_TECHNIQUE = 2
	};

	struct TechniqueData
	{
		void BindTechnique() noexcept
		{
			for (const auto& it : m_Binds)
			{
				it ? it->Bind() : void();
			}
		};

		uint8_t m_BindMode = BIND_TECHNIQUE;					// bind mode when using this technique
		std::string m_Camera = "";								// camera that this technique will use for rendering, empty for no change
		bool m_BindLights = false;								// generate light data for mesh, required only for direct rendering passes
		std::string m_RB = "", m_DSB = "";						// render and depth buffers to switch to before processing pass
		int8_t m_depthState = -1;								// depth state for the current pass, -1 for no change
		std::vector<std::unique_ptr<Bind::IBind>> m_Binds;		// technique binds
		bool m_IsCSM = false;									// requires orthogonal camera and proper shadow depth buffer provided
	};

	//
	//	CASCADE SHADOW MAPPING TECHNIQUE DATA
	//
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

	std::unordered_map<std::string, TechniqueData> m_TechDB;
	bool m_InitializedDefaults = false;

public:
	RTechnique() noexcept {};
	RTechnique(const RTechnique&) = default;
	~RTechnique() = default;

	static RTechnique& Get() noexcept			// singleton creation and access method
	{
		static RTechnique _SInstance;
		return _SInstance;
	}

	void InitTechniques() noexcept;				// generate preset techniques
	CascadeShadowMapData& CSMData() noexcept;	// access CSM technique variables
};