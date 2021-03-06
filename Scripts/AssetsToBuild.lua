return
{
	meshes =
	{
		builder = "MeshBuilder.exe",
		sourcePath = "",
		targetPath = "",
		sourceExtension = ".luaMesh",
		targetExtension = ".binMesh",
		assets =
		{
			"boxes",
			"ceiling",
			"floor",
			"innerWalls",
			"metal",
			"outerWalls",
			"railing",
			"cylinder1",
			"cylinder2",
			"snowman",
			"collisionData",
			"flag1",
			"flag2",
			"bullet",
		},
		dependencies =
		{
		},
	},
	vertexShader =
	{
		builder = "VertexShaderBuilder.exe",
		sourcePath = "",
		targetPath = "",
		sourceExtension = ".shader",
		targetExtension = ".shader",
		assets =
		{
			"vertex",
		},
		dependencies =
		{
			"shaders.inc",
		},
	},
	fragmentShader =
	{
		builder = "FragmentShaderBuilder.exe",
		sourcePath = "",
		targetPath = "",
		sourceExtension = ".shader",
		targetExtension = ".shader",
		assets =
		{
			"fragment",
			"transparency",
		},
		dependencies =
		{
			"shaders.inc",
		},
	},
	effects =
	{
		builder = "EffectBuilder.exe",
		sourcePath = "",
		targetPath = "",
		sourceExtension = ".luaEffect",
		targetExtension = ".binEffect",
		assets =
		{
			"effect",
			"transparentEffect",
		},
		dependencies =
		{
		},
	},
	materials =
	{
		builder = "MaterialBuilder.exe",
		sourcePath = "",
		targetPath = "",
		sourceExtension = ".luaMaterial",
		targetExtension = ".binMaterial",
		assets =
		{
			"boxes",
			"ceiling",
			"floor",
			"innerWalls",
			"metal",
			"outerWalls",
			"railing",
			"cylinder1",
			"cylinder2",
			"snowman",
			"snowmanClient",
			"flag1",
			"flag2",
			"bullet1",
			"bullet2",
		},
		dependencies =
		{
		},
	},
	textures =
	{
		builder = "TextureBuilder.exe",
		sourcePath = "",
		targetPath = "",
		sourceExtension = ".png",
		targetExtension = ".texture",
		assets =
		{
			"boxes",
			"floor",
			"innerWalls",
			"metal",
			"outerWalls",
			"railing",
			"logo",
			"numbers",
			"checkbox",
			"button",
			"earth",
			"football",
			"flag1",
			"flag2",
		},
		dependencies =
		{
		},
	},
}