-- Flying camera driven by the keyboard.
--
-- Attach to an entity with a Camera component. The camera looks down its own -Z, so the
-- movement axes are taken out of the current rotation rather than assumed to be world
-- axes - otherwise turning would not change which way W goes.
--
--   W / S        forward, back
--   A / D        left, right
--   E / Q        up, down (world up, so it does not drift when pitched)
--   Arrow keys   look around
--   Left Shift   hold to move faster

local CameraController = {}

-- Units per second, and radians per second for the look.
CameraController.speed = 5.0
CameraController.sprint_multiplier = 3.0
CameraController.look_speed = math.radians(90.0)

-- Looking straight up or down loses the yaw axis, so the pitch stops just short of it.
local pitch_limit = math.radians(89.0)

-- Euler angles are kept on the instance rather than read back from the quaternion each
-- frame: that conversion is lossy near the poles and would make the look jitter.
--
-- Called from on_update as well as on_start, because a script attached from the inspector
-- while the scene is already running never gets an on_start.
function CameraController:ensure_angles()
	if self.pitch then
		return
	end

	local euler = self.entity:get_transform().rotation:euler()
	self.pitch = euler.x
	self.yaw = euler.y
end

function CameraController:on_start()
	self:ensure_angles()
end

function CameraController:on_update(dt)
	self:ensure_angles()

	local transform = self.entity:get_transform()

	self:look(dt)
	transform.rotation = quat.from_euler(vec3.new(self.pitch, self.yaw, 0.0))

	local direction = self:move_direction(transform.rotation)
	if direction:length_squared() == 0.0 then
		return
	end

	local speed = self.speed
	if input.is_key_pressed(key.LeftShift) then
		speed = speed * self.sprint_multiplier
	end

	-- Normalised so that holding two keys is not faster than holding one.
	transform.position = transform.position + direction:normalize() * (speed * dt)
end

function CameraController:look(dt)
	local step = self.look_speed * dt

	if input.is_key_pressed(key.Up) then
		self.pitch = self.pitch + step
	end
	if input.is_key_pressed(key.Down) then
		self.pitch = self.pitch - step
	end
	if input.is_key_pressed(key.Left) then
		self.yaw = self.yaw + step
	end
	if input.is_key_pressed(key.Right) then
		self.yaw = self.yaw - step
	end

	self.pitch = math.clamp(self.pitch, -pitch_limit, pitch_limit)
end

-- The unnormalised sum of every direction being held, in world space.
function CameraController:move_direction(rotation)
	-- Rotating the local axes gives where the camera is actually facing now.
	local forward = rotation * vec3.new(0.0, 0.0, -1.0)
	local right = rotation * vec3.new(1.0, 0.0, 0.0)
	local up = vec3.new(0.0, 1.0, 0.0)

	local direction = vec3.new(0.0, 0.0, 0.0)

	if input.is_key_pressed(key.W) then
		direction = direction + forward
	end
	if input.is_key_pressed(key.S) then
		direction = direction - forward
	end
	if input.is_key_pressed(key.D) then
		direction = direction + right
	end
	if input.is_key_pressed(key.A) then
		direction = direction - right
	end
	if input.is_key_pressed(key.E) then
		direction = direction + up
	end
	if input.is_key_pressed(key.Q) then
		direction = direction - up
	end

	return direction
end

return CameraController
