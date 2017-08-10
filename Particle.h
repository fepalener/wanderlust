#ifndef _Particle_h_
#define _Particle_h_

#include <SFML\System.hpp>
#include <SFML\Graphics.hpp>

struct Particle 
{
	enum Type
	{
		Propellant,
		Smoke,
		ParticleCount
	};

	sf::Vector2f	position;
	sf::Color		color;
	sf::Time		lifetime;
};

#endif