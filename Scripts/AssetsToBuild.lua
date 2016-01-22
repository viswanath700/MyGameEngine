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
			"ceiling",
			"cement",
			"floor",
			"lambert2",
			"lambert3",
			"metal",
			"railing",
			"walls",
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
			"ceiling",
			"cement",
			"floor",
			"lambert2",
			"lambert3",
			"metal",
			"railing",
			"walls",
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
			"cement_wall",
			"floor",
			"metal_brace",
			"railing",
			"wall",
		},
		dependencies =
		{
		},
	},
}