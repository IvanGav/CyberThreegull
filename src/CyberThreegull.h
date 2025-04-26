#pragma once
#include "CyberThreegull_decl.h"
#include "DrillLib.h"
#include "SerializeTools.h"
#include "VK.h"
#include "Win32.h"
#include "DynamicVertexBuffer.h"
#include "TextRenderer.h"
#include "WASAPIInterface.h"
#include "Resources.h"
#include "Sounds.h"
#include "Terminal/Terminal.h"


namespace CyberThreegull {

U64 frameNumber;
U64 prevFrameTime;
U64 frameTime;
F64 deltaTime;
F64 totalTime;

HANDLE audioThread;
F64 audioPlaybackTime;
B32 audioThreadShouldShutdown;

V2F32 lastMouse;
V2F32 userMouse;

ArenaArrayList<VKGeometry::StaticModel> staticModelsToRender;
ArenaArrayList<VKGeometry::SkeletalModel*> skeletalModelsToRender;

struct TermInterface {
	V3F32 p1;
	V3F32 p2;
	V3F32 p3;
};

TermInterface term0{ {-5.78971F,0.07619F,-5.27076F}, {-5.78971F,0.992912F,-5.27076F}, {-5.1248F, 0.07619F, -6.42548F} };

struct LineCollider {
	V2F32 a;
	V2F32 b;
};

V2F32 closest_on_segment(V2F32 p, LineCollider c) {
	V2F32 v = c.b - c.a;
	F32 proj = dot(p - c.a, v) / length_sq(v);
	if (proj <= 0.0F) {
		return c.a;
	} else if (proj >= 1.0F) {
		return c.b;
	} else {
		return c.a + v * proj;
	}
}

ArenaArrayList<LineCollider> colliders;

struct Player {
	V2F32 pos;
	V2F32 velocity;
	V2F32 forward;
	V2F32 right;
	F32 yaw;
	F32 pitch;
	F32 radius;
} player;

void fill_audio_buffer(F32* buffer, U32 numSamples, U32 numChannels, F32 timeAmount) {
	Sounds::mix_into_buffer(buffer, numSamples, numChannels, timeAmount);
	audioPlaybackTime += timeAmount;
}

DWORD WINAPI audio_thread_func(LPVOID) {
	WASAPIInterface::init_wasapi(fill_audio_buffer);
	while (!audioThreadShouldShutdown) {
		WASAPIInterface::do_audio();
	}
	return 0;
}

void keyboard_callback(Win32::Key key, Win32::ButtonState state) {
	V2F32 mousePos = Win32::get_mouse();
	if (state == Win32::BUTTON_STATE_DOWN) {
		typeChar(key, Win32::key_to_typed_char(key));
	}
}
void mouse_callback(Win32::MouseButton button, Win32::MouseValue state) {
	V2F32 mousePos = Win32::get_mouse();
}

void draw_static_model(VKGeometry::StaticModel& model) {
	staticModelsToRender.push_back(model);
}

void setup_scene() {
	player.pos = V2F32{ 0.0F, 0.0F };
	player.yaw = 0.25F;
	player.radius = 0.25F;

	colliders.push_back(LineCollider{ {1.0F, 1.0F}, {1.0F, 5.0F} });
}

void draw_world() {
	for (LineCollider& c : colliders) {
		for (U32 i = 0; i <= 5; i++) {
			VKGeometry::StaticModel model;
			VKGeometry::make_static_model(&model, Resources::cubeMesh);
			V2F32 pos = c.a + (c.b - c.a) * F32(i) / 5;
			model.transform.translate({pos.x, 1.0F, pos.y});
			model.transform.scale({ 0.1F, 0.1F, 0.1F });
			model.color = V4F32{ 1.0F, 0.0F, 0.0F, 1.0F};
			draw_static_model(model);
		}
	}
	{
		VKGeometry::StaticModel model;
		VKGeometry::make_static_model(&model, Resources::cubeMesh);
		model.transform.translate(term0.p1);
		model.transform.scale({ 0.1F, 0.1F, 0.1F });
		model.color = V4F32{ 1.0F, 0.0F, 0.0F, 1.0F };
		draw_static_model(model);
	}
}

void update_world() {
	V2F32 deltaMouse = Win32::get_raw_delta_mouse() * 0.0005F;
	player.yaw += deltaMouse.x;
	player.pitch = clamp(player.pitch + deltaMouse.y, -0.2499F, 0.2499F);
	player.forward = V2F32{ sinf32(player.yaw), -cosf32(player.yaw) };
	player.right = V2F32{ sinf32(player.yaw + 0.25F), -cosf32(player.yaw + 0.25F) };
	player.velocity = {};
	if (Win32::keyboardState[Win32::KEY_W]) {
		player.velocity += player.forward * 5.0F;
	}
	if (Win32::keyboardState[Win32::KEY_A]) {
		player.velocity += -player.right * 5.0F;
	}
	if (Win32::keyboardState[Win32::KEY_S]) {
		player.velocity += -player.forward * 5.0F;
	}
	if (Win32::keyboardState[Win32::KEY_D]) {
		player.velocity += player.right * 5.0F;
	}
	V2F32 step = player.velocity * deltaTime;
	V2F32 newPos = player.pos + step;
	for (LineCollider c : colliders) {
		V2F32 closest = closest_on_segment(newPos, c);
		if (distance_sq(closest, newPos) >= player.radius * player.radius) {
			continue;
		}
		step += normalize(newPos - closest) * (player.radius - length(newPos - closest));
		newPos = player.pos + step;
	}
	player.pos = newPos;
}

void do_frame() {
	frameNumber++;
	LARGE_INTEGER perfCounter;
	if (!QueryPerformanceCounter(&perfCounter)) {
		abort("Could not get performance counter");
	}
	prevFrameTime = frameTime;
	frameTime = U64(perfCounter.QuadPart);
	deltaTime = F64(frameTime - prevFrameTime) / F64(performanceCounterTimerFrequency);
	totalTime += deltaTime;

	U64 frameArenaFrame0 = frameArena.stackPtr;
	const U32 initialModelToRenderCapacity = 32;
	staticModelsToRender.reset();
	staticModelsToRender.reserve(initialModelToRenderCapacity);
	skeletalModelsToRender.reset();
	skeletalModelsToRender.reserve(initialModelToRenderCapacity);

	V2F32 mousePos = Win32::get_mouse();
	lastMouse = userMouse;
	userMouse = mousePos;

	update_world();

	PerspectiveProjection proj;
	proj.project_perspective(0.05F, DEG_TO_TURN(120.0F), F32(Win32::framebufferHeight) / F32(Win32::framebufferWidth));

	M4x3F32 view;
	view.set_identity();
	view.rotate_axis_angle(V3F32{ 1.0F, 0.0F, 0.0F }, -player.pitch);
	view.rotate_axis_angle(V3F32{ 0.0F, 1.0F, 0.0F }, -player.yaw);
	view.translate(-V3F32{ player.pos.x, 0.5F, player.pos.y });

	ProjectiveTransformMatrix projView;
	projView.generate(proj, view);

	VK::FrameBeginResult beginAction = VK::begin_frame();
	if (beginAction == VK::FRAME_BEGIN_RESULT_TRY_AGAIN) {
		beginAction = VK::begin_frame();
	}
	if (beginAction == VK::FRAME_BEGIN_RESULT_CONTINUE) {
		VkCommandBuffer cmdBuf = VK::graphicsCommandBuffer;

		VkViewport viewport;
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = F32(VK::mainFramebuffer.framebufferWidth);
		viewport.height = F32(VK::mainFramebuffer.framebufferHeight);
		viewport.minDepth = 0.0F;
		viewport.maxDepth = 1.0F;
		VK::vkCmdSetViewport(cmdBuf, 0, 1, &viewport);
		VkRect2D scissor{};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = VK::mainFramebuffer.framebufferWidth;
		scissor.extent.height = VK::mainFramebuffer.framebufferHeight;
		VK::vkCmdSetScissor(cmdBuf, 0, 1, &scissor);

		VkRenderPassBeginInfo renderPassBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		renderPassBeginInfo.renderPass = VK::mainRenderPass;
		renderPassBeginInfo.framebuffer = VK::mainFramebuffer.framebuffer;
		renderPassBeginInfo.renderArea = VkRect2D{ VkOffset2D{ 0, 0 }, VkExtent2D{ VK::mainFramebuffer.framebufferWidth, VK::mainFramebuffer.framebufferHeight } };
		renderPassBeginInfo.clearValueCount = 2;
		VkClearValue clearValues[2];
		clearValues[0].color.float32[0] = 0.0F;
		clearValues[0].color.float32[1] = 0.0F;
		clearValues[0].color.float32[2] = 0.0F;
		clearValues[0].color.float32[3] = 0.0F;
		clearValues[1].depthStencil.depth = 0.0F;
		clearValues[1].depthStencil.stencil = 0;
		renderPassBeginInfo.pClearValues = clearValues;
		VK::vkCmdBeginRenderPass(cmdBuf, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		draw_world();

		VK::vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, VK::basicPipeline);
		VK::vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, VK::basicPipelineLayout, 0, 1, &VK::drawDataDescriptorSet.descriptorSet, 0, nullptr);

		VkBuffer geoBuffer = VK::geometryHandler.buffer;
		VK::vkCmdBindIndexBuffer(cmdBuf, geoBuffer, VK::geometryHandler.indicesOffset, VK_INDEX_TYPE_UINT16);
		
		V2F32 screenSize{ Win32::framebufferWidth, Win32::framebufferHeight };

		for (VKGeometry::StaticModel& model : staticModelsToRender) {
			VK::BasicPipelineRenderData modelInfo{
				VK::geometryHandler.gpuAddress + VK::geometryHandler.positionsOffset,
				VK::geometryHandler.gpuAddress + VK::geometryHandler.texcoordsOffset,
				VK::geometryHandler.gpuAddress + VK::geometryHandler.normalsOffset,
				projView,
				model.transform,
				Textures::simpleWhite.index,
				pack_unorm4x8(model.color)
			};
			VK::vkCmdPushConstants(cmdBuf, VK::basicPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VK::BasicPipelineRenderData), &modelInfo);
			VK::vkCmdDrawIndexed(cmdBuf, model.mesh->indicesCount, 1, model.mesh->indicesOffset, I32(model.mesh->verticesOffset), 0);
		}

		M4x3F32 roomTransform;
		roomTransform.set_identity();
		VK::BasicPipelineRenderData modelInfo{
				VK::geometryHandler.gpuAddress + VK::geometryHandler.positionsOffset,
				VK::geometryHandler.gpuAddress + VK::geometryHandler.texcoordsOffset,
				VK::geometryHandler.gpuAddress + VK::geometryHandler.normalsOffset,
				projView,
				roomTransform,
				Textures::simpleWhite.index,
				pack_unorm4x8(V4F32{1.0F, 1.0F, 1.0F, 1.0F})
		};
		VK::vkCmdPushConstants(cmdBuf, VK::basicPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VK::BasicPipelineRenderData), &modelInfo);
		Resources::rooms.draw_all(cmdBuf);
		modelInfo.texIdx = Textures::brainTexture.index;
		VK::vkCmdPushConstants(cmdBuf, VK::basicPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VK::BasicPipelineRenderData), &modelInfo);
		//Resources::rooms.draw_named(cmdBuf, "Sphere.001"a);
		
		DynamicVertexBuffer::Tessellator& tes = DynamicVertexBuffer::get_tessellator();

		printf("x%\n"a, distance(term0.p1, term0.p3));
		printf("y%\n"a, distance(term0.p1, term0.p2));
		{
			M4x3F32 textTransform;
			textTransform.set_identity();
			textTransform.translate(term0.p2);
			textTransform.set_row(0, -normalize(term0.p3 - term0.p1));
			textTransform.set_row(1, normalize(term0.p2 - term0.p1));
			textTransform.set_row(2, V3F32{0.0F, 0.0F, 1.0F});
			textTransform.transpose_rotation();
			File& f = getTerminal();
			tes.begin_draw(VK::uiPipeline, VK::uiPipelineLayout, DynamicVertexBuffer::DRAW_MODE_QUADS, textTransform);
			F32 offset = 0.0F;
			for (ArenaArrayList<char>& s : f) {
				TextRenderer::draw_string_batched(tes, StrA{ s.data, s.size }, 0.03F, offset, -0.01F, 0.05F, V4F32{ 0.0F, 0.0f, 0.0F, 1.0F }, 0);
				offset += 0.05F;
			}
			tes.end_draw();
		}

		tes.draw(projView);

		VK::vkCmdEndRenderPass(VK::graphicsCommandBuffer);

		VK::end_frame();

		for (VKGeometry::SkeletalModel* model : skeletalModelsToRender) {
			model->poseMatrices = nullptr;
		}
		frameArena.stackPtr = frameArenaFrame0;
	} else {
		Sleep(1);
	}
}

void run_cyber_threequell() {
	if (Win32::pTimeBeginPeriod) {
		Win32::pTimeBeginPeriod(1);
	}
	Sounds::load_sources();
	audioThread = CreateThread(NULL, 64 * KILOBYTE, audio_thread_func, NULL, 0, NULL);
	if (audioThread == NULL) {
		DWORD err = GetLastError();
		print("Failed to create audio thread, code: ");
		println_integer(err);
		return;
	}

	LARGE_INTEGER perfCounter;
	if (!QueryPerformanceCounter(&perfCounter)) {
		abort("Could not get performanceCounter");
	}
	frameTime = prevFrameTime = U64(perfCounter.QuadPart);

	if (!Win32::init(1920 / 2, 1080 / 2, do_frame, keyboard_callback, mouse_callback)) {
		return;
	}
	PNG::init_loader();
	LOG_TIME("VK Init Time: ") {
		VK::init_vulkan();
	}
	LOG_TIME("Asset Load Time: ") {
		VK::load_pipelines_and_descriptors();
		Textures::load_all();
		Resources::load_resources();
		VK::finish_startup();
	}

	terminalsInit();
	openTerminal(0);
	typeChar(Win32::KEY_0, '0');
	setup_scene();
	staticModelsToRender.allocator = &frameArena;
	skeletalModelsToRender.allocator = &frameArena;

	Win32::show_window();
	Win32::set_cursor(Win32::CURSOR_TYPE_POINTER);
	Win32::set_mouse_captured(true);

	while (!Win32::windowShouldClose) {
		Win32::poll_events();
		do_frame();
	}

	audioThreadShouldShutdown = true;
	if (WaitForSingleObject(audioThread, INFINITE) == WAIT_FAILED) {
		DWORD err = GetLastError();
		print("Failed to join audio thread, code: ");
		println_integer(err);
	} else {
		CloseHandle(audioThread);
	}
	CHK_VK(VK::vkDeviceWaitIdle(VK::logicalDevice));
	VK::end_vulkan();
	Win32::destroy();
	if (Win32::pTimeEndPeriod) {
		Win32::pTimeEndPeriod(1);
	}
}

}