CONTROLS:
	{ CAMERA }
		[Mouse]
			Left-click + drag up	Move camera forward
			Left-click + drag down	Move camera backwards
		[Keyboard, Press]
			W						Move forward
			A						Move left (strafe)
			S						Move backwards
			D						Move right (strafe)
			Up-Arrow				Rotate up on right-vector (yaw)
			Down-Arrow				Rotate down on right-vector (yaw)
			Right-Arrow				Rotate right on up-vector (pitch)
			Left-Arrow				Rotate left on up-vector (pitch)
			E						Reset camera to default position and orientation
			
	{ MODEL }
		[Keyboard, Press]
			I						+Y translation
			J						-X translation
			K						-Y translation
			L						+X translation
			Pg-Up					+Z translation
			Pg-Dn					-Z translation
			B						X rotation
			N						Y rotation
			M						Z rotation
			R						Reset Model to default position, scale, orientation
		[Keyboard, Release]
			O						Scale the model by a factor of -10%
			P						Scale the model by a factor of +10%
			1						Toggle Red Channel On/Off
			2						Toggle Green Channel On/Off
			3						Toggle Blue Channel On/Off
			4						Turn all color channels On 
			5						Toggle between Gouraud and Phong shading
			6						Toggle shading/lighting On/Off
			7						Toggle vertex color based on normals, or flat grey 
			8						Toggle draw with triangles or points
			9						Toggle Toggle using modelspace vs worldspace normals for vertex color
			z						Decrease ambient coefficient 
			x						Increase ambient coefficient
			c						Decrease diffuse coefficient
			v						Increase diffuse coefficient
			f						Decrease specular coefficient
			g						Increase specular coefficient
			q						Reset ambient, diffuse, and specular coefficients to default (0.25,1.0,1.0) 