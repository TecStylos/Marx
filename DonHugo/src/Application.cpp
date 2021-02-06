#include "future/SoundDevice.h"
#include "future/SoundBuffer.h"
#include "Sound2.h"

#include "future/CaptureDevice.h"
#include "future/CaptureBuffer.h"

#include "MicPlayback2.h"

#include "future/Tools.h"

#include "Effects/Chorus.h"
#include "Effects/Compressor.h"
#include "Effects/Distortion.h"
#include "Effects/Echo.h"
#include "Effects/Flanger.h"
#include "Effects/Gargle.h"
#include "Effects/Equalizer.h"
#include "Effects/Reverb.h"

#include <Marx.h>

#include <ShObjIdl.h>

struct KeyCombo
{
	static constexpr uint32_t nMaxKeys = 4;

	uint32_t nKeys = 0;
	int keys[nMaxKeys];
};

struct LoadedSound
{
	std::string name;
	Marx::Reference<Sound2> pSound;
	KeyCombo keyCombo;
	bool keyComboIsPressed = false;
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
}

std::string virtualKeyToString(int virtualKey)
{
	UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

	CHAR szName[128];
	int result = 0;
	/*switch (virtualKey)
	{
	case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
	case VK_RCONTROL: case VK_RMENU:
	case VK_LWIN: case VK_RWIN: case VK_APPS:
	case VK_PRIOR: case VK_NEXT:
	case VK_END: case VK_HOME:
	case VK_INSERT: case VK_DELETE:
	case VK_DIVIDE:
	case VK_NUMLOCK:
		scanCode |= KF_EXTENDED;
	default:
		result = GetKeyNameTextA(scanCode << 16, szName, 128);
	}*/

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
		Marx::RenderCommand::setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}
	~MainLayer() = default;
public:
	virtual void onUpdate(Marx::Timestep ts) override
	{
		Marx::RenderCommand::clear();

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
private:
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
		m_pMainSoundDevice = pNewDevice;
		reinitMicPlayback(false);

		for (auto& sound : m_sounds)
			sound->pSound->setNewDevice1(pNewDevice.get());
	}
	void setNewEchoSoundDevice(Marx::Reference<SoundDevice> pNewDevice)
	{
		reinitMicPlayback(true);
		m_pEchoSoundDevice = pNewDevice;
		reinitMicPlayback(false);

		for (auto& sound : m_sounds)
			sound->pSound->setNewDevice2(pNewDevice.get());
	}
	void setupDockspace()
	{
		static ImGuiID dockspaceID = 0;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		ImGuiDockNodeFlags dockspaceFlags = 0;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
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
				for (EffectType effectType : *pEl)
				{
					bool effectChanged = true;
					Effect* pEffect = pEl->getEffect(effectType);

					bool effectRemoved = false;
					{
						ImGui::PushID(pEffect);
						if (ImGui::Button("X"))
						{
							m_pSelectedSound->pSound->removeEffect(effectType);
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
					if (ImGui::TreeNodeEx(EffectTypeString(effectType), ImGuiTreeNodeFlags_DefaultOpen))
					{
						uint32_t i = 0;
						EffectParamDesc desc;

						while (pEffect->getNextParam(i++, &desc))
						{
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
						m_pSelectedSound->pSound->updateEffect(effectType);

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
			for (EffectType effectType : *pEl)
			{
				bool effectChanged = true;
				Effect* pEffect = pEl->getEffect(effectType);

				bool effectRemoved = false;
				{
					ImGui::PushID(pEffect);
					if (ImGui::Button("X"))
					{
						m_pMicPlayback->removeEffect(effectType);
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
				if (ImGui::TreeNodeEx(EffectTypeString(effectType), ImGuiTreeNodeFlags_DefaultOpen))
				{
					uint32_t i = 0;
					EffectParamDesc desc;

					while (pEffect->getNextParam(i++, &desc))
					{
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
					m_pMicPlayback->updateEffect(effectType);

				++effectID;
			}
		}
	}
	void renderAddEffectPopup()
	{
		if (m_pSelectedSound)
		{
			if (m_pSelectedSound->pSound->effectsEnabled() && ImGui::BeginPopupContextWindow())
			{
				if (!m_pSelectedSound->pSound->hasEffect(EffectType_Chorus) && ImGui::Button("Add Chorus"))
					m_pSelectedSound->pSound->addEffect(std::make_shared<EffectChorus>());

				if (!m_pSelectedSound->pSound->hasEffect(EffectType_Compressor) && ImGui::Button("Add Compressor"))
					m_pSelectedSound->pSound->addEffect(std::make_shared<EffectCompressor>());

				if (!m_pSelectedSound->pSound->hasEffect(EffectType_Distortion) && ImGui::Button("Add Distortion"))
					m_pSelectedSound->pSound->addEffect(std::make_shared<EffectDistortion>());

				if (!m_pSelectedSound->pSound->hasEffect(EffectType_Echo) && ImGui::Button("Add Echo"))
					m_pSelectedSound->pSound->addEffect(std::make_shared<EffectEcho>());

				if (!m_pSelectedSound->pSound->hasEffect(EffectType_Flanger) && ImGui::Button("Add Flanger"))
					m_pSelectedSound->pSound->addEffect(std::make_shared<EffectFlanger>());

				if (!m_pSelectedSound->pSound->hasEffect(EffectType_Gargle) && ImGui::Button("Add Gargle"))
					m_pSelectedSound->pSound->addEffect(std::make_shared<EffectGargle>());

				if (!m_pSelectedSound->pSound->hasEffect(EffectType_Equalizer) && ImGui::Button("Add Equalizer"))
					m_pSelectedSound->pSound->addEffect(std::make_shared<EffectEqualizer>());

				if (!m_pSelectedSound->pSound->hasEffect(EffectType_Reverb) && ImGui::Button("Add Reverb"))
					m_pSelectedSound->pSound->addEffect(std::make_shared<EffectReverb>());

				ImGui::EndPopup();
			}
		}
		else if (m_pMicPlayback)
		{
			if (m_pMicPlayback->effectsEnabled() && ImGui::BeginPopupContextWindow())
			{
				if (!m_pMicPlayback->hasEffect(EffectType_Chorus) && ImGui::Button("Add Chorus"))
					m_pMicPlayback->addEffect(std::make_shared<EffectChorus>());

				if (!m_pMicPlayback->hasEffect(EffectType_Compressor) && ImGui::Button("Add Compressor"))
					m_pMicPlayback->addEffect(std::make_shared<EffectCompressor>());

				if (!m_pMicPlayback->hasEffect(EffectType_Distortion) && ImGui::Button("Add Distortion"))
					m_pMicPlayback->addEffect(std::make_shared<EffectDistortion>());

				if (!m_pMicPlayback->hasEffect(EffectType_Echo) && ImGui::Button("Add Echo"))
					m_pMicPlayback->addEffect(std::make_shared<EffectEcho>());

				if (!m_pMicPlayback->hasEffect(EffectType_Flanger) && ImGui::Button("Add Flanger"))
					m_pMicPlayback->addEffect(std::make_shared<EffectFlanger>());

				if (!m_pMicPlayback->hasEffect(EffectType_Gargle) && ImGui::Button("Add Gargle"))
					m_pMicPlayback->addEffect(std::make_shared<EffectGargle>());

				if (!m_pMicPlayback->hasEffect(EffectType_Equalizer) && ImGui::Button("Add Equalizer"))
					m_pMicPlayback->addEffect(std::make_shared<EffectEqualizer>());

				if (!m_pMicPlayback->hasEffect(EffectType_Reverb) && ImGui::Button("Add Reverb"))
					m_pMicPlayback->addEffect(std::make_shared<EffectReverb>());

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
};

class Application : public Marx::Application
{
public:
	Application()
	{
		getWindow()->enableImGuiFallthrough(true);
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