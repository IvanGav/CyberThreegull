#pragma once
#include "CyberThreegull_decl.h"
#include "VK.h"
#include "Win32.h"
#include "DynamicVertexBuffer.h"
#include "TextRenderer.h"
#include "WASAPIInterface.h"
#include "SerializeTools.h"
#include "Sounds.h"


namespace CyberThreegull {

U64 frameNumber;
U64 prevFrameTime;
U64 frameTime;
F64 deltaTime;
F64 totalTime;

HANDLE audioThread;
F64 audioPlaybackTime;
B32 audioThreadShouldShutdown;

F32* testAudio;
U32 testAudioLength;

V2F32 lastMouse;
V2F32 userMouse;

V2F32 testPos;
V2F32 testPosVelocity;

V2F32 rayPos;
V2F32 rayEnd;

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
}
void mouse_callback(Win32::MouseButton button, Win32::MouseValue state) {
	V2F32 mousePos = Win32::get_mouse();
}

void draw_world() {
	testPosVelocity = normalize(userMouse - testPos) * 0.5F;
	V2F32 colliderPos{ 100.0F, 100.0F };
	F32 colliderRadius = 20.0F;
	F32 intersection = ray_intersect_circle(colliderPos, colliderRadius, testPos, testPosVelocity);
	if (intersection != F32_INF) {
		V2F32 intersectPos = testPos + intersection * testPosVelocity + normalize(-testPosVelocity) * 0.01;
		V2F32 hitNormal = normalize(intersectPos - colliderPos);
		if (length_sq(intersectPos - testPos) < length_sq(testPosVelocity)) {
			testPosVelocity = intersectPos - testPos;
		}
	}
	testPos += testPosVelocity;

	if (Win32::keyboardState[Win32::KEY_CTRL]) {
		rayPos = userMouse;
	} else {
		rayEnd = userMouse;
	}

	

	DynamicVertexBuffer::Tessellator& tes = DynamicVertexBuffer::get_tessellator();
	tes.begin_draw(VK::basicPipeline, VK::basicPipelineLayout, DynamicVertexBuffer::DRAW_MODE_TRIANGLES);
	tes.basic_circle(V2F32{ 100.0F, 100.0F }, 20.0F, 10, V4F32{1.0F, 0.0F, 0.0F, 1.0F}, Textures::simpleWhite.index);
	tes.basic_circle(userMouse, 15.0F, 10, V4F32{ 1.0F, 1.0F, 0.0F, 1.0F }, Textures::simpleWhite.index);
	tes.basic_circle(testPos, 5.0F, 10, V4F32{ 1.0F, 1.0F, 1.0F, 1.0F }, Textures::simpleWhite.index);
	intersection = ray_intersect_circle(colliderPos, colliderRadius, rayPos, rayEnd - rayPos);
	V2F32 end = intersection == F32_INF ? rayEnd : rayPos + (rayEnd - rayPos) * intersection;
	for (U32 i = 0; i < 100; i++) {
		tes.basic_circle(rayPos + (end - rayPos) * (F32(i) / 100.0F), 3.0F, 10, V4F32{0.0F, 1.0F, 1.0F, 1.0F}, Textures::simpleWhite.index);
	}
	tes.basic_circle(rayPos, 5.0F, 10, V4F32{ 0.0F, 1.0F, 0.0F, 1.0F }, Textures::simpleWhite.index);
	tes.basic_circle(rayEnd, 5.0F, 10, V4F32{ 0.0F, 0.5F, 0.0F, 1.0F }, Textures::simpleWhite.index);
	tes.end_draw();
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

	V2F32 mousePos = Win32::get_mouse();
	V2F32 mouseDelta = Win32::get_delta_mouse();

	lastMouse = userMouse;
	userMouse = mousePos;

	VK::FrameBeginResult beginAction = VK::begin_frame();
	if (beginAction == VK::FRAME_BEGIN_RESULT_TRY_AGAIN) {
		beginAction = VK::begin_frame();
	}
	if (beginAction == VK::FRAME_BEGIN_RESULT_CONTINUE) {

		VkViewport viewport;
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = F32(VK::mainFramebuffer.framebufferWidth);
		viewport.height = F32(VK::mainFramebuffer.framebufferHeight);
		viewport.minDepth = 0.0F;
		viewport.maxDepth = 1.0F;
		VK::vkCmdSetViewport(VK::graphicsCommandBuffer, 0, 1, &viewport);
		VkRect2D scissor{};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = VK::mainFramebuffer.framebufferWidth;
		scissor.extent.height = VK::mainFramebuffer.framebufferHeight;
		VK::vkCmdSetScissor(VK::graphicsCommandBuffer, 0, 1, &scissor);

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
		VK::vkCmdBeginRenderPass(VK::graphicsCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		draw_world();

		DynamicVertexBuffer::get_tessellator().draw();

		VK::vkCmdEndRenderPass(VK::graphicsCommandBuffer);

		VK::end_frame();
	} else {
		Sleep(1);
	}
}

void run_cyber_seaquell() {
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
		VK::finish_startup();
	}

	Win32::show_window();
	Win32::set_cursor(Win32::CURSOR_TYPE_POINTER);

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
}

}