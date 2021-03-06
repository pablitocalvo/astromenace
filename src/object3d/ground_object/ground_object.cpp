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

#include "ground_object.h"


//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cGroundObject::cGroundObject()
{
	ObjectStatus = 1; // чужой

	// подключаем к своему списку
	AttachGroundObject(this);

}



//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
cGroundObject::~cGroundObject()
{
	if (WheelObjectsNum != nullptr) {
		delete [] WheelObjectsNum;
		WheelObjectsNum = nullptr;
	};
	if (WheelRotateObjectsNum != nullptr) {
		delete [] WheelRotateObjectsNum;
		WheelRotateObjectsNum = nullptr;
	};
	if (WeaponBound != nullptr) {
		delete [] WeaponBound;
		WeaponBound = nullptr;
	};

	if (TargetHorizObject != nullptr) {
		delete [] TargetHorizObject;
		TargetHorizObject = nullptr;
	};
	if (TargetVertObject != nullptr) {
		delete [] TargetVertObject;
		TargetVertObject = nullptr;
	};

	if (BarrelObject != nullptr) {
		delete [] BarrelObject;
		BarrelObject = nullptr;
	};

	DetachGroundObject(this);
}










//-----------------------------------------------------------------------------
// Установка положения объекта
//-----------------------------------------------------------------------------
void cGroundObject::SetLocation(sVECTOR3D NewLocation)
{
	// вызываем родительскую функцию
	::cObject3D::SetLocation(NewLocation);

	// если оружие вообще есть
	if (Weapon != nullptr)
		for (int i = 0; i < WeaponQuantity; i++) {
			if (Weapon[i] != nullptr)
				Weapon[i]->SetLocation(NewLocation + WeaponLocation[i]);
		}
}




//-----------------------------------------------------------------------------
// Установка углов поворота объекта
//-----------------------------------------------------------------------------
void cGroundObject::SetRotation(sVECTOR3D NewRotation)
{
	// вызываем родительскую функцию
	::cObject3D::SetRotation(NewRotation);

	// оружие
	sVECTOR3D RotationBase = Rotation;
	sVECTOR3D BaseBoundTMP = BaseBound;
	vw_RotatePoint(BaseBoundTMP, RotationBase);

	sVECTOR3D RotationMiddle = Rotation;
	sVECTOR3D MiddleBoundTMP = MiddleBound;
	if (TargetHorizObject != nullptr)
		RotationMiddle = ObjectsList[TargetHorizObject[0]].Rotation + Rotation;
	vw_RotatePoint(MiddleBoundTMP, RotationMiddle);

	sVECTOR3D RotationWeapon = Rotation;
	if (TargetVertObject != nullptr)
		RotationWeapon = ObjectsList[TargetVertObject[0]].Rotation + Rotation;


	if (Weapon != nullptr)
		for (int i = 0; i < WeaponQuantity; i++)
			if (Weapon[i] != nullptr) {
				sVECTOR3D WeaponBoundTMP = WeaponBound[i];
				vw_RotatePoint(WeaponBoundTMP, RotationWeapon);

				WeaponLocation[i] = BaseBoundTMP + MiddleBoundTMP + WeaponBoundTMP;


				// особый случай, испускаем без вращающихся частей (антиматерия, ион)
				if ((TargetHorizObject == nullptr) &&
				    (TargetVertObject == nullptr) &&
				    !DoNotCalculateRotation) // и если нужно считать...
					RotationWeapon = sVECTOR3D(TargetVertObjectNeedAngle, TargetHorizObjectNeedAngle, 0.0f) + Rotation;
				Weapon[i]->SetRotation(Weapon[i]->Rotation^(-1.0f));
				Weapon[i]->SetRotation(RotationWeapon);

				Weapon[i]->SetLocation(Location + WeaponLocation[i]);
			}
}









//-----------------------------------------------------------------------------
// Обновление данных объектa Object3D
//-----------------------------------------------------------------------------
bool cGroundObject::Update(float Time)
{
	// вызываем родительскую функцию
	// если там передали удалить - выходим
	if (!::cObject3D::Update(Time))
		return false;
	// быстро вызвали еще раз... время не изменилось, или почти не изменилось
	if (TimeDelta == 0.0f)
		return true;





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// обработка скрипта
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if ((StartTimeSheet != nullptr) &&
	    (!StartTimeSheet->InUse)) {
		StartTimeSheet->InUse = true;

		NeedSpeed = StartTimeSheet->Speed;
		NeedAcceler = StartTimeSheet->Acceler;
		NeedRotate = StartTimeSheet->Rotation;
		RotationSpeed = StartTimeSheet->RotationAcceler;
		WeaponTargeting = StartTimeSheet->Targeting;

		if (Weapon != nullptr)
			for (int i = 0; i < WeaponQuantity; i++) {
				if (Weapon[i] != nullptr)
					WeaponSetFire[i] = StartTimeSheet->Fire;
			}
	}













	// если нужно наводить на цель
	if (WeaponTargeting) {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// наводим на цель, если есть возможность
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		// находимся в начальном состоянии поворота ствола
		int WeapNum = 204; // номер самого простого из пиратского оружия
		sVECTOR3D FirePos(0.0f,0.0f,0.0f);
		if (Weapon != nullptr) {
			if (Weapon[0] != nullptr)
				WeapNum = Weapon[0]->ObjectCreationType;

			int Count = 0;
			for (int i = 0; i < WeaponQuantity; i++) {
				if (Weapon[i] != nullptr) {
					FirePos += WeaponLocation[i];
					Count++;
				}
			}
			FirePos = FirePos^(1.0f/Count);
		}
		sVECTOR3D NeedAngle(TargetVertObjectNeedAngle,TargetHorizObjectNeedAngle,0);
		if (GetTurretOnTargetOrientateion(ObjectStatus, Location+FirePos, Rotation,
						  CurrentRotationMat,	&NeedAngle, WeapNum)) {
			// наводим на цель
			TargetHorizObjectNeedAngle = NeedAngle.y;
			TargetVertObjectNeedAngle = NeedAngle.x;
		} else {
			// врагов нет, нужно просто поднять ствол
			TargetVertObjectNeedAngle = TargetVertObjectMaxAngle*0.5f;
		}
	} else {
		// устанавливаем в начальное положение
		TargetHorizObjectNeedAngle = 0.0f;
		TargetVertObjectNeedAngle = 0.0f;

	}


	// собственно повороты элементов модели на углы
	{
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// поворот башни по горизонтале
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if ((TargetHorizObject != nullptr) &&
		    (TargetHorizObjectNeedAngle != TargetHorizObjectCurrentAngle)) {
			if (fabsf(TargetHorizObjectNeedAngle-TargetHorizObjectCurrentAngle) > 180.0f) {
				if (TargetHorizObjectCurrentAngle - TargetHorizObjectNeedAngle > 180.0f)
					TargetHorizObjectCurrentAngle -= 360.0f;
				if (TargetHorizObjectNeedAngle - TargetHorizObjectCurrentAngle > 180.0f)
					TargetHorizObjectCurrentAngle += 360.0f;
			}

			// находим угол, на который нужно повернуть
			float NeedRotateCalculation = TargetHorizObjectCurrentAngle;

			if (TargetHorizObjectNeedAngle>TargetHorizObjectCurrentAngle) {
				NeedRotateCalculation += 80.0f*TimeDelta/GameNPCTargetingSpeedPenalty;
				if (NeedRotateCalculation > TargetHorizObjectNeedAngle)
					NeedRotateCalculation = TargetHorizObjectNeedAngle;
			} else {
				NeedRotateCalculation -= 80.0f*TimeDelta/GameNPCTargetingSpeedPenalty;
				if (NeedRotateCalculation < TargetHorizObjectNeedAngle)
					NeedRotateCalculation = TargetHorizObjectNeedAngle;
			}

			// устанавливаем текущий поворот
			TargetHorizObjectCurrentAngle = NeedRotateCalculation;

			// поворачиваем все объекты
			for (int i=0; i<TargetHorizObjectQuantity; i++) {

				sVECTOR3D tmp = ObjectsList[TargetHorizObject[i]].Location - ObjectsList[TargetHorizObject[0]].Location;

				vw_RotatePointInv(tmp, ObjectsList[TargetHorizObject[i]].Rotation ^ (-1.0f));

				SetObjectRotation(sVECTOR3D(ObjectsList[TargetHorizObject[i]].Rotation.x,
							   -NeedRotateCalculation,
							   ObjectsList[TargetHorizObject[i]].Rotation.z), TargetHorizObject[i]);

				vw_RotatePoint(tmp, ObjectsList[TargetHorizObject[i]].Rotation);

				SetObjectLocation(tmp + ObjectsList[TargetHorizObject[0]].Location, TargetHorizObject[i]);
			}
		}





		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// поворот стволов по вертикали
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if ((TargetVertObject != nullptr) &&
		    (TargetVertObjectNeedAngle != TargetVertObjectCurrentAngle)) {

			// находим угол, на который нужно повернуть
			float NeedRotateCalculation = TargetVertObjectCurrentAngle;
			if (TargetVertObjectNeedAngle>TargetVertObjectCurrentAngle) {
				NeedRotateCalculation += 80.0f*TimeDelta/GameNPCTargetingSpeedPenalty;
				if (NeedRotateCalculation > TargetVertObjectNeedAngle)
					NeedRotateCalculation = TargetVertObjectNeedAngle;
				if (NeedRotateCalculation > TargetVertObjectMaxAngle)
					NeedRotateCalculation = TargetVertObjectMaxAngle;
			} else {
				NeedRotateCalculation -= 80.0f*TimeDelta/GameNPCTargetingSpeedPenalty;
				if (NeedRotateCalculation < TargetVertObjectNeedAngle)
					NeedRotateCalculation = TargetVertObjectNeedAngle;
				if (NeedRotateCalculation < TargetVertObjectMinAngle)
					NeedRotateCalculation = TargetVertObjectMinAngle;
			}

			// устанавливаем текущий поворот
			TargetVertObjectCurrentAngle = NeedRotateCalculation;

			// поворачиваем все объекты
			for (int i = 0; i < TargetVertObjectQuantity; i++) {

				sVECTOR3D tmp = ObjectsList[TargetVertObject[i]].Location - ObjectsList[TargetVertObject[0]].Location;

				vw_RotatePointInv(tmp, ObjectsList[TargetVertObject[i]].Rotation ^ (-1.0f));

				SetObjectRotation(sVECTOR3D(-NeedRotateCalculation,
							   ObjectsList[TargetVertObject[i]].Rotation.y,
							   ObjectsList[TargetVertObject[i]].Rotation.z), TargetVertObject[i]);

				vw_RotatePoint(tmp, ObjectsList[TargetVertObject[i]].Rotation);

				SetObjectLocation(tmp + ObjectsList[TargetVertObject[0]].Location, TargetVertObject[i]);
			}
		}
	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// находим точку стрельбы и учитываем направление
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	sVECTOR3D RotationBase = Rotation;
	sVECTOR3D BaseBoundTMP = BaseBound;
	vw_RotatePoint(BaseBoundTMP, RotationBase);

	sVECTOR3D RotationMiddle = Rotation;
	sVECTOR3D MiddleBoundTMP = MiddleBound;
	if (TargetHorizObject != nullptr)
		RotationMiddle = ObjectsList[TargetHorizObject[0]].Rotation + Rotation;
	vw_RotatePoint(MiddleBoundTMP, RotationMiddle);

	sVECTOR3D RotationWeapon = Rotation;
	if (TargetVertObject != nullptr)
		RotationWeapon = ObjectsList[TargetVertObject[0]].Rotation + Rotation;


	if (Weapon != nullptr)
		for (int i = 0; i < WeaponQuantity; i++)
			if (Weapon[i] != nullptr) {
				sVECTOR3D WeaponBoundTMP = WeaponBound[i];
				vw_RotatePoint(WeaponBoundTMP, RotationWeapon);

				WeaponLocation[i] = BaseBoundTMP + MiddleBoundTMP + WeaponBoundTMP;

				// особый случай, испускаем без вращающихся частей (антиматерия, ион)
				if ((TargetHorizObject == nullptr) &&
				    (TargetVertObject == nullptr) &&
				    !DoNotCalculateRotation) // и если нужно считать...
					RotationWeapon = Rotation - sVECTOR3D(TargetVertObjectNeedAngle, TargetHorizObjectNeedAngle, 0.0f);


				Weapon[i]->SetRotation(Weapon[i]->Rotation^(-1.0f));
				Weapon[i]->SetRotation(RotationWeapon);

				Weapon[i]->SetLocation(Location + WeaponLocation[i]);
			}






	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// смотрим, есть ли команда открыть огонь
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (Weapon != nullptr) {
		// если залп или игрок (игроку регулируем сами)
		if (WeaponFireType == 1) {
			for (int i=0; i<WeaponQuantity; i++) {
				if ((Weapon[i] != nullptr) &&
				    (WeaponSetFire[i]))
					Weapon[i]->WeaponFire(Time);
			}
		} else { // переменный огонь
			if (WeaponFireType == 2) {

				int PrimCount = 0;
				float PrimTime = 0.0f;
				int FirstWeapon = 6;
				int LastWeapon = 0;

				WeaponGroupCurrentFireDelay -= TimeDelta;

				// находим кол-во оружия
				for (int i=0; i<WeaponQuantity; i++) {
					if (Weapon[i] != nullptr) {
						PrimCount++;
						PrimTime += Weapon[i]->NextFireTime;
						if (FirstWeapon > i)
							FirstWeapon = i;
						if (LastWeapon < i)
							LastWeapon = i;
					}
				}
				// если еще не было начальной установки
				if (WeaponGroupCurrentFireNum == -1)
					WeaponGroupCurrentFireNum = FirstWeapon;


				// стреляем
				for (int i = 0; i < WeaponQuantity; i++) {
					if ((Weapon[i] != nullptr) &&
					    (WeaponSetFire[i])) {
						if (WeaponGroupCurrentFireNum == i &&
						    WeaponGroupCurrentFireDelay <= 0.0f) {
							Weapon[i]->WeaponFire(Time);

							WeaponGroupCurrentFireDelay = (PrimTime/PrimCount)*((1.0f+GameNPCWeaponPenalty)/2.0f);
							WeaponGroupCurrentFireNum ++;
							if (WeaponGroupCurrentFireNum > LastWeapon)
								WeaponGroupCurrentFireNum = FirstWeapon;

							// если такого оружия нет, берем что есть
							if (Weapon[WeaponGroupCurrentFireNum] == nullptr) {
								bool exit = false;
								while (!exit) {
									WeaponGroupCurrentFireNum ++;
									if (WeaponGroupCurrentFireNum > LastWeapon)
										WeaponGroupCurrentFireNum = FirstWeapon;
									if (Weapon[WeaponGroupCurrentFireNum] != nullptr)
										exit = true;
								}
							}
						}
					}
				}
			} else { // переменный огонь2 (залп ракет или чего-то еще)
				if (WeaponFireType == 3) {

					int PrimCount = 0;
					float PrimTime = 0.0f;
					int FirstWeapon = 6;
					int LastWeapon = 0;

					WeaponGroupCurrentFireDelay -= TimeDelta;

					// находим кол-во оружия
					for (int i=0; i<WeaponQuantity; i++) {
						if (Weapon[i] != nullptr) {
							PrimCount++;
							PrimTime += Weapon[i]->NextFireTime;
							if (FirstWeapon > i)
								FirstWeapon = i;
							if (LastWeapon < i)
								LastWeapon = i;
						}
					}
					// если еще не было начальной установки
					if (WeaponGroupCurrentFireNum == -1)
						WeaponGroupCurrentFireNum = FirstWeapon;


					// стреляем
					for (int i = 0; i < WeaponQuantity; i++) {
						if ((Weapon[i] != nullptr) &&
						    (WeaponSetFire[i])) {
							if (WeaponGroupCurrentFireNum == i &&
							    WeaponGroupCurrentFireDelay <= 0.0f) {
								Weapon[i]->WeaponFire(Time);

								WeaponGroupCurrentFireDelay = PrimTime/(PrimCount*PrimCount);
								WeaponGroupCurrentFireNum ++;
								if (WeaponGroupCurrentFireNum > LastWeapon)
									WeaponGroupCurrentFireNum = FirstWeapon;

								// если такого оружия нет, берем что есть
								if (Weapon[WeaponGroupCurrentFireNum] == nullptr) {
									bool exit = false;
									while (!exit) {
										WeaponGroupCurrentFireNum ++;
										if (WeaponGroupCurrentFireNum > LastWeapon)
											WeaponGroupCurrentFireNum = FirstWeapon;
										if (Weapon[WeaponGroupCurrentFireNum] != nullptr)
											exit = true;
									}
								}
							}
						}
					}
				}
			}
		}
	}







	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// вращение стволов пулемета
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if ((BarrelObjectQuantity != 0) &&
	    (BarrelObject != nullptr)) {
		for (int i = 0; i < BarrelObjectQuantity; i++) {
			ObjectsList[BarrelObject[i]].NeedGeometryAnimation = true;
			ObjectsList[BarrelObject[i]].GeometryAnimation += sVECTOR3D(0.0f,0.0f,500.0f*TimeDelta);
			if (ObjectsList[BarrelObject[i]].GeometryAnimation.z > 360.0f)
				ObjectsList[BarrelObject[i]].GeometryAnimation.z -= 360.0f;
		}
	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// вращение колес в транспорте
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (((WheelObjectsNum != nullptr) &&
	    ((WheelTrackSpeed >= 0.00001f) || (WheelTrackSpeed <= -0.00001f)))) {
		// перебираем все и ув. их угол вращения
		for (int i = 0; i < WheelQuantity; i++) {
			ObjectsList[WheelObjectsNum[i]].Rotation.x += WheelTrackSpeed*TimeDelta;

			if (ObjectsList[WheelObjectsNum[i]].Rotation.x > 360.0f)
				ObjectsList[WheelObjectsNum[i]].Rotation.x -= 360.0f;
			if (ObjectsList[WheelObjectsNum[i]].Rotation.x < -360.0f)
				ObjectsList[WheelObjectsNum[i]].Rotation.x += 360.0f;
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// тайловая анимация для траков
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (TrackObjectNum != -1 && (WheelTrackSpeed >= 0.00001f || WheelTrackSpeed <= -0.00001f)) {
		ObjectsList[TrackObjectNum].NeedTextureAnimation = true;
		ObjectsList[TrackObjectNum].TextureAnimation.x += (WheelTrackSpeed / 500.0f) * TimeDelta * TrackRotationDirection;
		if (ObjectsList[TrackObjectNum].TextureAnimation.x > 1.0f)
			ObjectsList[TrackObjectNum].TextureAnimation.x -= 1.0f;
		if (ObjectsList[TrackObjectNum].TextureAnimation.x < -1.0f)
			ObjectsList[TrackObjectNum].TextureAnimation.x += 1.0f;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// повотор
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (NeedRotate.x != 0.0f || NeedRotate.y != 0.0f || NeedRotate.z != 0.0f) {
		// Находим допустимый поворот по углу
		sVECTOR3D tmpRotate(0.0f, 0.0f, 0.0f);

		// угол по x
		if (NeedRotate.x != 0.0f) {
			float Sign = 1.0f;
			if (NeedRotate.x < 0.0f) Sign = -1.0f;
			// вычисляем скорость поворота по данным двигателя
			tmpRotate.x = Sign*MaxSpeedRotate*RotationSpeed.x*TimeDelta;
			// смотрим, если уже повернули - снимаем
			if (Sign == 1.0f) {
				if (tmpRotate.x >= NeedRotate.x) {
					tmpRotate.x = NeedRotate.x;
					NeedRotate.x = 0.0f;
				}
			} else {
				if (tmpRotate.x <= NeedRotate.x) {
					tmpRotate.x = NeedRotate.x;
					NeedRotate.x = 0.0f;
				}
			}
			// меняем значение
			if (NeedRotate.x != 0.0f) NeedRotate.x -= tmpRotate.x;
		}

		// угол по y
		if (NeedRotate.y != 0.0f) {
			float Sign = 1.0f;
			if (NeedRotate.y < 0.0f) Sign = -1.0f;
			// вычисляем скорость поворота по данным двигателя
			tmpRotate.y = Sign*MaxSpeedRotate*RotationSpeed.y*TimeDelta;
			// смотрим, если уже повернули - снимаем
			if (Sign == 1.0f) {
				if (tmpRotate.y >= NeedRotate.y) {
					tmpRotate.y = NeedRotate.y;
					NeedRotate.y = 0.0f;
				}
			} else {
				if (tmpRotate.y <= NeedRotate.y) {
					tmpRotate.y = NeedRotate.y;
					NeedRotate.y = 0.0f;
				}
			}
			// меняем значение
			if (NeedRotate.y != 0.0f) NeedRotate.y -= tmpRotate.y;
		}


		// угол по z
		if (NeedRotate.z != 0.0f) {
			float Sign = 1.0f;
			if (NeedRotate.z < 0.0f) Sign = -1.0f;
			// вычисляем скорость поворота по данным двигателя
			tmpRotate.z = Sign*MaxSpeedRotate*RotationSpeed.z*TimeDelta;
			// смотрим, если уже повернули - снимаем
			if (Sign == 1.0f) {
				if (tmpRotate.z >= NeedRotate.z) {
					tmpRotate.z = NeedRotate.z;
					NeedRotate.z = 0.0f;
				}
			} else {
				if (tmpRotate.z <= NeedRotate.z) {
					tmpRotate.z = NeedRotate.z;
					NeedRotate.z = 0.0f;
				}
			}
			// меняем значение
			if (NeedRotate.z != 0.0f) NeedRotate.z -= tmpRotate.z;
		}

		// установка поворота там же сохраняем, какой общий поворот модели
		SetRotation(tmpRotate);





		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// поворот колес в транспорте
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (WheelRotateObjectsNum != nullptr) {
			// перебираем все и ув. их угол вращения
			for (int i = 0; i < WheelRotateQuantity; i++) {
				float NeedRotateY = ObjectsList[WheelRotateObjectsNum[i]].Rotation.y;
				if (ObjectsList[WheelRotateObjectsNum[i]].Rotation.y > NeedRotate.y) {
					NeedRotateY -= 90.0f*TimeDelta;
					if (NeedRotateY < NeedRotate.y)
						NeedRotateY = NeedRotate.y;
				} else if (ObjectsList[WheelRotateObjectsNum[i]].Rotation.y < NeedRotate.y) {
					NeedRotateY += 90.0f*TimeDelta;
					if (NeedRotateY > NeedRotate.y)
						NeedRotateY = NeedRotate.y;
				}

				if (NeedRotateY>MaxWheelRotateAngle)
					NeedRotateY = MaxWheelRotateAngle;
				if (NeedRotateY<-MaxWheelRotateAngle)
					NeedRotateY = -MaxWheelRotateAngle;


				SetObjectRotation(sVECTOR3D(ObjectsList[WheelRotateObjectsNum[i]].Rotation.x,
							   NeedRotateY,
							   ObjectsList[WheelRotateObjectsNum[i]].Rotation.z), WheelRotateObjectsNum[i]);

			}
		}

	}









	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ускорение-замедление
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// если нужно разогнаться, или управление на игроке - и нужно стремиться к нулю
	if (NeedSpeed != 0.0f) {
		float Sign = 1.0f;
		// нужно двигать назад
		if (NeedSpeed < 0.0f) Sign = -1.0f;

		// "ровняем" скорость под модель
		if (Sign == 1.0f) {
			vw_Clamp(NeedSpeed, 0.0f, MaxSpeed);
		} else {
			vw_Clamp(NeedSpeed, -MaxSpeed, 0.0f);
		}

		// случай, когда нужно затормозить а не менять направление
		if (Sign == 1.0f) {
			if (NeedSpeed < Speed) Sign = -1.0f;
		} else {
			if (NeedSpeed > Speed) Sign = 1.0f;
		}


		// даем полный газ, учитывая сколько процентов можем выдать
		Acceler = Sign*MaxAcceler*NeedAcceler;

		// считаем текущую скорость
		Speed = Speed + Acceler*TimeDelta;

		// смотрим, если уже разогнались - снимаем
		if (Sign == 1.0f) {
			if (NeedSpeed <= Speed) {
				Speed = NeedSpeed;
				NeedSpeed = 0.0f;
			}
		} else {
			if (NeedSpeed >= Speed) {
				Speed = NeedSpeed;
				NeedSpeed = 0.0f;
			}
		}
	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// небольшая девиация-болтание колес, если нужно
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (DeviationOn)
		for (int i=0; i<DeviationObjQuantity; i++) {
			float Sign = 1.0f;
			// нужно двигать
			if (NeedDeviation[i] < 0.0f) Sign = -1.0f;
			if (Sign == 1.0f) {
				if (NeedDeviation[i] < CurentDeviationSum[i]) Sign = -1.0f;
			} else {
				if (NeedDeviation[i] > CurentDeviationSum[i]) Sign = 1.0f;
			}

			CurentDeviation[i] = Sign*0.35f*TimeDelta;

			if (Sign == 1.0f) {
				if (NeedDeviation[i] <= CurentDeviationSum[i]+CurentDeviation[i]) {
					CurentDeviation[i] -= CurentDeviationSum[i]+CurentDeviation[i]-NeedDeviation[i];
					CurentDeviationSum[i] += CurentDeviation[i];
					NeedDeviation[i] = vw_Randf0*0.1f;
				} else CurentDeviationSum[i] += CurentDeviation[i];
			} else {
				if (NeedDeviation[i] >= CurentDeviationSum[i]+CurentDeviation[i]) {
					CurentDeviation[i] += CurentDeviationSum[i]+CurentDeviation[i]-NeedDeviation[i];
					CurentDeviationSum[i] += CurentDeviation[i];
					NeedDeviation[i] = vw_Randf0*0.1f;
				} else CurentDeviationSum[i] += CurentDeviation[i];
			}

			sVECTOR3D Tmp = Deviation[i]^CurentDeviation[i];
			SetObjectLocation(ObjectsList[DeviationObjNum[i]].Location + Tmp, DeviationObjNum[i]);
		}










	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// считаем вектор движения
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//VECTOR3D Velocity = (Orientation^(Speed*TimeDelta))+(sVECTOR3D(0.0f,0.0f,10.0f)^TimeDelta);
	sVECTOR3D Velocity = (Orientation^(Speed*TimeDelta));

	// перемещение объекта, если нужно
	if (Velocity.x != 0.0f || Velocity.y != 0.0f  || Velocity.z != 0.0f ) {
		WheelTrackSpeed = Speed*SpeedToRotate;
		// делаем сдвиг модели в указанную точку
		SetLocation(Location+Velocity);
	}





	// объект в порядке - удалять не нужно
	return true;
}



