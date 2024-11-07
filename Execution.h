#pragma once
#include "Component.h"

using namespace std;
using namespace loader;

namespace Execution {
	void Main() {
		//���CD
		if (Base::Chronoscope::CheckChronoscope("BlinkCD")) {
			//��׼״̬��˫��W
			if (Base::Keyboard::CheckKey(87, 2) and Base::PlayerData::AimingState and Base::PlayerData::Weapons::WeaponType != 9) {
				Component::MovePlayerToPoint(
					Base::Vector3(
					Base::PlayerData::Coordinate::Collimator.x,
					Base::PlayerData::Coordinate::Collimator.y,
					Base::PlayerData::Coordinate::Collimator.z
					),
					Component::Effect{ true,2003,5 }
				);
				//���5��CD
				Base::Chronoscope::AddChronoscope(5.0, "BlinkCD", true);
			}
		}
		else
			//CD���������CD��Ч
			Base::PlayerData::Effects::GenerateSpecialEffects(2002, 0);
	}
}