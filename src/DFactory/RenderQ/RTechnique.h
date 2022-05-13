//#include "../D3DMgr/IBind/Bind_Includes.h"

class RTechnique
{
	friend class RTechniqueDB;
	friend class RPass;

public:
	RTechnique() = default;

	const uint32_t& Id() const noexcept
	{
		return m_Id;
	};
	const bool& RequiresMeshBinds() const noexcept
	{
		return m_UseMeshBinds;
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
	bool m_UseMeshBinds = false;							// technique relies on mesh binds
	//std::string m_CamId = "";								// camera that this technique will use for rendering, empty for no change
	CCamera* m_pCamera = nullptr;
	int8_t m_RB = -1, m_DSB = -1;							// render and depth buffers to switch to before rendering, -1 to not switch
	int8_t m_depthState = -1;								// depth state for the current pass, -1 for no change
	std::vector<std::unique_ptr<Bind::IBind>> m_Binds;		// technique binds
};

class RTechniqueDB
{
private:
	friend class RPass;
	std::vector<RTechnique> m_Techniques;
	bool m_InitializedDefaults = false;

public:
	RTechniqueDB() noexcept
	{
		/*
		// !!! there are no 32 passes so do I need this?
		// init technique vector
		for (uint8_t i = 0; i < 32; i++)
		{
			m_Techniques.emplace_back();
		}*/
	};
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
};