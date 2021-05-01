#include "future/SoundDevice.h"
#include "future/SoundBuffer.h"
#include "Sound2.h"

#include "future/CaptureDevice.h"
#include "future/CaptureBuffer.h"

#include "MicPlayback2.h"

#include "future/Tools.h"
#include "future/SaveFile.h"

#include <Marx.h>

#include <ShObjIdl.h>
#include <filesystem>

struct KeyCombo
{
	static constexpr uint32_t nMaxKeys = 4;

	uint32_t nKeys = 0;
	int keys[nMaxKeys];
};

struct LoadedSound
{
	std::string name;
	uint32_t internalID;
	Marx::Reference<Sound2> pSound;
	KeyCombo keyCombo;
	bool keyComboIsPressed = false;
public:
	void saveToSaveFile(SaveFile& sf)
	{
		auto main = "root.sounds." + std::to_string(internalID);
		auto prop = main + ".properties";
		auto effects = main + ".effects";

		sf.setVar<std::string>(prop, "name", name);
		sf.setVar<KeyCombo>(prop, "keyCombo", keyCombo);
		sf.setVar<std::string>(prop, "filepath", pSound->getFilepath());
		sf.setVar<float>(prop, "volume", pSound->getVolume());
		sf.setVar<bool>(prop, "looping", pSound->getLoopingEnabled());
		sf.setVar<bool>(prop, "useEffects", pSound->useEffects());

		auto pEl = pSound->getEffectList();
		for (auto type : *pEl)
		{
			auto pEff = pEl->getEffect(type);
			auto effChain = effects + effectTypeToString(pEff->getType());
			uint32_t i = 0;
			EffectParamDesc desc;
			while (pEff->getNextParam(i++, &desc))
			{
				switch (desc.type)
				{
				case EffectParamType::DWORD:
					sf.setVar<DWORD>(effChain, desc.internalName, *(DWORD*)desc.pValue); break;
				case EffectParamType::FLOAT:
					sf.setVar<FLOAT>(effChain, desc.internalName, *(FLOAT*)desc.pValue); break;
				case EffectParamType::LONG:
					sf.setVar<LONG>(effChain, desc.internalName, *(LONG*)desc.pValue); break;
				}
			}
		}
	}
	static Marx::Reference<LoadedSound> loadFromSaveFile(SaveFile& sf, const std::string& internalIDStr, SoundDevice* pDev1, SoundDevice* pDev2)
	{
		auto main = "root.sounds." + internalIDStr;
		auto prop = main + ".properties";
		auto effects = main + ".effects";

		auto ps = std::make_shared<LoadedSound>();
		ps->name = sf.getVar<std::string>(prop, "name", "None");
		ps->keyCombo = sf.getVar<KeyCombo>(prop, "keyCombo", KeyCombo());
		ps->internalID = (uint32_t)std::stoull(internalIDStr);

		try
		{
			ps->pSound = std::make_shared<Sound2>(pDev1, pDev2, sf.getVar<std::string>(prop, "filepath", "Unknown"));
		}
		catch (std::exception& e)
		{
			MX_ERROR("{0}", e.what());
			return nullptr;
		}
		ps->pSound->setVolume(sf.getVar<float>(prop, "volume", 1.0f));
		ps->pSound->enableLooping(sf.getVar<bool>(prop, "looping", false));
		ps->pSound->useEffects() = sf.getVar<bool>(prop, "useEffects", false);
		for (auto& eff : sf.getSubBlockNames(effects))
		{
			auto pEff = Effect::create(effectStringToType(eff));
			auto effChain = effects + eff;
			uint32_t i = 0;
			EffectParamDesc desc;
			while (pEff->getNextParam(i++, &desc))
			{
				switch (desc.type)
				{
				case EffectParamType::DWORD:
					*(DWORD*)desc.pValue = sf.getVar<DWORD>(effChain, desc.internalName, *(DWORD*)desc.pValue); break;
				case EffectParamType::FLOAT:
					*(FLOAT*)desc.pValue = sf.getVar<FLOAT>(effChain, desc.internalName, *(FLOAT*)desc.pValue); break;
				case EffectParamType::LONG:
					*(LONG*)desc.pValue = sf.getVar<LONG>(effChain, desc.internalName, *(LONG*)desc.pValue); break;
				}
			}
			ps->pSound->addEffect(pEff);
		}

		ps->pSound->applyEffects();

		return ps;
	}
};

namespace MyImGui
{
	bool InputText(const char* label, std::string* pStr)
	{
		ImGuiInputTextCallback callback = [](ImGuiInputTextCallbackData* data) -> int
		{
			if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
			{
				std::string* str = (std::string*)data->UserData;
				IM_ASSERT(data->Buf == str->c_str());
				str->resize(data->BufTextLen);
				data->Buf = (char*)str->c_str();
			}

			return 0;
		};
		return ImGui::InputText(label, (char*)pStr->c_str(), pStr->size() + 1, ImGuiInputTextFlags_CallbackResize, callback, (void*)pStr);
	}

	static void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
}

std::string virtualKeyToString(int virtualKey)
{
	UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

	CHAR szName[128];
	int result = 0;

	result = GetKeyNameTextA(scanCode << 16, szName, 128);

	DH_ASSERT(result != 0);

	return szName;
}

std::string openFileDialog()
{
	DH_DEBUG_HR_DECL;

	DH_ASSERT_HR(
		CoInitializeEx(
			NULL,
			COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
		)
	);

	ComPtr<IFileOpenDialog> pFileOpen;

	DH_ASSERT_HR(
		CoCreateInstance(
			CLSID_FileOpenDialog,
			NULL, CLSCTX_ALL,
			IID_IFileOpenDialog,
			(void**)pFileOpen.GetAddressOf()
		)
	);

	constexpr bool a = ERROR_CANCELLED == 0x800704c7;

	switch (hr = pFileOpen->Show(NULL))
	{
	case S_OK:
		break;
	case HRESULT_FROM_WIN32(ERROR_CANCELLED):
		return "";
	default:
		DH_ASSERT_HR(hr);
		break;
	}

	ComPtr<IShellItem> pItem;
	DH_ASSERT_HR(
		pFileOpen->GetResult(
			pItem.GetAddressOf()
		)
	);

	PWSTR pszFilePath;
	DH_ASSERT_HR(
		pItem->GetDisplayName(
			SIGDN_FILESYSPATH,
			&pszFilePath
		)
	);

	char cStr[MAX_PATH];
	size_t convChars = 0;
	wcstombs_s(&convChars, cStr, MAX_PATH, pszFilePath, MAX_PATH);

	CoTaskMemFree(pszFilePath);

	CoUninitialize();

	return cStr;
}

class MainLayer : public Marx::Layer
{
public:
	MainLayer()
		: Layer("MainLayer")
	{
		Marx::RenderCommand::setClearColor(0.05f, 0.05f, 0.05f, 1.0f);

		{
			m_background.pShader = Marx::Shader::create("assets/shaders/background.hlsl");

			struct Vertex { float x; float y; float u; float v; } pVertices[] = {
				{-1.0f, -1.0f, 0.0f, 0.0f},
				{-1.0f,  1.0f, 0.0f, 1.0f},
				{ 1.0f,  1.0f, 1.0f, 1.0f},
				{ 1.0f, -1.0f, 1.0f, 0.0f}
			};
			auto pVertexBuffer = Marx::VertexBuffer::create(pVertices, sizeof(pVertices));
			pVertexBuffer->setLayout({
				{ Marx::ShaderDataType::Float2, "A_POSITION"},
				{ Marx::ShaderDataType::Float2, "A_TEXCOORD" }
				}
			);

			uint32_t pIndices[] = { 0, 1, 2, 2, 3, 0 };
			auto pIndexBuffer = Marx::IndexBuffer::create(pIndices, sizeof(pIndices) / sizeof(uint32_t), Marx::IndexBuffer::PrimitiveType::TriangleList);

			m_background.pVertexArray = Marx::VertexArray::create(pVertexBuffer, pIndexBuffer);
		}

		{
			auto deviceSaveFile = SaveFile::loadFromFile("savefiles/devices.dhsf");
			if (deviceSaveFile)
			{
				if (deviceSaveFile->hasVar("root.devices.microphone", "guid"))
				{
					try
					{
						setNewCaptureDevice(
							std::make_shared<CaptureDevice>(
								deviceSaveFile->getVar<GUID>("root.devices.microphone", "guid", GUID()),
								deviceSaveFile->getVar<std::string>("root.devices.microphone", "description", "Last Microphone")
								)
						);
					}
					catch (std::exception& e)
					{
						MX_ERROR("{0}", e.what());
					}
				}
				if (deviceSaveFile->hasVar("root.devices.main-out", "guid"))
				{
					try
					{
						setNewMainSoundDevice(
							std::make_shared<SoundDevice>(
								(HWND)(Marx::Application::get()->getWindow()->getNativeWindow()),
								deviceSaveFile->getVar<GUID>("root.devices.main-out", "guid", GUID()),
								deviceSaveFile->getVar<std::string>("root.devices.main-out", "description", "Last Main Output")
								)
						);
					}
					catch (std::exception& e)
					{
						MX_ERROR("{0}", e.what());
					}
				}
				if (deviceSaveFile->hasVar("root.devices.echo-out", "guid"))
				{
					try
					{
						setNewEchoSoundDevice(
							std::make_shared<SoundDevice>(
								(HWND)(Marx::Application::get()->getWindow()->getNativeWindow()),
								deviceSaveFile->getVar<GUID>("root.devices.echo-out", "guid", GUID()),
								deviceSaveFile->getVar<std::string>("root.devices.echo-out", "description", "Last Echo Output")
								)
						);
					}
					catch (std::exception& e)
					{
						MX_ERROR("{0}", e.what());
					}
				}
			}
		}

		{
			auto settingsSaveFile = SaveFile::loadFromFile("savefiles/settings.dhsf");
			if (settingsSaveFile)
			{
				if (m_pMicPlayback)
				{
					m_pMicPlayback->setVolume(settingsSaveFile->getVar<float>("root.settings.microphone.volumes", "master", 1.0f));
					m_pMicPlayback->setVolumeMultiplier1(settingsSaveFile->getVar<float>("root.settings.microphone.volumes", "main", 1.0f));
					m_pMicPlayback->setVolumeMultiplier2(settingsSaveFile->getVar<float>("root.settings.microphone.volumes", "echo", 1.0f));
				}

				m_soundVolumes.master = settingsSaveFile->getVar<float>("root.settings.sound.volumes", "master", 1.0f);
				m_soundVolumes.mainMultiplier = settingsSaveFile->getVar<float>("root.settings.sound.volumes", "main", 1.0f);
				m_soundVolumes.echoMultiplier = settingsSaveFile->getVar<float>("root.settings.sound.volumes", "echo", 1.0f);

				m_background.color[0] = settingsSaveFile->getVar<float>("root.settings.background.color", "red", 0.05f);
				m_background.color[1] = settingsSaveFile->getVar<float>("root.settings.background.color", "green", 0.05f);
				m_background.color[2] = settingsSaveFile->getVar<float>("root.settings.background.color", "blue", 0.05f);
				updateBackgroundColor();
				updateBackgroundBrightness(settingsSaveFile->getVar<float>("root.settings.background", "brightness", 0.5f));
				m_background.imagePath = settingsSaveFile->getVar<std::string>("root.settings.background", "imagePath", "");
				if (!m_background.imagePath.empty())
				{
					m_background.pImage = Marx::Texture2D::create(m_background.imagePath);
					if (m_background.pImage)
						m_background.imageAspectRatio = (float)m_background.pImage->getWidth() / (float)m_background.pImage->getHeight();
				}
				m_background.useImage = m_background.pImage ? settingsSaveFile->getVar<bool>("root.settings.background", "useImage", false) : false;
			}
		}

		{
			auto soundsSaveFile = SaveFile::loadFromFile("savefiles/sounds.dhsf");
			if (soundsSaveFile)
			{
				for (auto& internalIDStr : soundsSaveFile->getSubBlockNames("root.sounds"))
				{
					uint32_t newID = (uint32_t)std::stoull(internalIDStr);
					m_soundIDCounter = std::max(newID, m_soundIDCounter);
					auto pSound = LoadedSound::loadFromSaveFile(*soundsSaveFile, internalIDStr, m_pMainSoundDevice.get(), m_pEchoSoundDevice.get());
					if (pSound)
						m_sounds.push_back(pSound);
				}
				++m_soundIDCounter;
			}
		}

		updateCamera((float)Marx::Application::get()->getWindow()->getWidth() / (float)Marx::Application::get()->getWindow()->getHeight());
	}
	~MainLayer()
	{
		std::filesystem::create_directory("savefiles");
		{
			SaveFile sf;
			if (m_pCaptureDevice)
			{
				sf.setVar<GUID>("root.devices.microphone", "guid", *m_pCaptureDevice->getLPGuid());
				sf.setVar<std::string>("root.devices.microphone", "description", m_pCaptureDevice->getDescription());
			}
			if (m_pMainSoundDevice)
			{
				sf.setVar<GUID>("root.devices.main-out", "guid", *m_pMainSoundDevice->getLPGuid());
				sf.setVar<std::string>("root.devices.main-out", "description", m_pMainSoundDevice->getDescription());
			}
			if (m_pEchoSoundDevice)
			{
				sf.setVar<GUID>("root.devices.echo-out", "guid", *m_pEchoSoundDevice->getLPGuid());
				sf.setVar<std::string>("root.devices.echo-out", "description", m_pEchoSoundDevice->getDescription());
			}
			sf.saveToFile("savefiles/devices.dhsf");
		}

		{
			SaveFile sf;
			if (m_pMicPlayback)
			{
				sf.setVar<float>("root.settings.microphone.volumes", "master", m_pMicPlayback->getVolume());
				sf.setVar<float>("root.settings.microphone.volumes", "main", m_pMicPlayback->getVolumeMultiplier1());
				sf.setVar<float>("root.settings.microphone.volumes", "echo", m_pMicPlayback->getVolumeMultiplier2());
			}

			sf.setVar<float>("root.settings.sound.volumes", "master", m_soundVolumes.master);
			sf.setVar<float>("root.settings.sound.volumes", "main", m_soundVolumes.mainMultiplier);
			sf.setVar<float>("root.settings.sound.volumes", "echo", m_soundVolumes.echoMultiplier);

			sf.setVar<float>("root.settings.background.color", "red", m_background.color[0]);
			sf.setVar<float>("root.settings.background.color", "green", m_background.color[1]);
			sf.setVar<float>("root.settings.background.color", "blue", m_background.color[2]);
			sf.setVar<float>("root.settings.background", "brightness", m_background.brightness[0]);
			if (!m_background.imagePath.empty() && m_background.pImage)
				sf.setVar<std::string>("root.settings.background", "imagePath", m_background.imagePath);
			else
				m_background.useImage = false;
			sf.setVar<bool>("root.settings.background", "useImage", m_background.useImage);

			if (m_pMicPlayback)
			{
				sf.setVar<float>("root.settings.microphone.volumes", "master", m_pMicPlayback->getVolume());
				sf.setVar<float>("root.settings.microphone.volumes", "main", m_pMicPlayback->getVolumeMultiplier1());
				sf.setVar<float>("root.settings.microphone.volumes", "echo", m_pMicPlayback->getVolumeMultiplier2());
			}
			sf.saveToFile("savefiles/settings.dhsf");
		}
		{
			SaveFile sf;
			for (uint32_t i = 0; i < m_sounds.size(); ++i)
				m_sounds[i]->saveToSaveFile(sf);
			sf.saveToFile("savefiles/sounds.dhsf");
		}
	}
public:
	virtual void onUpdate(Marx::Timestep ts) override
	{
		Marx::RenderCommand::clear();

		if (m_background.useImage && m_background.pImage)
		{
			Marx::Renderer::beginScene(m_background.camera);

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			Marx::Renderer::submit(m_background.pShader, m_background.pVertexArray, transform, m_background.pImage);

			Marx::Renderer::endScene();
		}

		if (!m_editingKeyCombo)
		{
			for (auto pSound : m_sounds)
			{
				auto& keyCombo = pSound->keyCombo;
				bool comboIsPressed = keyCombo.nKeys != 0;
				for (uint32_t i = 0; i < keyCombo.nKeys; ++i)
				{
					if (!Marx::Input::isKeyPressed(keyCombo.keys[i]))
					{
						comboIsPressed = false;
						break;
					}
				}

				if (comboIsPressed && !pSound->keyComboIsPressed)
				{
					if (!pSound->pSound->isPlaying())
						pSound->pSound->start();
					else
						pSound->pSound->stop();
				}

				pSound->keyComboIsPressed = comboIsPressed;
			}
		}
	}
	virtual void onEvent(Marx::Event& event) override
	{
		Marx::EventDispatcher dispatcher(event);
		dispatcher.dispatch<Marx::KeyPressEvent>(MX_BIND_EVENT_METHOD(MainLayer::onKeyPress));
		dispatcher.dispatch<Marx::WindowResizeEvent>(MX_BIND_EVENT_METHOD(MainLayer::onWindowResize));
	}
	virtual void onImGuiRender() override
	{
		setupDockspace();
		renderDeviceSelector();
		renderSettings();
		renderLog();
		renderSoundList();
		renderSoundEffectConfigurator();
		renderSoundPropertyConfigurator();
	}
	bool onKeyPress(Marx::KeyPressEvent& e)
	{
		if (m_editingKeyCombo && m_pSelectedSound)
		{
			auto& keyCombo = m_pSelectedSound->keyCombo;
			bool foundKey = false;
			for (uint32_t i = 0; i < keyCombo.nKeys; ++i)
			{
				if (keyCombo.keys[i] == e.getKeyCode())
					foundKey = true;
				if (foundKey && i < keyCombo.nKeys - 1)
					keyCombo.keys[i] = keyCombo.keys[i + 1];
			}

			if (foundKey)
			{
				--keyCombo.nKeys;
			}
			else if (keyCombo.nKeys < keyCombo.nMaxKeys)
			{
				keyCombo.keys[keyCombo.nKeys] = e.getKeyCode();
				++keyCombo.nKeys;
			}
		}

		return true;
	}
	bool onWindowResize(Marx::WindowResizeEvent& e)
	{
		updateCamera((float)e.getWidth() / (float)e.getHeight());
		
		return true;
	}
	void updateCamera(float aspectRatio)
	{
		float ratio = aspectRatio / m_background.imageAspectRatio;
		if (aspectRatio < m_background.imageAspectRatio)
			m_background.camera = Marx::OrthographicCamera(-1.0f * ratio, 1.0f * ratio, -1.0f, 1.0f);
		else
			m_background.camera = Marx::OrthographicCamera(-1.0f, 1.0f, -1.0f / ratio, 1.0f / ratio);
	}
	void updateBackgroundBrightness(float brightness)
	{
		m_background.brightness[0] = brightness;
		m_background.brightness[1] = brightness;
		m_background.brightness[2] = brightness;
		m_background.brightness[3] = 1.0f;
		m_background.pShader->updateUniform("c_brightness", &m_background.brightness[0], Marx::ShaderDataType::Float4);
	}
private:
	void updateBackgroundColor()
	{
		Marx::RenderCommand::setClearColor(m_background.color[0], m_background.color[1], m_background.color[2], 1.0f);
	}
	const MicPlayback2Desc* getMicPlaybackDesc()
	{
		static MicPlayback2Desc desc;
		desc.pCaptureDev = m_pCaptureDevice.get();
		desc.pSoundDev[0] = m_pMainSoundDevice.get();
		desc.pSoundDev[1] = m_pEchoSoundDevice.get();
		desc.nBlocksPerUpdate = 2048;
		desc.nChannels = 2;
		desc.nBlocksPerSec = 44100;
		desc.nBytesPerSample = 2;
		desc.enableEffects = false;

		if (m_useMicEffects)
		{
			desc.enableEffects = true;
			desc.nBlocksPerUpdate = 6615;
		}
		return &desc;
	}
	void reinitMicPlayback(bool loadVolumesAndReset)
	{
		static float volume;
		static float volumeMultiplier1;
		static float volumeMultiplier2;

		if (loadVolumesAndReset)
		{
			volume = m_pMicPlayback ? m_pMicPlayback->getVolume() : 1.0f;
			volumeMultiplier1 = m_pMicPlayback ? m_pMicPlayback->getVolumeMultiplier1() : 1.0f;
			volumeMultiplier2 = m_pMicPlayback ? m_pMicPlayback->getVolumeMultiplier2() : 1.0f;

			m_pMicPlayback.reset();
		}
		else
		{
			if (m_pCaptureDevice && m_pMainSoundDevice && m_pEchoSoundDevice)
			{
				GUID guid = *m_pCaptureDevice->getLPGuid();
				std::string description = m_pCaptureDevice->getDescription();
				m_pCaptureDevice.reset();
				m_pCaptureDevice = std::make_shared<CaptureDevice>(guid, description);

				m_pMicPlayback = std::make_shared<MicPlayback2>(getMicPlaybackDesc());
				m_pMicPlayback->setVolume(volume);
				m_pMicPlayback->setVolumeMultiplier1(volumeMultiplier1);
				m_pMicPlayback->setVolumeMultiplier2(volumeMultiplier2);
			}
		}
	}
	void setNewCaptureDevice(Marx::Reference<CaptureDevice> pNewDevice)
	{
		reinitMicPlayback(true);
		m_pCaptureDevice = pNewDevice;
		reinitMicPlayback(false);
	}
	void setNewMainSoundDevice(Marx::Reference<SoundDevice> pNewDevice)
	{
		reinitMicPlayback(true);
		auto pOldDev = m_pMainSoundDevice;
		m_pMainSoundDevice = pNewDevice;
		reinitMicPlayback(false);

		for (auto pSound : m_sounds)
		{
			try
			{
				pSound->pSound->setNewDevice1(pNewDevice.get());
			}
			catch (std::exception& e)
			{
				MX_ERROR("{0}", e.what());
			}
		}
	}
	void setNewEchoSoundDevice(Marx::Reference<SoundDevice> pNewDevice)
	{
		reinitMicPlayback(true);
		auto pOldDev = m_pMainSoundDevice;
		m_pEchoSoundDevice = pNewDevice;
		reinitMicPlayback(false);

		for (auto pSound : m_sounds)
		{
			try
			{
				for (auto& sound : m_sounds)
					sound->pSound->setNewDevice2(pNewDevice.get());
			}
			catch (std::exception& e)
			{
				MX_ERROR("{0}", e.what());
			}
		}
	}
	void setupDockspace()
	{
		static ImGuiID dockspaceID = 0;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
		ImGuiDockNodeFlags dockspaceFlags = 0;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::Begin("Dockspace", nullptr, windowFlags);
		ImGui::PopStyleVar();
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			dockspaceID = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspaceID, { 0.0f, 0.0f }, dockspaceFlags);
		}
		ImGui::End();

		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
	}
	void renderDeviceSelector()
	{
		if (ImGui::Begin("Device Selector"))
		{
			{
				static bool alreadyOpen = false;
				static std::vector<CaptureDeviceEnum> availDevices;

				MyImGui::HelpMarker("The microphone that will record your beautiful voice.");
				ImGui::SameLine();
				if (ImGui::BeginCombo("Microphone", m_pCaptureDevice ? m_pCaptureDevice->getDescription().c_str() : nullptr))
				{
					if (!alreadyOpen)
					{
						alreadyOpen = true;
						availDevices = CaptureDevice::getAvailDevices();
					}

					for (uint32_t i = 0; i < availDevices.size(); ++i)
					{
						bool isSelected = (availDevices[i].guid == (m_pCaptureDevice ? *m_pCaptureDevice->getLPGuid() : GUID()));
						if (ImGui::Selectable(availDevices[i].strDescription.c_str(), isSelected))
						{
							setNewCaptureDevice(std::make_shared<CaptureDevice>(availDevices[i].guid, availDevices[i].strDescription));
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
			}

			{
				static bool alreadyOpen = false;
				static std::vector<SoundDeviceEnum> availDevices;

				MyImGui::HelpMarker("The output your friends will listen to. Usually a virtual cable like VB-Cable (www.vb-cable.com).");
				ImGui::SameLine();
				if (ImGui::BeginCombo("Main-Out", m_pMainSoundDevice ? m_pMainSoundDevice->getDescription().c_str() : nullptr))
				{
					if (!alreadyOpen)
					{
						alreadyOpen = true;
						availDevices = SoundDevice::getAvailDevices();
					}

					for (uint32_t i = 0; i < availDevices.size(); ++i)
					{
						bool isSelected = (availDevices[i].guid == (m_pMainSoundDevice ? *m_pMainSoundDevice->getLPGuid() : GUID()));
						if (ImGui::Selectable(availDevices[i].strDescription.c_str(), isSelected))
						{
							setNewMainSoundDevice(
								std::make_shared<SoundDevice>(
									(HWND)(Marx::Application::get()->getWindow()->getNativeWindow()),
									availDevices[i].guid,
									availDevices[i].strDescription
								)
							);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
			}

			{
				static bool alreadyOpen = false;
				static std::vector<SoundDeviceEnum> availDevices;

				MyImGui::HelpMarker("The output you will hear (like your own voice echo).");
				ImGui::SameLine();
				if (ImGui::BeginCombo("Echo-Out", m_pEchoSoundDevice ? m_pEchoSoundDevice->getDescription().c_str() : nullptr))
				{
					if (!alreadyOpen)
					{
						alreadyOpen = true;
						availDevices = SoundDevice::getAvailDevices();
					}

					for (uint32_t i = 0; i < availDevices.size(); ++i)
					{
						bool isSelected = (availDevices[i].guid == (m_pEchoSoundDevice ? *m_pEchoSoundDevice->getLPGuid() : GUID()));
						if (ImGui::Selectable(availDevices[i].strDescription.c_str(), isSelected))
						{
							setNewEchoSoundDevice(
								std::make_shared<SoundDevice>(
									(HWND)(Marx::Application::get()->getWindow()->getNativeWindow()),
									availDevices[i].guid,
									availDevices[i].strDescription
									)
							);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
			}

			bool isMainCapturing = m_pMicPlayback ? m_pMicPlayback->isCapturing() : false;
			bool isMainPlaying = m_pMicPlayback ? m_pMicPlayback->isPlaying() : false;
			ImGui::Checkbox("Mic Recording", &isMainCapturing);
			ImGui::Checkbox("Mic Playing", &isMainPlaying);
		}
		ImGui::End();
	}
	void renderSettings()
	{
		if (ImGui::Begin("Settings", nullptr))
		{
			if (ImGui::TreeNodeEx("Microphone", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (m_pMicPlayback)
				{
					{
						float oldVolume = m_pMicPlayback->getVolume();
						float newVolume = oldVolume;

						ImGui::SliderFloat("Master", &newVolume, 0.0f, 1.0f, "%.2f", 1.0f);

						if (oldVolume != newVolume)
							m_pMicPlayback->setVolume(newVolume);
					}
					{
						float oldVolume = m_pMicPlayback->getVolumeMultiplier1();
						float newVolume = oldVolume;

						ImGui::SliderFloat("Main", &newVolume, 0.0f, 1.0f, "%.2f", 1.0f);

						if (oldVolume != newVolume)
							m_pMicPlayback->setVolumeMultiplier1(newVolume);
					}
					{
						float oldVolume = m_pMicPlayback->getVolumeMultiplier2();
						float newVolume = oldVolume;

						ImGui::SliderFloat("Echo", &newVolume, 0.0f, 1.0f, "%.2f", 1.0f);

						if (oldVolume != newVolume)
							m_pMicPlayback->setVolumeMultiplier2(newVolume);
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNodeEx("Sound", ImGuiTreeNodeFlags_DefaultOpen))
			{
				{
					if (ImGui::SliderFloat("Master", &m_soundVolumes.master, 0.0f, 1.0f, "%.2f", 1.0f))
					{
						if (m_pMainSoundDevice) m_pMainSoundDevice->setVolume(m_soundVolumes.master * m_soundVolumes.mainMultiplier);
						if (m_pEchoSoundDevice) m_pEchoSoundDevice->setVolume(m_soundVolumes.master * m_soundVolumes.echoMultiplier);
					}
					if (m_pMainSoundDevice && ImGui::SliderFloat("Main", &m_soundVolumes.mainMultiplier, 0.0f, 1.0f, "%.2f", 1.0f))
						m_pMainSoundDevice->setVolume(m_soundVolumes.master * m_soundVolumes.mainMultiplier);
					if (m_pEchoSoundDevice && ImGui::SliderFloat("Echo", &m_soundVolumes.echoMultiplier, 0.0f, 1.0f, "%.2f", 1.0f))
						m_pEchoSoundDevice->setVolume(m_soundVolumes.master * m_soundVolumes.echoMultiplier);
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNodeEx("Background", ImGuiTreeNodeFlags_DefaultOpen))
			{
				{
					if (ImGui::ColorEdit3("Color", m_background.color))
						updateBackgroundColor();
					ImGui::Checkbox("Use Image", &m_background.useImage);
					if (m_background.useImage)
					{
						ImGui::SameLine();
						if (ImGui::Button("Load"))
						{
							std::string filepath = openFileDialog();
							if (!filepath.empty())
							{
								m_background.pImage = Marx::Texture2D::create(filepath);
								if (m_background.pImage)
								{
									m_background.imageAspectRatio = (float)m_background.pImage->getWidth() / (float)m_background.pImage->getHeight();
									m_background.imagePath = filepath;
									updateCamera((float)Marx::Application::get()->getWindow()->getWidth() / (float)Marx::Application::get()->getWindow()->getHeight());
								}
							}
						}
						if (ImGui::SliderFloat("Brightness", &m_background.brightness[0], 0.0f, 1.0f, "%.2f", 1.0f))
							updateBackgroundBrightness(m_background.brightness[0]);
					}
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
	void renderLog()
	{
		if (ImGui::Begin("Log", nullptr))
		{
			ImGui::Text("Log msg 1\nLog msg 2");
		}
		ImGui::End();
	}
	void renderSoundList()
	{
		if (ImGui::Begin("Sound List", 0, ImGuiWindowFlags_MenuBar))
		{
			{
				ImGui::BeginMenuBar();
				if (ImGui::MenuItem("Load"))
				{
					std::string filepath = openFileDialog();
					if (!filepath.empty())
					{
						auto newSound = std::make_shared<LoadedSound>();
						newSound->name = extractNameFromFilepath(filepath);
						newSound->internalID = m_soundIDCounter++;
						newSound->pSound = std::make_shared<Sound2>(m_pMainSoundDevice.get(), m_pEchoSoundDevice.get(), filepath);
						m_sounds.push_back(newSound);
					}
				}
				if (m_pSelectedSound && ImGui::MenuItem("Deselect"))
				{
					m_pSelectedSound = nullptr;
				}
				ImGui::EndMenuBar();
			}

			for (uint32_t i = 0; i < m_sounds.size(); ++i)
			{
				auto sound = m_sounds[i];

				bool isSelected = (m_pSelectedSound == sound.get());
				{
					ImGui::PushID(sound->pSound.get());
					if (!sound->pSound->isPlaying())
					{
						if (ImGui::Button("Play"))
						{
							sound->pSound->start();
							m_pSelectedSound = sound.get();
						}
					}
					else
					{
						if (ImGui::Button("Stop"))
							sound->pSound->stop();
					}
					ImGui::PopID();
				}
				ImGui::SameLine();
				if (ImGui::Selectable(sound->name.c_str(), isSelected))
					m_pSelectedSound = sound.get();
				if (isSelected)
					ImGui::SetItemDefaultFocus();

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::Button("Remove"))
					{
						m_sounds.erase(m_sounds.begin() + i);
						m_pSelectedSound = nullptr;
					}
					ImGui::EndPopup();
				}
			}
		}
		ImGui::End();
	}
	void renderSoundEffectConfigurator()
	{
		if (ImGui::Begin("Effect Configurator"))
		{
			if (m_pSelectedSound)
			{
				renderEffectListForSound();
			}
			else if (m_pMicPlayback)
			{
				renderEffectListForMic();
			}
			else
			{
				ImGui::Text("No sound selected.");
			}
		}

		renderAddEffectPopup();

		ImGui::End();
	}
	void renderEffectListForSound()
	{
		ImGui::Text("Effects for sound '%s'", m_pSelectedSound->name.c_str());

		if (!m_pSelectedSound->pSound->effectsEnabled())
		{
			ImGui::Text("Effects are disabled");
			ImGui::Text("for the selected sound.");
		}
		else
		{
			if (ImGui::Checkbox("Use Effects", &m_pSelectedSound->pSound->useEffects()))
				m_pSelectedSound->pSound->applyEffects();

			if (m_pSelectedSound->pSound->useEffects())
			{
				EffectList* pEl = m_pSelectedSound->pSound->getEffectList();
				uint32_t effectID = 0;
				for (EffectType effecX : *pEl)
				{
					bool effectChanged = true;
					Effect* pEffect = pEl->getEffect(effecX);

					bool effectRemoved = false;
					{
						ImGui::PushID(pEffect);
						if (ImGui::Button("X"))
						{
							m_pSelectedSound->pSound->removeEffect(effecX);
							effectRemoved = true;
						}
						ImGui::PopID();
					}

					if (effectRemoved)
						break;

					bool effectsRearranged = false;
					{
						ImGui::PushID(pEffect);
						ImGui::SameLine();
						if (ImGui::ArrowButton("##arrUp", ImGuiDir_Up) && effectID > 0)
						{
							m_pSelectedSound->pSound->swapEffects(effectID, effectID - 1);
							effectsRearranged = true;
						}
						ImGui::SameLine();
						if (ImGui::ArrowButton("##arrDown", ImGuiDir_Down) && effectID < pEl->size() - 1)
						{
							m_pSelectedSound->pSound->swapEffects(effectID, effectID + 1);
							effectsRearranged = true;
						}
						ImGui::PopID();
					}

					if (effectsRearranged)
						break;

					ImGui::SameLine();
					if (ImGui::TreeNodeEx(EffectTypeString(effecX), ImGuiTreeNodeFlags_DefaultOpen))
					{
						uint32_t i = 0;
						EffectParamDesc desc;

						while (pEffect->getNextParam(i++, &desc))
						{
							MyImGui::HelpMarker(desc.help);
							ImGui::SameLine();
							switch (desc.type)
							{
							case EffectParamType::FLOAT:
								if (ImGui::SliderFloat(desc.name, (float*)desc.pValue, *(float*)&desc.minVal, *(float*)&desc.maxVal, "%.3f", 1.0f))
									effectChanged = true;
								break;
							case EffectParamType::LONG:
								if (ImGui::SliderInt(desc.name, (int*)desc.pValue, *(int*)&desc.minVal, *(int*)&desc.maxVal))
									effectChanged = true;
								break;
							case EffectParamType::DWORD:
							{
								int value = *(DWORD*)desc.pValue;
								if (ImGui::SliderInt(desc.name, &value, *(DWORD*)&desc.minVal, *(DWORD*)&desc.maxVal))
								{
									*(DWORD*)desc.pValue = value;
									effectChanged = true;
								}
								break;
							}
							}
						}

						ImGui::TreePop();
					}

					if (effectChanged)
						m_pSelectedSound->pSound->updateEffect(effecX);

					++effectID;
				}
			}
		}
	}
	void renderEffectListForMic()
	{
		ImGui::Text("Effects for device 'microphone'.");

		if (ImGui::Checkbox("Use Effects", &m_useMicEffects))
		{
			reinitMicPlayback(true);
			reinitMicPlayback(false);
		}

		if (m_pMicPlayback->effectsEnabled())
		{
			EffectList* pEl = m_pMicPlayback->getEffectList();
			uint32_t effectID = 0;
			for (EffectType effecX : *pEl)
			{
				bool effectChanged = true;
				Effect* pEffect = pEl->getEffect(effecX);

				bool effectRemoved = false;
				{
					ImGui::PushID(pEffect);
					if (ImGui::Button("X"))
					{
						m_pMicPlayback->removeEffect(effecX);
						effectRemoved = true;
					}
					ImGui::PopID();
				}

				if (effectRemoved)
					break;

				bool effectsRearranged = false;
				{
					ImGui::PushID(pEffect);
					ImGui::SameLine();
					if (ImGui::ArrowButton("##arrUp", ImGuiDir_Up) && effectID > 0)
					{
						m_pMicPlayback->swapEffects(effectID, effectID - 1);
						effectsRearranged = true;
					}
					ImGui::SameLine();
					if (ImGui::ArrowButton("##arrDown", ImGuiDir_Down) && effectID < pEl->size() - 1)
					{
						m_pMicPlayback->swapEffects(effectID, effectID + 1);
						effectsRearranged = true;
					}
					ImGui::PopID();
				}

				if (effectsRearranged)
					break;

				ImGui::SameLine();
				if (ImGui::TreeNodeEx(EffectTypeString(effecX), ImGuiTreeNodeFlags_DefaultOpen))
				{
					uint32_t i = 0;
					EffectParamDesc desc;

					while (pEffect->getNextParam(i++, &desc))
					{
						MyImGui::HelpMarker(desc.help);
						ImGui::SameLine();
						switch (desc.type)
						{
						case EffectParamType::FLOAT:
							if (ImGui::SliderFloat(desc.name, (float*)desc.pValue, *(float*)&desc.minVal, *(float*)&desc.maxVal, "%.3f", 1.0f))
								effectChanged = true;
							break;
						case EffectParamType::LONG:
							if (ImGui::SliderInt(desc.name, (int*)desc.pValue, *(int*)&desc.minVal, *(int*)&desc.maxVal))
								effectChanged = true;
							break;
						case EffectParamType::DWORD:
						{
							int value = *(DWORD*)desc.pValue;
							if (ImGui::SliderInt(desc.name, &value, *(DWORD*)&desc.minVal, *(DWORD*)&desc.maxVal))
							{
								*(DWORD*)desc.pValue = value;
								effectChanged = true;
							}
							break;
						}
						}
					}

					ImGui::TreePop();
				}

				if (effectChanged)
					m_pMicPlayback->updateEffect(effecX);

				++effectID;
			}
		}
	}
	void renderAddEffectPopup()
	{
		if (m_pSelectedSound)
		{
			if (m_pSelectedSound->pSound->effectsEnabled() && m_pSelectedSound->pSound->useEffects() && ImGui::BeginPopupContextWindow())
			{
				for (uint32_t t = (EffectType)1; t < EffectType_Count; ++t)
				{
					std::string label = "Add " + effectTypeToString((EffectType)t);
					if (!m_pSelectedSound->pSound->hasEffect((EffectType)t) && ImGui::Button(label.c_str()))
						m_pSelectedSound->pSound->addEffect(Effect::create((EffectType)t));
				}

				ImGui::EndPopup();
			}
		}
		else if (m_pMicPlayback)
		{
			if (m_pMicPlayback->effectsEnabled() && ImGui::BeginPopupContextWindow())
			{
				for (uint32_t t = (EffectType)1; t < EffectType_Count; ++t)
				{
					std::string label = "Add " + effectTypeToString((EffectType)t);
					if (!m_pMicPlayback->hasEffect((EffectType)t) && ImGui::Button(label.c_str()))
						m_pMicPlayback->addEffect(Effect::create((EffectType)t));
				}

				ImGui::EndPopup();
			}
		}
	}
	void renderSoundPropertyConfigurator()
	{
		static LoadedSound* pLastSelectedSound = nullptr;

		if (ImGui::Begin("Property Configurator"))
		{
			if (m_pSelectedSound)
			{
				ImGui::Text("Filepath:           %s", m_pSelectedSound->pSound->getFilepath().c_str());
				{
					const char* sizeName;
					float size = convertBytes(m_pSelectedSound->pSound->getSize(), &sizeName);
					ImGui::Text("Size in memory:     %.2f %s", size, sizeName);
				}
				ImGui::Text("Number of channels: %d", m_pSelectedSound->pSound->getNumChannels());
				ImGui::Text("Samplerate:         %d", m_pSelectedSound->pSound->getSampleRate());
				ImGui::Text("Bits per sample:    %d", m_pSelectedSound->pSound->getBitsPerSample());
				ImGui::Text("Effects enabled:    %s", m_pSelectedSound->pSound->effectsEnabled() ? "Yes" : "No");

				{
					float oldVolume = m_pSelectedSound->pSound->getVolume();
					float newVolume = oldVolume;

					ImGui::SliderFloat("Volume", &newVolume, 0.0f, 1.0f, "%.2f", 1.0f);

					if (oldVolume != newVolume)
						m_pSelectedSound->pSound->setVolume(newVolume);
				}
				{
					bool oldLoop = m_pSelectedSound->pSound->getLoopingEnabled();
					bool newLoop = oldLoop;

					ImGui::Checkbox("Enable looping", &newLoop);

					if (oldLoop != newLoop)
						m_pSelectedSound->pSound->enableLooping(newLoop);

					ImGui::ProgressBar((float)m_pSelectedSound->pSound->getCurrBlock() / (float)m_pSelectedSound->pSound->getBlockCount());
				}

				{
					ImGui::Checkbox("Edit key combo", &m_editingKeyCombo);

					std::string keyComboString;
					for (uint32_t i = 0; i < m_pSelectedSound->keyCombo.nKeys; ++i)
					{
						
						keyComboString.append(virtualKeyToString(m_pSelectedSound->keyCombo.keys[i]));

						if (i < m_pSelectedSound->keyCombo.nKeys - 1)
							keyComboString.append(" + ");
					}
					ImGui::Text("Key combo: %s", keyComboString.c_str());
				}
			}
			else
			{
				ImGui::Text("No sound selected.");
			}
			pLastSelectedSound = m_pSelectedSound;
		}
		ImGui::End();
	}
private:
	Marx::Reference<CaptureDevice> m_pCaptureDevice;
	Marx::Reference<SoundDevice> m_pMainSoundDevice;
	Marx::Reference<SoundDevice> m_pEchoSoundDevice;
	Marx::Reference<MicPlayback2> m_pMicPlayback;
	struct SoundVolumes
	{
		float master = 1.0f;
		float mainMultiplier = 1.0f;
		float echoMultiplier = 1.0f;
	} m_soundVolumes;
	bool m_useMicEffects = false;
	bool m_editingKeyCombo = false;
private:
	std::vector<Marx::Reference<LoadedSound>> m_sounds;
	LoadedSound* m_pSelectedSound = 0;
	uint32_t m_soundIDCounter = 0;
	struct Background
	{
		bool useImage = false;
		float color[3] = { 0.05f, 0.05f, 0.05f };
		float imageAspectRatio = 1.0f;
		float brightness[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
		Marx::Reference<Marx::Texture2D> pImage;
		Marx::Reference<Marx::Shader> pShader;
		Marx::Reference<Marx::VertexArray> pVertexArray;
		Marx::OrthographicCamera camera = Marx::OrthographicCamera(-1.0f, 1.0f, -1.0f, 1.0f);
		std::string imagePath;
	} m_background;
};

class Application : public Marx::Application
{
public:
	Application()
	{
		getWindow()->enableImGuiFallthrough(true);
		getWindow()->setTitle("DonHugo");
		HRESULT hr = CoInitialize(nullptr);
		pushLayer(new MainLayer());
	}
	~Application()
	{
		CoUninitialize();
	}
};

Marx::Application* Marx::createApplication()
{
	return new ::Application();
}