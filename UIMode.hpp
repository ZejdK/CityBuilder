


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

inline UIMode getNextMode(UIMode uiMode) {

    switch (uiMode) {
    case UIMode::AddRoad:       return UIMode::Select;
    case UIMode::Select:        return UIMode::View;
    case UIMode::View:          return UIMode::AddRoad;
    }

    return UIMode::View;
}


