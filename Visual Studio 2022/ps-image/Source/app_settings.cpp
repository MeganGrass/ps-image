/*
*
*	Megan Grass
*	December 14, 2024
* 
*/

#include "app.h"

void Global_Application::Tooltip(String Tip)
{
	ImGui::SameLine(); ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(Tip.c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void Global_Application::TooltipOnHover(String Tip)
{
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
	{
		ImGui::BeginTooltip();
		ImGui::Text(Tip.c_str());
		ImGui::EndTooltip();
	}
}

bool Global_Application::ScrollOnHover(void* Input, ImGuiDataType DataType, std::uintmax_t Step, std::uintmax_t Min, std::uintmax_t Max) const
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
		ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

		if (Window->Device()->GetMouseDeltaZ() != 0.0f)
		{
			bool b_Add = false;

			if (Window->Device()->GetMouseDeltaZ() > 0.0f)
			{
				b_Add = true;
			}

			if (DataType == ImGuiDataType_S8)
			{
				if (!b_Add && !*(int8_t*)Input) { return false; }
				*(int8_t*)Input = std::clamp(b_Add ? *(int8_t*)Input += (int8_t)Step : *(int8_t*)Input -= (int8_t)Step, (int8_t)Min, (int8_t)Max);
			}
			else if (DataType == ImGuiDataType_U8)
			{
				if (!b_Add && !*(uint8_t*)Input) { return false; }
				*(uint8_t*)Input = std::clamp(b_Add ? *(uint8_t*)Input += (uint8_t)Step : *(uint8_t*)Input -= (uint8_t)Step, (uint8_t)Min, (uint8_t)Max);
			}
			else if (DataType == ImGuiDataType_S16)
			{
				if (!b_Add && !*(int16_t*)Input) { return false; }
				*(int16_t*)Input = std::clamp(b_Add ? *(int16_t*)Input += (int16_t)Step : *(int16_t*)Input -= (int16_t)Step, (int16_t)Min, (int16_t)Max);
			}
			else if (DataType == ImGuiDataType_U16)
			{
				if (!b_Add && !*(uint16_t*)Input) { return false; }
				*(uint16_t*)Input = std::clamp(b_Add ? *(uint16_t*)Input += (uint16_t)Step : *(uint16_t*)Input -= (uint16_t)Step, (uint16_t)Min, (uint16_t)Max);
			}
			else if (DataType == ImGuiDataType_S32)
			{
				if (!b_Add && !*(int32_t*)Input) { return false; }
				*(int32_t*)Input = std::clamp(b_Add ? *(int32_t*)Input += (int32_t)Step : *(int32_t*)Input -= (int32_t)Step, (int32_t)Min, (int32_t)Max);
			}
			else if (DataType == ImGuiDataType_U32)
			{
				if (!b_Add && !*(uint32_t*)Input) { return false; }
				*(uint32_t*)Input = std::clamp(b_Add ? *(uint32_t*)Input += (uint32_t)Step : *(uint32_t*)Input -= (uint32_t)Step, (uint32_t)Min, (uint32_t)Max);
			}
			else if (DataType == ImGuiDataType_S64)
			{
				if (!b_Add && !*(int64_t*)Input) { return false; }
				*(int64_t*)Input = std::clamp(b_Add ? *(int64_t*)Input += (int64_t)Step : *(int64_t*)Input -= (int64_t)Step, (int64_t)Min, (int64_t)Max);
			}
			else if (DataType == ImGuiDataType_U64)
			{
				if (!b_Add && !*(uint64_t*)Input) { return false; }
				*(uint64_t*)Input = std::clamp(b_Add ? *(uint64_t*)Input += Step : *(uint64_t*)Input -= Step, Min, Max);
			}
			else if (DataType == ImGuiDataType_Bool)
			{
				if (!b_Add && *(bool*)Input == false) { return false; }
				b_Add ? *(bool*)Input = true : *(bool*)Input = false;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(50));

			return true;
		}
	}

	return false;
}

bool Global_Application::ScrollFloatOnHover(void* Input, ImGuiDataType DataType, double Step, double Min, double Max) const
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
		ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

		if (Window->Device()->GetMouseDeltaZ() != 0.0f)
		{
			bool b_Add = false;

			if (Window->Device()->GetMouseDeltaZ() > 0.0f)
			{
				b_Add = true;
			}

			if (DataType == ImGuiDataType_Float)
			{
				if (!b_Add && *(float*)Input == 0.0f) { return false; }
				*(float*)Input = std::clamp(b_Add ? *(float*)Input += (float)Step : *(float*)Input -= (float)Step, (float)Min, (float)Max);
			}
			else if (DataType == ImGuiDataType_Double)
			{
				if (!b_Add && *(double*)Input == 0.0f) { return false; }
				*(double*)Input = std::clamp(b_Add ? *(double*)Input += Step : *(double*)Input -= Step, Min, Max);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(50));

			return true;
		}
	}

	return false;
}

void Global_Application::AdjustWidthInput(std::uint16_t& Width)
{
	std::uint16_t MinWidth = 1;
	if (b_Raw4bpp || (m_Texture && Texture().GetDepth() == 4))
	{
		if (Width % 4)
		{
			Width += 4 - (Width % 4);
		}
		MinWidth = 4;
	}
	else if (b_Raw8bpp || (m_Texture && Texture().GetDepth() == 8))
	{
		if (Width % 2)
		{
			Width += 2 - (Width % 2);
		}
		MinWidth = 2;
	}
	Width = std::clamp(Width, MinWidth, uint16_t(1024));
}

void Global_Application::AdjustHeightInput(std::uint16_t& Height)
{
	std::uint16_t MinHeight = 1;
	if (b_Raw4bpp || (m_Texture && Texture().GetDepth() == 4))
	{
		if (Height % 4)
		{
			Height += 4 - (Height % 4);
		}
		MinHeight = 4;
	}
	else if (b_Raw8bpp || (m_Texture && Texture().GetDepth() == 8))
	{
		if (Height % 2)
		{
			Height += 2 - (Height % 2);
		}
		MinHeight = 2;
	}
	Height = std::clamp(Height, MinHeight, uint16_t(512));
}