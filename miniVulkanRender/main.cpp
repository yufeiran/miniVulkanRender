#include"miniVulkanRender.h"

bool isAutoRotate = true;
float rotateAng = 0.0f;
glm::mat4 modelMat;
glm::mat4 modelMat1;

glm::vec3 eye = glm::vec3(2.0f, 2.0f, 2.0f);
glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

glm::vec3 front = glm::normalize(center - eye);

glm::mat4 viewMat;


void miniVulkanRender::mainLoop() {


	bool myWindow = true;

	float cameraStepSize = 0.0f;


	std::vector<float>historyFPS;
	while (!glfwWindowShouldClose(window)) {

		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		glfwPollEvents();

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		//ImGui::ShowDemoWindow();

		{
			ImGui::Begin("Config");

			ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			historyFPS.push_back(ImGui::GetIO().Framerate);
			ImGui::PlotHistogram("All FPS", historyFPS.data(), historyFPS.size());
			if (historyFPS.size() > 10000) {
				historyFPS.erase(historyFPS.begin());
			}
			ImGui::Checkbox("auto rotate", &isAutoRotate);



			ImGui::SliderAngle("rotate ang", &rotateAng, 0.0f, 360.0f);

			ImGui::SliderFloat("camera distance", &cameraStepSize, -3.0, 5.0);

			if (isAutoRotate == true) {
				rotateAng = time * glm::radians(90.0f);


			}
			while (rotateAng > glm::radians(360.0)) {
				rotateAng -= glm::radians(360.0);
			}


			modelMat = glm::rotate(glm::mat4(1.0f), rotateAng,
				glm::vec3(0.0f, 0.0f, 1.0f));

			modelMat1 = modelMat;
			modelMat1 = glm::translate(modelMat1, { 3.0, 0.0, 0.0 });

			ImGui::End();
		}

		eye = rawEye - cameraStepSize * front;

		viewMat = glm::lookAt(eye, center, up);

		drawFrame();



	}
	vkDeviceWaitIdle(device);
}

int main() {
	miniVulkanRender render;

	

	try {
		render.initWindow();
		render.initVulkan();
		render.initImgui();
		render.mainLoop();



		render.cleanup();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}