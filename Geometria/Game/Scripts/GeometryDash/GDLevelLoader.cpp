#include "GDLevelLoader.h"
#include "../../../Encryption/Encryption.h"
#include "../../../String/StringAPI.h"
#include "../../../Web/Web.h"
#include "../../../Physics/Colliders/BoxCollider.h"
//#include "Graphics/Cores/Texture/rectpack2D/finders_interface.h"

void GDLevelLoader::OnStart()
{
	/*WebRequest request("http://www.boomlings.com/database/downloadGJLevel22.php", WebRequest::HttpMethod::HTTP_POST);
	WebResponse response;
	WebForm form;

	form.AddField("gameVersion", "20");
	form.AddField("binaryVersion", "35");
	form.AddField("gdw", "0");
	form.AddField("levelID", "1650666");
	form.AddField("secret", "Wmfd2893gb7");

	request.SendWebRequest(&response, form);

	std::string urlResult = response.body;*/

	GDTextures = TextureManager::OpenTexturePack("Game/Textures/Blocks/blocks.gtxp");

	//TextureManager::OpenSpriteSheet("Game/Textures/Blocks/GJ_GameSheet.plist");

	// Stereo Madness: 1556066
	// How by Spu7nix: 63395980
	// Future Funk by JonathanGD: 44062068
	// Block IDs by GDColon: 58079690
	// Test Level by BoxShards: 76620227 (Testing Certain Blocks)

	std::string urlResult = Web::Post("http://www.boomlings.com/database/downloadGJLevel22.php", "gameVersion=20&binaryVersion=35&gdw=0&levelID=1556066&secret=Wmfd2893gb7");
	
	std::string levelSubstring = StringAPI::GetSubstringBetween(urlResult, ":4:", ":5:");

	if (!StringAPI::StartsWith(levelSubstring, "kS"))
	{
		if (!StringAPI::StartsWith(levelSubstring, "-1") || levelSubstring != "")
		{
			//std::cout << levelSubstring << std::endl;
			std::string decodeBase64 = Encryption::Base64::Decode(levelSubstring, true);
			std::string decodeZLib = Encryption::ZLib::Inflate(decodeBase64, Encryption::ZLib::Format::GZIP);

			if (decodeZLib == "")
			{
				// This means the level is old, decode in ZLIB instead.
				decodeZLib = Encryption::ZLib::Inflate(decodeBase64, Encryption::ZLib::Format::ZLIB);
			}

			ReadLevel(decodeZLib);
		}
		else
		{
			std::cout << "Returned -1" << std::endl;
		}
	}
	else
	{
		ReadLevel(levelSubstring);
	}
}

void GDLevelLoader::ReadLevel(std::string rawData)
{
	std::vector<std::string> allObjects = StringAPI::SplitIntoVector(rawData, ";");
	LoadCustomProperties();

	TextureManager::UploadToGPU();

	for (auto i : allObjects)
	{
		if (!StringAPI::StartsWith(i, "kS"))
		{
			int objId = 0;
			Vector3 position, rotation, scale(1);
			bool fX = false, fY = false, noC = false;
			std::vector<std::string> allValues = StringAPI::SplitIntoVector(i, ",");
			for (int id = 0; id < allValues.size(); id += 2)
			{
				if (allValues[id] != "" && allValues[id + 1] != "")
				{
					int value = std::stoi(allValues[id]);
					float result = 0;
					try
					{
						result = std::stof(allValues[id + 1]);
					}
					catch (...)
					{

					}

					switch (value)
					{
					// Object ID
					case 1:
						objId = result;
						for (auto size : customSize)
						{
							if (size.first == result)
							{
								scale = size.second;
								break;
							}
						}

						for (auto collisions : noCollisions)
						{
							if (collisions == result)
							{
								noC = true;
								break;
							}
						}
						break;

					// X Position
					case 2:
						position.x = result / 30.0f;
						break;

					// Y Position
					case 3:
						position.y = result / 30.0f;
						break;

					//Flip X Texture
					case 4:
						if (allValues[id + 1] == "1")
							fX = true;
						break;

					//Flip Y Texture
					case 5:
						if (allValues[id + 1] == "1")
							fY = true;
						break;

					// Z Rotation
					case 6:
						rotation.z -= result;
						break;

					/*case 21:
						colorID = std::stoi(results[i]);
						break;*/

					// XY Scale
					case 32:
						if (scale.x != 0 && scale.y != 0)
						{
							scale.x *= result;
							scale.y *= result;
						}
						break;
					}
				}
			}
			
			Model* newObj = new Model(Model::Square(), position, rotation, scale);

			newObj->FlipTexture(fX, fY);

			if (!noC)
			{
				BoxCollider* b = newObj->AddScript<BoxCollider>();
				for (auto collSize : customCollisionSize)
				{
					if (collSize.first == objId)
					{
						b->SetScale(collSize.second);
					}
				}
			}

			newObj->SetTexture(*GDTextures[objId]);
			RendererCore::AddModel(*newObj);
		}
	}
}

void GDLevelLoader::LoadCustomProperties()
{
	//Custom Size
	//|
	//- Outline Blocks [Various]
	AddCustomSize(305, Vector3(80.f / 120, 80.f / 120, 1));
	AddCustomSize(307, Vector3(200.f / 120, 108.f / 120, 1));
	AddCustomSize(468, Vector3(120.f / 120, 6.f / 120, 1));
	AddCustomSize(471, Vector3(8.f / 120, 120.f / 120, 1));
	AddCustomSize(472, Vector3(8.f / 120, 8.f / 120, 1));
	AddCustomSize(473, Vector3(10.f / 120, 10.f / 120, 1));
	AddCustomSize(474, Vector3(16.f / 120, 8.f / 120, 1));
	AddCustomSize(475, Vector3(120.f / 120, 6.f / 120, 1));
	AddCustomSize(661, Vector3(60.f / 120, 60.f / 120, 1));
	AddCustomSize(662, Vector3(120.f / 120, 60.f / 120, 1));
	AddCustomSize(663, Vector3(120.f / 120, 60.f / 120, 1));
	AddCustomSize(664, Vector3(120.f / 120, 60.f / 120, 1));
	AddCustomSize(1154, Vector3(60.f / 120, 6.f / 120, 1));
	AddCustomSize(1155, Vector3(60.f / 120, 60.f / 120, 1));
	AddCustomSize(1156, Vector3(60.f / 120, 60.f / 120, 1));
	AddCustomSize(1157, Vector3(8.f / 120, 60.f / 120, 1));
	AddCustomSize(1158, Vector3(8.f / 120, 8.f / 120, 1));
	AddCustomSize(1202, Vector3(120.f / 120, 12.f / 120, 1));
	AddCustomSize(1203, Vector3(120.f / 120, 14.f / 120, 1));
	AddCustomSize(1204, Vector3(120.f / 120, 14.f / 120, 1));
	AddCustomSize(1205, Vector3(14.f / 120, 14.f / 120, 1));
	AddCustomSize(1206, Vector3(14.f / 120, 14.f / 120, 1));
	AddCustomSize(1207, Vector3(26.f / 120, 14.f / 120, 1));
	AddCustomSize(1208, Vector3(60.f / 120, 14.f / 120, 1));
	AddCustomSize(1209, Vector3(120.f / 120, 14.f / 120, 1));
	AddCustomSize(1210, Vector3(120.f / 120, 14.f / 120, 1));
	AddCustomSize(1220, Vector3(120.f / 120, 24.f / 120, 1));
	AddCustomSize(1221, Vector3(120.f / 120, 26.f / 120, 1));
	AddCustomSize(1222, Vector3(120.f / 120, 26.f / 120, 1));
	AddCustomSize(1223, Vector3(26.f / 120, 26.f / 120, 1));
	AddCustomSize(1224, Vector3(26.f / 120, 26.f / 120, 1));
	AddCustomSize(1225, Vector3(50.f / 120, 16.f / 120, 1));
	AddCustomSize(1226, Vector3(120.f / 120, 26.f / 120, 1));
	AddCustomSize(1227, Vector3(120.f / 120, 26.f / 120, 1));
	AddCustomSize(1260, Vector3(120.f / 120, 6.f / 120, 1));
	AddCustomSize(1261, Vector3(10.f / 120, 10.f / 120, 1));
	AddCustomSize(1262, Vector3(120.f / 120, 12.f / 120, 1));
	AddCustomSize(1263, Vector3(16.f / 120, 16.f / 120, 1));
	AddCustomSize(1264, Vector3(120.f / 120, 24.f / 120, 1));
	AddCustomSize(1265, Vector3(29.f / 120, 29.f / 120, 1));
	AddCustomSize(1339, Vector3(240.f / 120, 120.f / 120, 1));
	//|
	//  I'm going to punt Robtop to the moon
	//  For how he renders these Outline Blocks
	//  - B
	//|
	//- Decals		[Various]
	//|
	AddCustomSize(18, Vector3(512.f / 120,166.f / 120,1));
	AddCustomSize(19, Vector3(416.f / 120,146.f / 120,1));
	AddCustomSize(20, Vector3(292.f / 120,116.f / 120,1));
	AddCustomSize(21, Vector3(168.f / 120,52.f / 120,1));
	AddCustomSize(41, Vector3(78.f / 120, 278.f / 120, 1));
	AddCustomSize(48, Vector3(460.f / 120, 154.f / 120, 1));
	AddCustomSize(49, Vector3(320.f / 120, 128.f / 120, 1));
	AddCustomSize(106, Vector3(108.f / 120, 262.f / 120, 1));
	AddCustomSize(107, Vector3(108.f / 120, 152.f / 120, 1));
	AddCustomSize(110, Vector3(78.f / 120, 136.f / 120, 1));
	AddCustomSize(113, Vector3(498.f / 120, 158.f / 120, 1));
	AddCustomSize(114, Vector3(344.f / 120, 134.f / 120, 1));
	AddCustomSize(115, Vector3(222.f / 120, 106.f / 120, 1));
	//|
	//- Rods		[rod_##_001.png]
	//|
	AddCustomSize(15, Vector3(0.21 * 1.5, 1.33 * 1.5, 1));
	AddCustomSize(16, Vector3(0.2 * 1.5, 0.88 * 1.5, 1));
	AddCustomSize(17, Vector3(0.18 * 1.5, 0.41 * 1.5, 1));
	//|
	//- Portals		[portal_XX_front_001.png]
	//|
	AddCustomSize(10, Vector3(50.f / 60, 150.f / 60, 1));//gravity down		(01)
	AddCustomSize(11, Vector3(50.f / 60, 150.f / 60, 1));//gravity up	    (02)
	AddCustomSize(12, Vector3(68.f / 60, 172.f / 60, 1));//cube				(03)
	AddCustomSize(13, Vector3(68.f / 60, 172.f / 60, 1));//ship				(04)
	AddCustomSize(45, Vector3(88.f / 60, 184.f / 60, 1));//mirror			(05)
	AddCustomSize(46, Vector3(88.f / 60, 184.f / 60, 1));//unmirror			(06)
	AddCustomSize(47, Vector3(68.f / 60, 172.f / 60, 1));//ball				(07)
	AddCustomSize(99, Vector3(62.f / 60, 180.f / 60, 1));//unmini			(08)
	AddCustomSize(101, Vector3(62.f / 60, 180.f / 60, 1));//mini			(09)
	AddCustomSize(111, Vector3(68.f / 60, 172.f / 60, 1));//ufo				(10)
	AddCustomSize(286, Vector3(82.f / 60, 182.f / 60, 1));//dual mode		(11)
	AddCustomSize(287, Vector3(82.f / 60, 182.f / 60, 1));//undual mode		(12)
	AddCustomSize(660, Vector3(68.f / 60, 172.f / 60, 1));//wave			(13)
	AddCustomSize(745, Vector3(68.f / 60, 172.f / 60, 1));//robot			(14)
	AddCustomSize(747, Vector3(78.f / 60, 180.f / 60, 1));//teleport in		(15)
	AddCustomSize(749, Vector3(78.f / 60, 180.f / 60, 1));//teleport out	(16)
	AddCustomSize(1331, Vector3(68.f / 60, 172.f / 60, 1));//robot			(17)
	//|
	//- differently-scaled blocks
	// what the fuck rob

	//No Collisions
	//|
	//|
	//- Decals		[Various]
	//|
	AddNoCollisions(18);
	AddNoCollisions(19);
	AddNoCollisions(20);
	AddNoCollisions(21);
	AddNoCollisions(41);
	AddNoCollisions(48);
	AddNoCollisions(49);
	AddNoCollisions(106);
	AddNoCollisions(107);
	AddNoCollisions(110);
	AddNoCollisions(113);
	AddNoCollisions(114);
	AddNoCollisions(115);
	//|
	//- Rods
	//|
	AddNoCollisions(15);
	AddNoCollisions(16);
	AddNoCollisions(17);

	//Custom Collision Size
	//|
	//|
	//- Spikes
	//|
	AddCustomCollisionSize(8, Vector3(0.2, 0.2, 1));
	AddCustomCollisionSize(39, Vector3(0.2, 0.2, 1));
	AddCustomCollisionSize(103, Vector3(0.2, 0.2, 1));
}

void GDLevelLoader::AddCustomSize(int id, Vector3 size)
{
	customSize.push_back(std::make_pair(id, size));
}

void GDLevelLoader::AddNoCollisions(int id)
{
	noCollisions.push_back(id);
}

void GDLevelLoader::AddCustomCollisionSize(int id, Vector3 size)
{
	customCollisionSize.push_back(std::make_pair(id, size));
}
