


#pragma once
#include <string>
#include <optional>



struct CitizenState {

	int pathId;
	bool insideJunction;
	float s; // parameter showing distance progress along the edge
	bool indicateRight;
	bool indicateLeft;
};

class Citizen {

	// data
	int id;
	std::string name;
	std::string surname;
	std::string colour;

	std::optional<CitizenState> state;

public:
	Citizen(int id, std::string name, std::string surname, std::string colour)
		: id(id), name(name), surname(surname), colour(colour), state(std::nullopt) {}

	void activate(int pathId) {

		state = CitizenState(pathId, false, 0.f);
	}

	void update(const CitizenState &newState) {

		state = newState;
	}



	int getId() const { return id; }
	std::string getColour() const { return colour; }



	bool isActive() const { return state != std::nullopt; }
	int getPathId() const { return state->pathId; }
	bool isInsideJunction() const { return state->insideJunction; }
	float getS() const { return state->s; }
	bool isIndicatingRight() const { return state->indicateRight; }
	bool isIndicatingLeft() const { return state->indicateLeft; }
};


