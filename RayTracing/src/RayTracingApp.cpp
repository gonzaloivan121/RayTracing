#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "RayTracingLayer.h"

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";
	// https://tomeko.net/online_tools/file_to_hex.php?lang=en
	spec.IconPath = "resources/logo.png";
	spec.CustomTitlebar = true;
	spec.CenterWindow = true;

	Walnut::Application* app = new Walnut::Application(spec);
	std::shared_ptr<RayTracingLayer> rayTracingLayer = std::make_shared<RayTracingLayer>();
	app->PushLayer(rayTracingLayer);
	app->SetMenubarCallback([app, rayTracingLayer]() {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
				rayTracingLayer->ShowNewSceneModal();
			}
			if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
				rayTracingLayer->SaveScene();
			}
			if (ImGui::BeginMenu("Load Scene")) {
				if (ImGui::MenuItem("Default Scene")) {
					rayTracingLayer->LoadDefaultScene();
				}

				ImGui::Separator();

				auto& allScenes = rayTracingLayer->GetAllScenes();

				for (auto& scene : allScenes) {
					if (ImGui::MenuItem(scene.c_str())) {
						rayTracingLayer->LoadScene(scene);
					}
				}

				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) {
				if (!rayTracingLayer->AreThereUnsavedChanges()) {
					app->Close();
				} else {
					rayTracingLayer->ShowCloseConfirmationModal();
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			if (ImGui::MenuItem("Show Scene", NULL, rayTracingLayer->IsScenePanelShown())) {
				rayTracingLayer->ToggleScenePanel();
			}

			if (ImGui::MenuItem("Show Settings", NULL, rayTracingLayer->IsSettingsPanelShown())) {
				rayTracingLayer->ToggleSettingsPanel();
			}

			if (ImGui::MenuItem("Show Stats", NULL, rayTracingLayer->IsStatsPanelShown())) {
				rayTracingLayer->ToggleStatsPanel();
			}

			if (ImGui::MenuItem("Show Viewport", NULL, rayTracingLayer->IsViewportPanelShown())) {
				rayTracingLayer->ToggleViewportPanel();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Export")) {
			if (ImGui::MenuItem("Image")) {
				rayTracingLayer->ExportImage();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help")) {
			if (ImGui::MenuItem("About")) {
				rayTracingLayer->ShowAboutModal();
			}

			if (ImGui::MenuItem("Controls")) {
				rayTracingLayer->ShowControlsModal();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}