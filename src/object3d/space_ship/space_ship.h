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

#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "../object3d.h"
#include "../weapon/weapon.h"




//-----------------------------------------------------------------------------
// Класс cSpaceShip
//-----------------------------------------------------------------------------
class cSpaceShip : public cObject3D
{
public:
	// базовые конструктор и деструктор объекта
	cSpaceShip();
	virtual ~cSpaceShip();

	// Обновление данных объектa
	virtual bool	Update(float Time);
	// Установка положения объекта
	virtual void	SetLocation(sVECTOR3D NewLocation);
	virtual void	SetLocationArcadePlayer(sVECTOR3D NewLocation);
	// Установка углов поворота объекта
	virtual void	SetRotation(sVECTOR3D NewRotation);

	// текущей вектор движения корабля
	sVECTOR3D	Velocity{0.0f, 0.0f, 0.0f};

	// нужно повернуть (пересчитать мэш) при следующем проходе
	// Update + учесть эти данные в Rotation
	sVECTOR3D	NeedRotate{0.0f, 0.0f, 0.0f};
	// скорость поворота по каждому раправлению
	sVECTOR3D	RotationSpeed{1.0f, 1.0f, 1.0f};

	// максимальная скорость units/sec, зависит от двигателя
	float	MaxSpeed{0.0f};
	// максимальное ускорение units/sec*sec, зависит от двигателя
	float	MaxAcceler{0.0f};
	// макс. маневровых двигателей
	float	MaxSpeedRotate{0.0f};

	// вперед-назад
	// текущая скорость
	float	Speed{0.0f};
	// нужная скорость
	float	NeedSpeed{0.0f};
	// текущее ускорение
	float	Acceler{0.0f};
	// нужное ускорение
	float	NeedAcceler{1.0f};

	// влево-вправо
	// текущая скорость
	float	SpeedLR{0.0f};
	// нужная скорость
	float	NeedSpeedLR{0.0f};
	// текущее ускорение
	float	AccelerLR{0.0f};
	// нужное ускорение
	float	NeedAccelerLR{1.0f};

	// вверх-вниз
	// текущая скорость
	float	SpeedUD{0.0f};
	// нужная скорость
	float	NeedSpeedUD{0.0f};
	// текущее ускорение
	float	AccelerUD{0.0f};
	// нужное ускорение
	float	NeedAccelerUD{1.0f};

	// вперед-назад по вектору камеры
	// текущая скорость
	float	SpeedByCamFB{0.0f};
	// нужная скорость
	float	NeedSpeedByCamFB{0.0f};
	// текущее ускорение
	float	AccelerByCamFB{0.0f};
	// нужное ускорение
	float	NeedAccelerByCamFB{1.0f};

	// влево-вправо
	// текущая скорость
	float	SpeedByCamLR{0.0f};
	// нужная скорость
	float	NeedSpeedByCamLR{0.0f};
	// текущее ускорение
	float	AccelerByCamLR{0.0f};
	// нужное ускорение
	float	NeedAccelerByCamLR{1.0f};

	// вверх-вниз
	// текущая скорость
	float	SpeedByCamUD{0.0f};
	// нужная скорость
	float	NeedSpeedByCamUD{0.0f};
	// текущее ускорение
	float	AccelerByCamUD{0.0f};
	// нужное ускорение
	float	NeedAccelerByCamUD{1.0f};

	// управление - куда перемещаться при сделующем Update'е
	// от 0.0 до 1.0, по "силе" движения в нужную сторону
	float	MoveForward{0.0f};
	float	MoveBackward{0.0f};
	float	MoveLeft{0.0f};
	float	MoveRight{0.0f};
	float	MoveUp{0.0f};
	float	MoveDown{0.0f};

	// кол-во оружия доступного на данной моделе
	int		WeaponQuantity{0};
	// выстрел из оружия, т.е. передача команды "стрелять" оружию при сделующем Update'е
	bool		*WeaponSetFire{nullptr};
	// указатель на массив оружия
	cWeapon		**Weapon{nullptr};
	// расположение оружия на коробле (относительное)
	sVECTOR3D	*WeaponLocation{nullptr};
	// тип оружия по мощьности, для определенных слотов
	int		*WeaponType{nullptr};
	// первоначальный доворот оружия для корабля (будем использовать только для игрка, для остальных он ноль)
	float		*WeaponYAngle{nullptr};
	// тип стрельбы из оружия 1-обычный, 2-переменный (по умолчанию)
	int		WeaponFireType{2};
	int 		WeaponGroupCurrentFireNum{-1};
	float 		WeaponGroupCurrentFireDelay{0.0f};
	// поворот оружия
	bool		NeedWeaponRotate{true};

	// кол-во оружия доступного на данной моделе
	int		BossWeaponQuantity{0};
	// выстрел из оружия, т.е. передача команды "стрелять" оружию при сделующем Update'е
	bool		*BossWeaponSetFire{nullptr};
	// указатель на массив оружия
	cWeapon		**BossWeapon{nullptr};
	// расположение оружия на коробле (относительное)
	sVECTOR3D	*BossWeaponLocation{nullptr};
	// тип оружия по мощьности, для определенных слотов
	int		*BossWeaponType{nullptr};
	// первоначальный доворот оружия для корабля (будем использовать только для игрка, для остальных он ноль)
	float		*BossWeaponYAngle{nullptr};
	// тип стрельбы из оружия 1-обычный, 2-переменный (по умолчанию)
	int		BossWeaponFireType{2};
	int 		BossWeaponGroupCurrentFireNum{-1};
	float 		BossWeaponGroupCurrentFireDelay{0.0f};

	// выстрел из оружия-фларес, т.е. передача команды "стрелять" оружию при сделующем Update'е
	bool		WeaponFlareSetFire{false};
	// указатель на оружие-фларе
	cWeapon		*WeaponFlare{nullptr};
	// расположение оружия на коробле (относительное)
	sVECTOR3D	WeaponFlareLocation{0.0f, 0.0f, 0.0f};

	// кол-во двигателей
	int		EngineQuantity{0}; // FIXME remove, we have std::vector::size() now
	// тип, как будем удалять двигатели -сразу, или глушить
	bool		EngineDestroyType{false};
	// двигатели
	std::vector<cParticleSystem*> Engine{};
	// положение двигателей
	sVECTOR3D	*EngineLocation{nullptr};

	// вкл. двигатели торможения при повороте
	bool	NeedStopRotation{false};

	// двигатели поворотов, левый
	int		EngineLeftQuantity{0}; // кол-во двигателей // FIXME remove, we have std::vector::size() now
	std::vector<cParticleSystem*> EngineLeft{}; // двигатели
	sVECTOR3D	*EngineLeftLocation{nullptr}; // положение двигателей

	// двигатели поворотов, правый
	int		EngineRightQuantity{0}; // кол-во двигателей  // FIXME remove, we have std::vector::size() now
	std::vector<cParticleSystem*> EngineRight{}; // двигатели
	sVECTOR3D	*EngineRightLocation{nullptr}; // положение двигателей

	// для собственного списка
	cSpaceShip	*Next{nullptr};
	cSpaceShip	*Prev{nullptr};
};


//-----------------------------------------------------------------------------
// Менеджер cSpaceShip, дополнительный
//-----------------------------------------------------------------------------

// Включаем в список
void	AttachSpaceShip(cSpaceShip* SpaceShip);
// Исключаем из списка
void	DetachSpaceShip(cSpaceShip* SpaceShip);
// Находим нужный по ID
cSpaceShip *FindSpaceShipByID(int ID);
// Проверяем все объекты, обновляем данные
void	UpdateAllSpaceShip(float Time);
// Прорисовываем все объекты
void	DrawAllSpaceShip(bool VertexOnlyPass, unsigned int ShadowMap, int DrawOnlyType=-1);
// Удаляем все объекты в списке
void	ReleaseAllSpaceShip();

#endif // SPACESHIP_H
