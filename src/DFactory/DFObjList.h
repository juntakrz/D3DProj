#pragma once

class DFObjList
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

	std::map<std::string, ListObject> m_objList;
public:
	static DFObjList& Get() {

		static DFObjList _SInstance;
		return _SInstance;
	}

	void Add(const std::string& id, void* ptr, Type type);
	std::pair<void*, Type> Get(const std::string& id);
	void Remove(const std::string& id);
};