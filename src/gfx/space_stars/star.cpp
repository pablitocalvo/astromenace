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

#include "star.h"



//-----------------------------------------------------------------------------
//	обновление информации частицы
//-----------------------------------------------------------------------------
bool sStar::Update(float TimeDelta)
{

	if (Show) {
		Alpha += AlphaDelta * TimeDelta;
		if (Alpha >= 1.0f)
			Show = false;
	} else {
		Alpha -= AlphaDelta * TimeDelta;
		if (Alpha <= 0.3f)
			Show = true;
	}
	vw_Clamp(Alpha, 0.0f, 1.0f);


	// если пришли сюда - значит все хорошо и частица работает
	return true;
}


//-----------------------------------------------------------------------------
//	переносим положение частицы на указаное растояние
//-----------------------------------------------------------------------------
void sStar::Move(sVECTOR3D NewLocationDelta)
{
	// просто делаем приращение
	Location += NewLocationDelta;
}
