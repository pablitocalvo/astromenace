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

#include "../base.h"
#include <random> // this one should not being used in other parts of code

namespace {

std::random_device rd;
std::mt19937 gen(rd());

} // unnamed namespace

/*
 * Generate random float between 0.0f and 1.0f.
 */
float vw_fRand()
{
	return std::generate_canonical<float, 10>(gen);
}

/*
 * Generate random float between 0.0f and Max.
 */
float vw_fRandNum(float Max)
{
	return vw_fRand() * Max;
}

/*
 * Generate random integer between 0 and Max.
 */
int vw_iRandNum(int Max)
{
	return floor(vw_fRand() * (Max + 1));
}
