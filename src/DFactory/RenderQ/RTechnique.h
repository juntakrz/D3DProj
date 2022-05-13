#include "../D3DMgr/IBind/Bind_Includes.h"

class RTechnique
{
	friend class RTechniqueDB;

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
	uint32_t m_Id = 0;			// bitwise id
	bool m_UseMeshBinds = false;
	std::vector<std::unique_ptr<Bind::IBind>> m_Binds;
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