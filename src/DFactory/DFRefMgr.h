#pragma once

class DFRefMgr
{
public:
	enum class Type {

		Null = 0, Model, Camera, Light
	};

private:
	struct ListObject {
		void* ptr;
		Type type;
	};

	std::map<std::string, ListObject> m_refList;
public:
	static DFRefMgr& Get() {

		static DFRefMgr _SInstance;
		return _SInstance;
	}

	bool Add(const std::string& id, void* ptr, Type type);
	std::pair<void*, Type> Get(const std::string& id);
	bool Remove(const std::string& id);
};