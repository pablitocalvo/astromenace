/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2018 Mikhail Kurinnoi, Viewizard


	AstroMenace is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	AstroMenace is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef ALIENSPACEFIGHTER_H
#define ALIENSPACEFIGHTER_H

#include "../space_ship.h"



//-----------------------------------------------------------------------------
// Класс cAlienSpaceFighter
//-----------------------------------------------------------------------------
class cAlienSpaceFighter : public cSpaceShip
{
public:

	virtual ~cAlienSpaceFighter() {};

	// Создание нужного объекта
	virtual void	Create(int	SpaceShipNum);

};




//-----------------------------------------------------------------------------
// Дополнительные функции для cAlienSpaceFighter
//-----------------------------------------------------------------------------

// Создание двигателя
void SetAlienSpaceFighterEngine(cParticleSystem *ParticleSystem, int EngineType);




#endif // ALIENSPACEFIGHTER_H
