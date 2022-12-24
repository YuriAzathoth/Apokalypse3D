/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2022 Yuriy Zinchenko

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <utility>
#include "InputComponents.h"
#include "MouseComponents.h"
#include "MouseSystems.h"

using namespace A3D::Components::Input;
using namespace A3D::Components::Mouse;

A3D::MouseSystems::MouseSystems(flecs::world& world)
{
	world.module<MouseSystems>("A3D::Systems::Mouse");
	world.import<InputComponents>();
	world.import<MouseComponents>();

	flecs::query actions = world.query<ActionKey, const Button>();
	updateButtons_ = world.observer<const ButtonsState>("UpdateButtons")
					 .event(flecs::OnSet)
					 .each([actions = std::move(actions)](const ButtonsState& mouse)
	{
		actions.each([&mouse](flecs::entity e, ActionKey& action, const Button& button)
		{
			action.current = mouse.down & (1 << button.code);
			if (action.previous != action.current)
				e.modified<ActionKey>();
		});
	});

	flecs::query axesX = world.query_builder<ActionAxis>().term<AxisX>().build();
	flecs::query axesY = world.query_builder<ActionAxis>().term<AxisY>().build();
	updateMovement_ = world.observer<const Movement>("UpdateMovement")
					  .event(flecs::OnSet)
					  .each([axesX = std::move(axesX), axesY = std::move(axesY)](const Movement& movement)
	{
		axesX.each([&movement](flecs::entity e, ActionAxis& axis)
		{
			axis.delta += movement.dx;
			e.modified<ActionAxis>();
		});
		axesY.each([&movement](flecs::entity e, ActionAxis& axis)
		{
			axis.delta += movement.dy;
			e.modified<ActionAxis>();
		});
	});

	flecs::query axesWheel = world.query_builder<ActionAxis>().term<Wheel>().build();
	updateWheel_ = world.observer<const WheelState>("UpdateWheel")
				   .event(flecs::OnSet)
				   .each([axesWheel = std::move(axesWheel)](const WheelState& wheel)
	{
		axesWheel.each([&wheel](flecs::entity e, ActionAxis& axis)
		{
			axis.delta += wheel.delta;
			e.modified<ActionAxis>();
		});
	});

	world.add<ButtonsState>();
	world.add<Movement>();
	world.add<WheelState>();
}
