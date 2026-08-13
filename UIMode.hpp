


#pragma once
#include <string_view>



enum class UIMode {

	AddRoad,
	Select,
	View
};


constexpr std::string_view to_string(UIMode uiMode) {
    
    switch (uiMode) {
    case UIMode::AddRoad: return "Adding roads";
    case UIMode::Select:  return "Selecting vertex";
    case UIMode::View:    return "Viewing";
    default:              return "Invalid mode";
    }
}


