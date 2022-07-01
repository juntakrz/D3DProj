#include "DFMain.h"

void DFMain::JSONLoad(const wchar_t* path, json& out_j) noexcept
{
	std::ifstream fStream(path);
	out_j.clear();

	if (!fStream.good()) {
		std::wstring msg = L"Failed to load '" + std::wstring(mapName.begin(), mapName.end()) + L".dmap'.\nThe map files are corrupt or missing.";
		MessageBoxW(nullptr, msg.c_str(), L"Critical Error", MB_OK | MB_ICONERROR);
		exit(404);
	}

	fStream >> out_j;
}

void DFMain::JSONParseCameras(const json& cameraData) noexcept
{
	// parse cameras
	for (const auto& it : cameraData.at("cameras")) {

		// create camera if it doesn't exist
		if (it.contains("name")) {

			std::string name = it.at("name");
			float pos[3] = { 0.0f, 0.0f, 0.0f };
			float rotation[2] = { 0.0f, 0.0f };

			DF.CameraAdd(name);

			// set camera position
			if (it.contains("position")) {
				it.at("position").get_to(pos);
			}

			// set camera rotation
			DF.Camera(name)->SetPos({ pos[0], pos[1], pos[2] });

			if (it.contains("rotation")) {
				it.at("rotation").get_to(rotation);
			}

			DF.Camera(name)->SetRotation( rotation[0], rotation[1] );

			// set camera mode
			/* vars:
				orthographic: x = width, y = height, z = nearZ, w = farZ
				perspective:  x = FOV, y = aspect ratio, z = nearZ, w = farZ
			*/
			if (it.contains("mode")) {

				float vars[4];
				it.at("mode").at("variables").get_to(vars);

				if (it.at("mode").at("view") == "orthographic") {
					
					DF.Camera(name)->SetAsOrthographic(vars[0], vars[1], vars[2], vars[3]);
				}

				if (it.at("mode").at("view") == "perspective") {
					
					DF.Camera(name)->SetAsPerspective(vars[0], vars[1], vars[2], vars[3]);
				}
			}
		}
	}
}

void DFMain::JSONParseMaterials(const nlohmann::json& materials) noexcept
{
	DFMaterial::DFMATERIAL_DESC DFMatDesc{};

	for (const auto& it : materials) {

		// clear material description
		DFMatDesc = {};

		// get material name, must always be defined
		if (!it.contains("name")) {
			std::wstring msg = L"Failed to load '" + std::wstring(mapName.begin(), mapName.end()) + L"'\n.Definitions of materials are corrupt.";
			MessageBoxW(nullptr, L"Critical Error", msg.c_str(), MB_OK | MB_ICONERROR);
			exit(405);
		}
		DFMatDesc.name = it.at("name").get<std::string>();

#ifdef _DEBUG || _DFDEBUG
		std::string msg = "JSON: processing " + DFMatDesc.name + "\n";
		OutputDebugStringA(msg.c_str());
#endif

		(it.contains("manageTextures")) ? it.at("manageTextures").get_to(DFMatDesc.manageTextures) : 0;

		// shaders
		if (it.contains("shaders")) {
			auto container = it.at("shaders");
			(container.contains("vs")) ? container.at("vs").get_to(DFMatDesc.shaders.vertex) : "";
			(container.contains("gs")) ? container.at("gs").get_to(DFMatDesc.shaders.geometry) : "";
			(container.contains("ps")) ? container.at("ps").get_to(DFMatDesc.shaders.pixel) : "";
		}

		// textures
		if (it.contains("textures")) {
			auto container = it.at("textures");
			(container.contains("tex0")) ? container.at("tex0").get_to(DFMatDesc.textures.tex0) : "";
			(container.contains("tex1")) ? container.at("tex1").get_to(DFMatDesc.textures.tex1) : "";
			(container.contains("tex2")) ? container.at("tex2").get_to(DFMatDesc.textures.tex2) : "";
			(container.contains("tex3")) ? container.at("tex3").get_to(DFMatDesc.textures.tex3) : "";
			(container.contains("tex4")) ? container.at("tex4").get_to(DFMatDesc.textures.tex4) : "";
			(container.contains("tex5")) ? container.at("tex5").get_to(DFMatDesc.textures.tex5) : "";
		}

		// variables
		if (it.contains("variables")) {
			auto container = it.at("variables");

			(container.contains("intensity")) ? container.at("intensity").get_to(DFMatDesc.material.intensity) : 0;
			(container.contains("bumpiness")) ? container.at("bumpiness").get_to(DFMatDesc.material.bumpiness) : 0;
			(container.contains("metalness")) ? container.at("metalness").get_to(DFMatDesc.material.metalness) : 0;
			(container.contains("roughness")) ? container.at("roughness").get_to(DFMatDesc.material.roughness) : 0;

			if (container.contains("ambient")) {
				float colors[3];
				container.at("ambient").get_to(colors);
				DFMatDesc.material.ambientColor = { colors[0], colors[1], colors[2], 1.0 };
			}

			if (container.contains("F0")) {
				float colors[3];
				container.at("F0").get_to(colors);
				DFMatDesc.material.F0 = { colors[0], colors[1], colors[2] };
			}
		}

		// passes
		if (it.contains("passes")) {
			auto passes = it.at("passes");
			for (const auto& pass : passes) {

				std::string passName = pass.get<std::string>();

				(passName == "background") ? DFMatDesc.passes |= DF::Pass::Background : 0;
				(passName == "standard") ? DFMatDesc.passes |= DF::Pass::Standard : 0;
				(passName == "blur") ? DFMatDesc.passes |= DF::Pass::Blur : 0;
				(passName == "sprite") ? DFMatDesc.passes |= DF::Pass::PointSprites : 0;
				(passName == "shadow") ? DFMatDesc.passes |= DF::Pass::Shadow : 0;
				(passName == "null") ? DFMatDesc.passes = 0 : 0;
			}
		}

		DF.MatM->MatAdd(&DFMatDesc);
	}
}

void DFMain::JSONParseLights(const json& lights) noexcept
{
	for (const auto& it : lights) {

		float pos[3], color[3], intensity;

		// special code for direct light, as it is unique
		if (it.at("name") == "$directLight") {

			if (it.contains("position")) {
				it.at("position").get_to(pos);
				DF.LightM->DLSetPos({ pos[0], pos[1], pos[2] });
			}

			if (it.contains("color")) {
				it.at("color").get_to(color);
				DF.LightM->DL().color = { color[0], color[1], color[2], 1.0 };
			}
		}

		if (it.at("type") == "PL") {

			if (it.contains("position")) {

				it.at("position").get_to(pos);
				DF.LightM->PL(it.at("name").get<std::string>()).pMesh->SetPos(pos[0], pos[1], pos[2]);
			}

			if (it.contains("color")) {

				it.at("color").get_to(color);
				DF.LightM->PL(it.at("name").get<std::string>()).color = { color[0], color[1], color[2], 1.0 };
			}
		}
	}
}

void DFMain::JSONParseObjects(const json& objects) noexcept
{
	std::string name, material = DF::Default::material;
	bool createAABB = false, created = false;

	for (const auto& it : objects) {

		created = false;

		uint16_t paramA = 0, paramB = 0;
		float translation[3] = { 0.0f, 0.0f, 0.0f };
		float rotation[3] = { 0.0f, 0.0f, 0.0f };
		float scaling[3] = { 1.0f, 1.0f, 1.0f };

		name = it.at("name");
		(it.contains("createAABB")) ? createAABB = it.at("createAABB") : createAABB = false;
		(it.contains("divisionsH")) ? paramA = it.at("divisionsH") : 0;
		(it.contains("divisionsV")) ? paramB = it.at("divisionsV") : 0;
		(it.contains("divisions")) ? paramA = it.at("divisions") : 0;

		(it.contains("position")) ? it.at("position").get_to(translation) : 0;
		(it.contains("rotation")) ? it.at("rotation").get_to(rotation) : 0;
		(it.contains("scaling")) ? it.at("scaling").get_to(scaling) : 0;

		(it.contains("material")) ? material = it.at("material") : material = DF::Default::material;

		if (it.at("type") == "plane") {

			DF.ModelM->Create(DF::idPlane, name, createAABB, paramA, paramB);
			created = true;
		}

		if (it.at("type") == "cube") {

			DF.ModelM->Create(DF::idCube, name, createAABB);
			created = true;
		}

		if (it.at("type") == "sphere") {

			DF.ModelM->Create(DF::idSphere, name, createAABB, paramA);
			created = true;
		}

		if (created) {

			DF.ModelM->SetPos(translation[0], translation[1], translation[2]);
			DF.ModelM->SetRotation(rotation[0], rotation[1], rotation[2]);
			DF.ModelM->SetScaling(scaling[0], scaling[1], scaling[2]);

			DF.ModelM->SetMaterial(material, 0u);
		}
	}
}

void DFMain::JSONParseCommands(const json& commands) noexcept
{
	/*
	* command design styles:
	* A: command, source object type, target object type
	* B: command, source object type, variable type to set
	*/

	std::string command, srcObj, tgtObj;

	for (const auto& it : commands) {

		command = it.at("command");

		// command: "translateTo" - moves source object to target object's position
		if (command == "translateTo") {

			// source: srcCamera - perform command with the defined camera
			if (it.contains("srcCamera")) {

				srcObj = it.at("srcCamera");

				// target: tgtCamera - perform command using the target camera
				if (it.contains("tgtCamera")) {

					tgtObj = it.at("tgtCamera");


				}

				// target: tgtLight - perform command using the target light
				if (it.contains("tgtLight")) {

					tgtObj = it.at("tgtLight");

					// if target light is a direct light - execute direct light method
					if (tgtObj == "$directLight") {

						DF.Camera(srcObj)->SetPos(DF.LightM->DLGetPosA());
						DF.LightM->DLSetCamera(DF.Camera(srcObj));
					}
				}
			}
		}

		// command: lock object relative to the position (and focus of) another object
		if (command == "lockTo") {

			if (it.contains("srcCamera")) {
				
				srcObj = it.at("srcCamera");

				if(it.contains("tgtCamera")) {

					tgtObj = it.at("tgtCamera");
					DF.Camera(srcObj)->LockToCameraTarget(DF.Camera(tgtObj));
				}
			}
		}
	}
}
