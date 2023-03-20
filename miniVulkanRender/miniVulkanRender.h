#pragma once
#include<glad/glad.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include<GLFW/glfw3native.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/hash.hpp>

#include<chrono>

#include<iostream>
#include<stdexcept>
#include<cstdlib>
#include<vector>
#include<cstring>
#include<map>
#include<optional>
#include<set>
#include<cstdint>
#include<limits>
#include<algorithm>
#include<fstream>
#include<array>
#include<unordered_map>



#include<imgui.h>
#include<imgui_impl_glfw.h>
#include<imgui_impl_vulkan.h>


const uint32_t WIDTH = 1280;
const uint32_t HEIGHT = 960;

const std::string MODEL_PATH = "../models/viking_room.obj";
const std::string TEXTURE_PATH = "../textures/viking_room.png";

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};
const std::vector<const char*>deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};




#ifdef NDEBUG
const bool enableValidationLayers = false;
#else 
const bool enableValidationLayers = true;
#endif

static ImGui_ImplVulkanH_Window g_MainWindowData;

extern bool isAutoRotate;
extern float rotateAng;
extern glm::mat4 modelMat;
extern glm::mat4 modelMat1;

const glm::vec3 rawEye = glm::vec3(2.0f, 2.0f, 2.0f);
extern glm::vec3 eye;
extern glm::vec3 center;
extern glm::vec3 up;

extern glm::vec3 front;

extern glm::mat4 viewMat;


struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;
	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3>getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}

	bool operator==(const Vertex& other)const {
		return (pos == other.pos) && (color == other.color) && texCoord == other.texCoord;
	}
};

namespace std {
	template<>struct hash<Vertex> {
		size_t operator()(Vertex const& vertex)const {
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

struct Model {
	glm::mat4 model;
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);


struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}


};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR>presentModes;
};


class miniVulkanRender {
public:
	void run() {
		initWindow();
		initVulkan();
		initImgui();
		mainLoop();
		cleanup();
	}

	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);


	}

	void initVulkan() {
		createInstance();
		setupDebugMessenger();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		createRenderPass();
		createDescriptorSetLayout();
		createGraphicsPipeline();
		createCommandPool();
		createColorResources();
		createDepthResources();
		createFramebuffers();
		createTextureImage();
		createTextureImageView();
		createTextureSampler();
		loadModel();
		createVertexBuffer();
		createIndexBuffer();
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets();
		createCommandBuffers();
		createSyncObjects();
	}

	void initImgui();	
	void mainLoop();
	void cleanup();

	GLFWwindow* window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkSurfaceKHR surface;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout piplineLayout;
	VkPipeline graphicsPipeline;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	bool framebufferResized = false;
	uint32_t currentFrame = 0;

	std::vector<Vertex>vertices;
	std::vector<uint32_t>indices;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;



	uint32_t mipLevels;
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;



	std::vector<VkBuffer>uniformBuffers;
	std::vector<VkDeviceMemory>uniformBuffersMemory;
	std::vector<void*>uniformBuffersMapped;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	//for MSAA
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

	//for imgui
	VkDescriptorPool imguiPool;
	ImGuiIO imguiIO;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	std::vector<Model>models;

	


	void cleanupImGui();



	void recreateSwapChain();

	void cleanupSwapChain();

	

	void createInstance();


	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	void setupDebugMessenger();

	bool isDeviceSuitable(VkPhysicalDevice device);

	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	void pickPhysicalDevice();

	int rateDeviceSuitability(VkPhysicalDevice device);



	std::vector<const char*>getRequiredExtensions();

	bool checkValidationLayerSupport();

	void createLogicalDevice();

	void createSurface();


	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);


	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createSwapChain();

	void createImageViews();

	void createGraphicsPipeline();

	static std::vector<char>readFile(const std::string& filename);

	VkShaderModule createShaderModule(const std::vector<char>& code);
	 
	void createRenderPass();


	void createFramebuffers();

	void createCommandPool();

	void createCommandBuffers();

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	void drawFrame();

	void updateUniformBuffer(uint32_t currentImage);
	void updateUniformBuffer1(uint32_t currentImage);

	void createSyncObjects();

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void createIndexBuffer();

	void createVertexBuffer();

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void createDescriptorSetLayout();

	void createUniformBuffers();

	void createDescriptorPool();

	void createDescriptorSets();

	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
		VkImage& image, VkDeviceMemory& imageMemory);

	void createTextureImage();

	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	VkCommandBuffer beginSingleTimeCommands();

	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	void createTextureImageView();

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

	void createTextureSampler();

	void createDepthResources();

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	VkFormat findDepthFormat();

	bool hasStencilComponent(VkFormat format);


	void loadModel();

	VkSampleCountFlagBits getMaxUsableSampleCount();

	void createColorResources();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);


};
